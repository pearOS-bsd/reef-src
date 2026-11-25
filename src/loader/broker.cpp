// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The Mach ports of a session.  `reef --mach-broker` keeps who holds which right to which port and moves the messages between
// the processes over a local socket, with the descriptors that go with them; it also keeps the bootstrap names and answers what
// the sandbox of a program has to ask.  A port name is global: its high half is the process that made it.

#include "host.h"
#include "sandbox.h"

#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <deque>
#include <fcntl.h>
#include <string>
#include <sys/epoll.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cutefish::reef {

namespace {

// The version of what goes over the socket: it is part of the socket's name.
constexpr int BrokerVersion = 2;

enum : uint32_t {
    // A process sends a message to a port.
    SendMessage = 1,
    // The broker gives it to the receiver.
    ReceivedMessage = 2,
    // A process gives up send and send-once rights.
    ReleaseRights = 3,
    // The receiver destroys its port.
    DestroyPort = 4,
    // To the holders of rights: the port is gone.
    DeadName = 5,
    // To the receiver: no send right is left.
    NoSenders = 6,
    // A send-once right goes away without a message.
    DestroySendOnce = 7,
    // Bootstrap: a name for a port, or no port to take the name away.
    CheckIn = 8,
    LookUp = 9,
    Reply = 10,
    // The answer comes after everything that the other processes sent before.
    Synchronize = 11,
    // Sandbox: a profile, a new connection of the same program, extensions, files.
    AddProfile = 12,
    AddConnection = 13,
    IssueExtension = 14,
    ConsumeExtension = 15,
    ReleaseExtension = 16,
    OpenFile = 17,
    ChangeFile = 18,
    // A receive right moved: to the process that sent it, and back from it when nothing of it is on the way any more.
    ReceiveMoved = 19,
};

// Dispositions of rights, as in Mach.
enum : uint32_t {
    MoveReceive = 16,
    MoveSend = 17,
    MoveSendOnce = 18,
    CopySend = 19,
    MakeSend = 20,
    MakeSendOnce = 21,
};

// Header flags.
constexpr uint32_t InMemory = 1;
constexpr uint32_t ReplaceName = 2;
constexpr uint32_t CreatePort = 4;
constexpr uint32_t Unaudited = 8;

// Descriptor flags.
constexpr uint32_t NewPort = 1;
constexpr uint32_t KeepSend = 2;
constexpr uint32_t HasSenders = 4;
constexpr uint32_t DeadPort = 8;
constexpr uint32_t NotPort = 0x10;
constexpr uint32_t OwnReceiver = 0x20;

constexpr uint32_t BootstrapNotPrivileged = 1100;
constexpr uint32_t BootstrapUnknownService = 1102;
constexpr uint32_t BootstrapServiceActive = 1103;

// A message: this header, its descriptors, the body, then a name.  A big message is in a memory file, the last descriptor.
struct Header
{
    uint32_t type;
    uint32_t descriptorCount;
    uint64_t remote;
    uint64_t local;
    uint64_t value;
    uint32_t remoteDisposition;
    uint32_t flags;
    int32_t pid;
    uint32_t uid;
    uint32_t gid;
    uint32_t unknown34;
    uint32_t bodySize;
    uint32_t nameSize;
};

// A right in a message.
struct Descriptor
{
    uint64_t name;
    uint32_t disposition;
    uint32_t flags;
    uint64_t unknown10[2];
};

struct Connection;

// What a connection could not take yet.
struct Outgoing
{
    std::vector<char> data;
    std::vector<int> fds;
};

// The rights of one connection to a port.
struct Rights
{
    uint64_t sends;
    uint64_t sendOnces;
    // Send-once rights not used: each that goes away unused tells the receiver.
    uint64_t unusedSendOnces;
};

// A message for a port whose receive right is on its way to another process.
struct Pending
{
    Connection* to;
    std::vector<char> data;
    std::vector<int> fds;
};

struct Port
{
    Connection* receiver;
    std::unordered_map<Connection*, Rights> senders;
    // Bootstrap names of the port: they count as send rights.
    int names;
    uint64_t madeSends;
    bool noSendersSent;
    // The process that sent the receive right, until it says that nothing of the port is on the way any more.
    Connection* transit;
    std::deque<Pending> pending;
};

struct Connection
{
    int fd = -1;
    int pid = 0;
    uint32_t uid = 0;
    uint32_t gid = 0;
    std::deque<Outgoing> queue;
    bool writing = false;
    std::unordered_set<uint64_t> receives;
    std::unordered_set<uint64_t> sends;
    // The bootstrap names it registered.
    std::unordered_map<std::string, uint64_t> names;
    SandboxPolicy sandbox;
};

// A process that waits for Synchronize.
struct Waiter
{
    int fd;
    Connection* connection;
    uint64_t value;
};

struct Broker
{
    int epoll = -1;
    int listener = -1;
    std::unordered_map<int, Connection*> connections;
    std::unordered_map<uint64_t, Port> ports;
    std::unordered_map<std::string, uint64_t> services;
    SandboxTokens tokens;
    std::vector<char> buffer = std::vector<char>(0x1f000);
    bool synchronizing = false;
    std::vector<Waiter> waiters;
    // Ports whose receive right moved and whose old receiver did not answer yet.
    std::unordered_set<uint64_t> transits;
};

} // namespace

static long receiveMessage(int fd, void* data, size_t size, int* fds, uint32_t* count, int flags)
{
    iovec vector = {data, size};
    alignas(cmsghdr) char control[1024];
    msghdr message = {};
    message.msg_iov = &vector;
    message.msg_iovlen = 1;
    message.msg_control = control;
    message.msg_controllen = sizeof(control);
    long result;
    while ((result = recvmsg(fd, &message, flags | MSG_CMSG_CLOEXEC)) < 0) {
        if (errno != EINTR) {
            *count = 0;
            return -errno;
        }
    }
    uint32_t received = 0;
    for (cmsghdr* header = CMSG_FIRSTHDR(&message); header; header = CMSG_NXTHDR(&message, header)) {
        if (header->cmsg_level != SOL_SOCKET || header->cmsg_type != SCM_RIGHTS)
            continue;
        const uint32_t n = (header->cmsg_len - CMSG_LEN(0)) / sizeof(int);
        const int* given = reinterpret_cast<const int*>(CMSG_DATA(header));
        for (uint32_t i = 0; i < n; i++) {
            if (received < *count)
                fds[received++] = given[i];
            else
                close(given[i]);
        }
    }
    *count = received;
    return result;
}

static long machReceive(int fd, void* data, size_t size, int* fds, uint32_t* count)
{
    return receiveMessage(fd, data, size, fds, count, 0);
}

// The broker runs on its own, outside the session of the program.
static void startBroker()
{
    char path[4096];
    const ssize_t size = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (size <= 0)
        return;
    path[size] = 0;
    const pid_t child = fork();
    if (child < 0)
        return;
    if (child == 0) {
        setsid();
        if (fork() == 0) {
            const int null = open("/dev/null", O_RDWR);
            if (null >= 0) {
                dup2(null, 0);
                dup2(null, 1);
                dup2(null, 2);
            }
            close_range(3, ~0U, 0);
            sigset_t mask;
            sigemptyset(&mask);
            sigprocmask(SIG_SETMASK, &mask, nullptr);
            char* environment[] = {nullptr};
            execle(path, "reef", "--mach-broker", nullptr, environment);
            _exit(127);
        }
        _exit(0);
    }
    while (waitpid(child, nullptr, 0) < 0 && errno == EINTR) {
    }
}

static int connectTo(const std::string& path)
{
    const int fd = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);
    if (fd < 0)
        return -errno;
    sockaddr_un address = {};
    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, sizeof(address.sun_path), "%s", path.c_str());
    if (connect(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        const int error = errno;
        close(fd);
        return -error;
    }
    return fd;
}

// A process of the same user: it gets a connection.
static Connection* addConnection(Broker& broker, int fd)
{
    ucred credentials = {};
    socklen_t length = sizeof(credentials);
    if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &credentials, &length) != 0 || credentials.uid != getuid()) {
        close(fd);
        return nullptr;
    }
    int bufferSize = 4 << 20;
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
    auto* connection = new Connection;
    connection->fd = fd;
    connection->pid = credentials.pid;
    connection->uid = credentials.uid;
    connection->gid = credentials.gid;
    broker.connections[fd] = connection;
    epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = fd;
    epoll_ctl(broker.epoll, EPOLL_CTL_ADD, fd, &event);
    return connection;
}

static long sendMessage(int fd, const void* data, size_t size, const int* fds, uint32_t count)
{
    iovec vector = {const_cast<void*>(data), size};
    msghdr message = {};
    message.msg_iov = &vector;
    message.msg_iovlen = 1;
    std::vector<char> control;
    if (count) {
        control.resize(CMSG_SPACE(count * sizeof(int)));
        cmsghdr* header = reinterpret_cast<cmsghdr*>(control.data());
        header->cmsg_level = SOL_SOCKET;
        header->cmsg_type = SCM_RIGHTS;
        header->cmsg_len = CMSG_LEN(count * sizeof(int));
        memcpy(CMSG_DATA(header), fds, count * sizeof(int));
        message.msg_control = control.data();
        message.msg_controllen = control.size();
    }
    long result;
    do {
        result = sendmsg(fd, &message, MSG_NOSIGNAL);
        if (result >= 0)
            return result;
    } while (errno == EINTR);
    return -errno;
}

static long machSend(int fd, const void* data, size_t size, const int* fds, uint32_t count)
{
    long result = sendMessage(fd, data, size, fds, count);
    if (result == -ETOOMANYREFS) {
        // Descriptors on their way count against the limit of open files.
        rlimit limit = {};
        if (getrlimit(RLIMIT_NOFILE, &limit) == 0 && limit.rlim_cur < limit.rlim_max) {
            limit.rlim_cur = limit.rlim_max;
            if (setrlimit(RLIMIT_NOFILE, &limit) == 0)
                result = sendMessage(fd, data, size, fds, count);
        }
    }
    return result;
}

// To a connection: now, or when its socket takes it.
static void send(Broker& broker, Connection* connection, const void* data, size_t size, std::vector<int> fds)
{
    if (connection->queue.empty()) {
        const long result = sendMessage(connection->fd, data, size, fds.data(), fds.size());
        if (result >= 0 || (result != -EAGAIN && result != -ENOBUFS)) {
            for (int fd : fds)
                close(fd);
            return;
        }
    }
    const char* bytes = static_cast<const char*>(data);
    connection->queue.push_back({std::vector<char>(bytes, bytes + size), std::move(fds)});
    if (!connection->writing) {
        epoll_event event = {};
        event.events = EPOLLIN | EPOLLOUT;
        event.data.fd = connection->fd;
        epoll_ctl(broker.epoll, EPOLL_CTL_MOD, connection->fd, &event);
        connection->writing = true;
    }
}

// The messages that waited for the port go to where they were going.
static void flushPort(Broker& broker, Port& port)
{
    port.transit = nullptr;
    std::deque<Pending> pending;
    pending.swap(port.pending);
    for (Pending& message : pending)
        send(broker, message.to, message.data.data(), message.data.size(), std::move(message.fds));
}

static void sendOnceNotification(Broker& broker, uint64_t name, Connection* receiver)
{
    if (!receiver)
        return;
    struct
    {
        Header header;
        uint32_t bits;
        uint32_t size;
        uint32_t remote;
        uint32_t local;
        uint32_t voucher;
        uint32_t id;
        uint64_t ndr;
    } notification = {};
    notification.header.type = ReceivedMessage;
    notification.header.remote = name;
    notification.header.value = MoveSendOnce;
    notification.header.remoteDisposition = MoveSendOnce;
    notification.header.bodySize = 0x20;
    notification.bits = MoveSendOnce;
    notification.size = 0x20;
    // MACH_NOTIFY_SEND_ONCE
    notification.id = 0x47;
    notification.ndr = 0x1000000;
    send(broker, receiver, &notification, sizeof(notification), {});
}

static void noSenders(Broker& broker, uint64_t name, Port& port)
{
    if (!port.receiver || port.noSendersSent || port.names != 0)
        return;
    for (auto& [connection, rights] : port.senders) {
        if (rights.sends != 0)
            return;
    }
    port.noSendersSent = true;
    Header notification = {};
    notification.type = NoSenders;
    notification.remote = name;
    notification.local = port.madeSends;
    send(broker, port.receiver, &notification, sizeof(notification), {});
}

// To the receiver of a port, unless the receive right is on its way: then the message waits for it.
static void deliver(Broker& broker, Connection* to, uint64_t name, const void* data, size_t size, std::vector<int> fds, bool unaudited,
                    Connection* sender)
{
    for (auto& [key, port] : broker.ports) {
        if (key != name)
            continue;
        if (port.transit && (port.transit != sender || !unaudited)) {
            const char* bytes = static_cast<const char*>(data);
            port.pending.push_back({to, std::vector<char>(bytes, bytes + size), std::move(fds)});
            return;
        }
        break;
    }
    send(broker, to, data, size, std::move(fds));
}

static std::string socketPath()
{
    return "/run/user/" + std::to_string(getuid()) + "/reef-mach-" + std::to_string(BrokerVersion);
}

static int connectBroker()
{
    const std::string path = socketPath();
    int fd = connectTo(path);
    if (fd < 0) {
        startBroker();
        for (int tries = 400; tries; tries--) {
            fd = connectTo(path);
            if (fd >= 0)
                return fd;
            const timespec delay = {0, 5000000};
            nanosleep(&delay, nullptr);
        }
    }
    return fd;
}

void addMachServices(HostServices& services)
{
    services.machConnect = connectBroker;
    services.machSend = machSend;
    services.machReceive = machReceive;
}

// The holders of rights learn that the name is dead; its bootstrap names go away.
static void destroyPort(Broker& broker, uint64_t name)
{
    auto it = std::find_if(broker.ports.begin(), broker.ports.end(), [name](const auto& entry) { return entry.first == name; });
    if (it == broker.ports.end())
        return;
    Port& port = it->second;
    flushPort(broker, port);
    broker.transits.erase(name);
    for (auto& [connection, rights] : port.senders) {
        connection->sends.erase(name);
        Header notification = {};
        notification.type = DeadName;
        notification.remote = name;
        send(broker, connection, &notification, sizeof(notification), {});
    }
    if (port.receiver)
        port.receiver->receives.erase(name);
    if (port.names != 0) {
        for (auto service = broker.services.begin(); service != broker.services.end();) {
            if (service->second == name)
                service = broker.services.erase(service);
            else
                ++service;
        }
    }
    broker.ports.erase(it);
}

static void closeConnection(Broker& broker, Connection* connection)
{
    epoll_ctl(broker.epoll, EPOLL_CTL_DEL, connection->fd, nullptr);
    broker.connections.erase(connection->fd);
    close(connection->fd);
    for (auto& [name, registered] : connection->names) {
        auto service = broker.services.find(name);
        if (service == broker.services.end() || service->second != registered)
            continue;
        broker.services.erase(service);
        auto it = broker.ports.find(registered);
        if (it != broker.ports.end()) {
            it->second.names--;
            noSenders(broker, registered, it->second);
        }
    }
    const std::vector<uint64_t> receives(connection->receives.begin(), connection->receives.end());
    for (uint64_t name : receives) {
        auto it = broker.ports.find(name);
        if (it != broker.ports.end())
            it->second.receiver = nullptr;
        destroyPort(broker, name);
    }
    for (uint64_t name : connection->sends) {
        auto it = broker.ports.find(name);
        if (it == broker.ports.end())
            continue;
        Port& port = it->second;
        auto right = port.senders.find(connection);
        if (right == port.senders.end())
            continue;
        const uint64_t unused = right->second.unusedSendOnces;
        port.senders.erase(right);
        for (uint64_t i = 0; i < unused; i++)
            sendOnceNotification(broker, name, port.receiver);
        noSenders(broker, name, port);
    }
    const std::vector<uint64_t> transits(broker.transits.begin(), broker.transits.end());
    for (uint64_t name : transits) {
        auto it = broker.ports.find(name);
        if (it != broker.ports.end()) {
            Port& port = it->second;
            for (auto message = port.pending.begin(); message != port.pending.end();) {
                if (message->to != connection) {
                    ++message;
                    continue;
                }
                for (int fd : message->fds)
                    close(fd);
                message = port.pending.erase(message);
            }
            if (port.transit != connection)
                continue;
            flushPort(broker, port);
        }
        broker.transits.erase(name);
    }
    for (Outgoing& message : connection->queue) {
        for (int fd : message.fds)
            close(fd);
    }
    delete connection;
}

// The port that a descriptor names; a process makes a port by naming it with its own process number.
static Port* portFor(Broker& broker, Connection* connection, const Descriptor& descriptor)
{
    auto it = broker.ports.find(descriptor.name);
    if (it != broker.ports.end())
        return &it->second;
    if (!(descriptor.flags & NewPort) || connection->pid != int(descriptor.name >> 32))
        return nullptr;
    Port& port = broker.ports[descriptor.name];
    port.receiver = connection;
    connection->receives.insert(descriptor.name);
    return &port;
}

// The second string after the name of a message.
static std::string secondString(const char* text, size_t first, uint32_t size)
{
    const size_t used = std::min<size_t>(first + 1, size);
    text += used;
    return std::string(text, strnlen(text, size - used));
}

static void sandboxRequest(Broker& broker, Connection* connection, const Header* header, const char* text, std::vector<int>& fds)
{
    const std::string name(text, strnlen(text, header->nameSize));
    Header reply = {};
    reply.type = Reply;
    reply.local = header->local;
    switch (header->type) {
    case AddProfile:
        connection->sandbox.add(name);
        return;
    case AddConnection: {
        if (fds.empty())
            return;
        const int fd = fds.front();
        fds.erase(fds.begin());
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
        Connection* added = addConnection(broker, fd);
        if (added)
            added->sandbox = connection->sandbox;
        return;
    }
    case IssueExtension: {
        const std::string path = secondString(text, name.size(), header->nameSize);
        std::string token;
        if (name.empty() || path.empty() || path[0] != '/') {
            reply.remoteDisposition = 1;
        } else {
            if (connection->sandbox.mayIssue(name))
                token = issueSandboxToken(broker.tokens, name, path);
            reply.remoteDisposition = token.empty();
        }
        reply.nameSize = token.size() + 1;
        std::vector<char> message(sizeof(reply) + reply.nameSize);
        memcpy(message.data(), &reply, sizeof(reply));
        memcpy(message.data() + sizeof(reply), token.c_str(), token.size() + 1);
        send(broker, connection, message.data(), message.size(), {});
        return;
    }
    case ConsumeExtension: {
        std::string extension;
        std::string path;
        if (!findSandboxToken(broker.tokens, name, extension, path))
            reply.remoteDisposition = EINVAL;
        else
            reply.value = connection->sandbox.consume(extension, path);
        send(broker, connection, &reply, sizeof(reply), {});
        return;
    }
    case ReleaseExtension:
        connection->sandbox.release(header->value);
        return;
    case OpenFile: {
        int fd = -1;
        reply.remoteDisposition = connection->sandbox.open(name, int(header->value), int(header->remoteDisposition), &fd);
        send(broker, connection, &reply, sizeof(reply), fd >= 0 ? std::vector<int>{fd} : std::vector<int>{});
        return;
    }
    case ChangeFile: {
        const std::string to = secondString(text, name.size(), header->nameSize);
        reply.remoteDisposition = connection->sandbox.change(int(header->value), name, to, int(header->remoteDisposition));
        send(broker, connection, &reply, sizeof(reply), {});
        return;
    }
    default:
        return;
    }
}

static void readConnection(Broker& broker, Connection* connection);

// What came from a connection; `memory` is the memory file of a big message, or -1.
static void handleMessage(Broker& broker, Connection* connection, Header* header, size_t size, std::vector<int>& fds, int memory)
{
    const char* text = reinterpret_cast<const char*>(header) + sizeof(Header) + header->descriptorCount * sizeof(Descriptor) + header->bodySize;
    switch (header->type) {
    case SendMessage: {
        Connection* receiver = nullptr;
        const bool unaudited = header->flags & Unaudited;
        auto it = broker.ports.find(header->remote);
        if (it != broker.ports.end()) {
            receiver = it->second.receiver;
            if (receiver && header->remoteDisposition == MoveSendOnce && !unaudited) {
                auto right = it->second.senders.find(connection);
                if (right != it->second.senders.end() && right->second.unusedSendOnces)
                    right->second.unusedSendOnces--;
            }
        }
        auto* descriptors = reinterpret_cast<Descriptor*>(header + 1);
        for (uint32_t i = 0; i < header->descriptorCount; i++) {
            Descriptor& descriptor = descriptors[i];
            if ((descriptor.flags & NotPort) || !descriptor.name)
                continue;
            const uint64_t name = descriptor.name;
            Port* port = portFor(broker, connection, descriptor);
            descriptor.flags &= ~(NewPort | HasSenders | OwnReceiver);
            if (!port) {
                descriptor.flags |= DeadPort;
                continue;
            }
            const uint32_t disposition = descriptor.disposition;
            if (disposition == MoveReceive) {
                if (connection != port->receiver)
                    continue;
                connection->receives.erase(name);
                if (descriptor.flags & KeepSend) {
                    port->senders[connection].sends++;
                    connection->sends.insert(name);
                }
                if (!receiver) {
                    port->receiver = nullptr;
                    destroyPort(broker, name);
                    descriptor.flags |= DeadPort;
                    continue;
                }
                auto right = port->senders.find(receiver);
                if (right != port->senders.end()) {
                    port->senders.erase(right);
                    receiver->sends.erase(name);
                }
                port->receiver = receiver;
                receiver->receives.insert(name);
                flushPort(broker, *port);
                port->transit = connection;
                broker.transits.insert(name);
                Header moved = {};
                moved.type = ReceiveMoved;
                moved.remote = name;
                send(broker, connection, &moved, sizeof(moved), {});
                port->madeSends = 0;
                port->noSendersSent = false;
                bool senders = port->names != 0;
                for (auto& [holder, rights] : port->senders) {
                    if (!senders)
                        senders = rights.sends != 0;
                }
                if (senders)
                    descriptor.flags |= HasSenders;
                else
                    port->noSendersSent = true;
                continue;
            }
            const bool sendRight = disposition == MoveSend || disposition == CopySend || disposition == MakeSend;
            if (!sendRight) {
                if (disposition != MoveSendOnce && disposition != MakeSendOnce)
                    continue;
                if (connection != port->receiver && disposition == MoveSendOnce) {
                    Rights& rights = port->senders[connection];
                    if (rights.unusedSendOnces)
                        rights.unusedSendOnces--;
                }
            } else if (connection == port->receiver) {
                port->madeSends++;
            }
            if (!receiver)
                continue;
            if (receiver == port->receiver) {
                descriptor.flags |= OwnReceiver;
                continue;
            }
            Rights& rights = port->senders[receiver];
            receiver->sends.insert(name);
            if (sendRight) {
                rights.sends++;
                port->noSendersSent = false;
            } else {
                rights.sendOnces++;
                rights.unusedSendOnces++;
            }
        }
        if (!receiver) {
            for (int fd : fds)
                close(fd);
            if (memory >= 0)
                close(memory);
            break;
        }
        header->type = ReceivedMessage;
        header->flags &= ~Unaudited;
        if (!unaudited) {
            header->pid = connection->pid;
            header->uid = connection->uid;
            header->gid = connection->gid;
        }
        if (memory < 0) {
            deliver(broker, receiver, header->remote, header, size, fds, unaudited, connection);
        } else {
            Header forwarded = *header;
            forwarded.descriptorCount = 0;
            forwarded.flags |= InMemory;
            fds.push_back(memory);
            deliver(broker, receiver, header->remote, &forwarded, sizeof(forwarded), fds, unaudited, connection);
        }
        fds.clear();
        return;
    }
    case ReleaseRights: {
        auto it = broker.ports.find(header->remote);
        if (it == broker.ports.end())
            break;
        Port& port = it->second;
        auto right = port.senders.find(connection);
        if (right == port.senders.end())
            break;
        Rights& rights = right->second;
        rights.sends -= std::min(rights.sends, header->local);
        rights.sendOnces -= std::min(rights.sendOnces, header->value);
        if (rights.sends == 0 && rights.sendOnces == 0) {
            const uint64_t unused = rights.unusedSendOnces;
            port.senders.erase(right);
            connection->sends.erase(header->remote);
            for (uint64_t i = 0; i < unused; i++)
                sendOnceNotification(broker, header->remote, port.receiver);
        }
        noSenders(broker, header->remote, port);
        break;
    }
    case DestroyPort: {
        auto it = broker.ports.find(header->remote);
        if (it != broker.ports.end() && it->second.receiver == connection)
            destroyPort(broker, header->remote);
        break;
    }
    case DestroySendOnce: {
        auto it = broker.ports.find(header->remote);
        if (it == broker.ports.end())
            break;
        auto right = it->second.senders.find(connection);
        if (right == it->second.senders.end() || right->second.unusedSendOnces == 0)
            break;
        right->second.unusedSendOnces--;
        sendOnceNotification(broker, header->remote, it->second.receiver);
        break;
    }
    case CheckIn: {
        const std::string name(text, strnlen(text, header->nameSize));
        Header reply = {};
        reply.type = Reply;
        reply.local = header->local;
        auto existing = broker.services.find(name);
        if (!header->remote) {
            if (existing != broker.services.end()) {
                auto it = broker.ports.find(existing->second);
                broker.services.erase(existing);
                if (it != broker.ports.end()) {
                    it->second.names--;
                    noSenders(broker, it->first, it->second);
                }
            }
            send(broker, connection, &reply, sizeof(reply), {});
            break;
        }
        const Descriptor descriptor = {header->remote, 0, (header->flags >> 2) & 1, {}};
        Port* port = portFor(broker, connection, descriptor);
        if (!port) {
            reply.remoteDisposition = BootstrapUnknownService;
            send(broker, connection, &reply, sizeof(reply), {});
            break;
        }
        if (existing != broker.services.end() && existing->second != header->remote) {
            if (!(header->flags & ReplaceName) && broker.ports.find(existing->second) != broker.ports.end()) {
                reply.remoteDisposition = BootstrapServiceActive;
                send(broker, connection, &reply, sizeof(reply), {});
                break;
            }
            auto old = broker.ports.find(existing->second);
            if (old != broker.ports.end()) {
                old->second.names--;
                noSenders(broker, old->first, old->second);
            }
        }
        if (existing == broker.services.end() || existing->second != header->remote) {
            broker.services[name] = header->remote;
            port->names++;
            port->noSendersSent = false;
            if (connection == port->receiver)
                port->madeSends++;
            else
                connection->names[name] = header->remote;
        }
        send(broker, connection, &reply, sizeof(reply), {});
        break;
    }
    case LookUp: {
        const std::string name(text, strnlen(text, header->nameSize));
        Header reply = {};
        reply.type = Reply;
        reply.local = header->local;
        auto service = broker.services.find(name);
        auto it = service == broker.services.end() ? broker.ports.end() : broker.ports.find(service->second);
        if (!connection->sandbox.mayLookUp(name)) {
            reply.remoteDisposition = BootstrapNotPrivileged;
        } else if (it == broker.ports.end()) {
            reply.remoteDisposition = BootstrapUnknownService;
        } else {
            reply.remote = it->first;
            Port& port = it->second;
            if (connection == port.receiver) {
                reply.flags = OwnReceiver;
            } else {
                port.senders[connection].sends++;
                port.noSendersSent = false;
                connection->sends.insert(it->first);
            }
            if (port.receiver)
                reply.pid = port.receiver->pid;
        }
        send(broker, connection, &reply, sizeof(reply), {});
        break;
    }
    case Synchronize: {
        const uint64_t value = header->local;
        if (broker.synchronizing) {
            broker.waiters.push_back({connection->fd, connection, value});
            break;
        }
        broker.synchronizing = true;
        size_t waiting;
        do {
            waiting = broker.waiters.size();
            std::vector<std::pair<int, Connection*>> others;
            for (auto& [fd, other] : broker.connections) {
                if (other != connection)
                    others.push_back({fd, other});
            }
            for (auto& [fd, other] : others) {
                auto it = broker.connections.find(fd);
                if (it != broker.connections.end() && it->second == other)
                    readConnection(broker, other);
            }
        } while (waiting < broker.waiters.size());
        broker.synchronizing = false;
        std::vector<Waiter> waiters = {{connection->fd, connection, value}};
        waiters.insert(waiters.end(), broker.waiters.begin(), broker.waiters.end());
        broker.waiters.clear();
        for (Waiter& waiter : waiters) {
            auto it = broker.connections.find(waiter.fd);
            if (it == broker.connections.end() || it->second != waiter.connection)
                continue;
            Header reply = {};
            reply.type = Reply;
            reply.local = waiter.value;
            send(broker, waiter.connection, &reply, sizeof(reply), {});
        }
        break;
    }
    case AddProfile:
    case AddConnection:
    case IssueExtension:
    case ConsumeExtension:
    case ReleaseExtension:
    case OpenFile:
    case ChangeFile:
        sandboxRequest(broker, connection, header, text, fds);
        break;
    case ReceiveMoved: {
        auto it = broker.ports.find(header->remote);
        if (it != broker.ports.end() && it->second.transit == connection) {
            flushPort(broker, it->second);
            broker.transits.erase(header->remote);
        }
        break;
    }
    }
    for (int fd : fds)
        close(fd);
    fds.clear();
}

static void readConnection(Broker& broker, Connection* connection)
{
    for (;;) {
        iovec vector = {broker.buffer.data(), broker.buffer.size()};
        alignas(cmsghdr) char control[1024];
        msghdr message = {};
        message.msg_iov = &vector;
        message.msg_iovlen = 1;
        message.msg_control = control;
        message.msg_controllen = sizeof(control);
        int fds[251];
        uint32_t count = 0;
        long size;
        do
            size = recvmsg(connection->fd, &message, MSG_DONTWAIT | MSG_CMSG_CLOEXEC);
        while (size < 0 && errno == EINTR);
        if (size < 0) {
            if (errno == EAGAIN)
                return;
            size = -errno;
        } else {
            for (cmsghdr* header = CMSG_FIRSTHDR(&message); header; header = CMSG_NXTHDR(&message, header)) {
                if (header->cmsg_level != SOL_SOCKET || header->cmsg_type != SCM_RIGHTS)
                    continue;
                const uint32_t n = (header->cmsg_len - CMSG_LEN(0)) / sizeof(int);
                const int* given = reinterpret_cast<const int*>(CMSG_DATA(header));
                for (uint32_t i = 0; i < n; i++) {
                    if (count < std::size(fds))
                        fds[count++] = given[i];
                    else
                        close(given[i]);
                }
            }
        }
        if (size < long(sizeof(Header))) {
            for (uint32_t i = 0; i < count; i++)
                close(fds[i]);
            closeConnection(broker, connection);
            return;
        }
        auto* header = reinterpret_cast<Header*>(broker.buffer.data());
        std::vector<int> received(fds, fds + count);
        if (header->flags & InMemory) {
            if (received.empty()) {
                closeConnection(broker, connection);
                return;
            }
            const int memory = received.back();
            received.pop_back();
            struct stat status;
            void* mapped;
            if (fstat(memory, &status) != 0 || status.st_size < long(sizeof(Header)) ||
                (mapped = mmap(nullptr, status.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, memory, 0)) == MAP_FAILED) {
                close(memory);
                for (int fd : received)
                    close(fd);
                continue;
            }
            static_cast<Header*>(mapped)->flags |= InMemory;
            handleMessage(broker, connection, static_cast<Header*>(mapped), status.st_size, received, memory);
            munmap(mapped, status.st_size);
        } else {
            if (size_t(size) < (header->descriptorCount + 2) * sizeof(Descriptor) + header->bodySize + header->nameSize) {
                for (int fd : received)
                    close(fd);
                continue;
            }
            handleMessage(broker, connection, header, size, received, -1);
        }
        if (broker.connections.find(connection->fd) == broker.connections.end())
            return;
    }
}

// What the socket of a connection can take again.
static bool writeQueue(Connection* connection)
{
    while (!connection->queue.empty()) {
        Outgoing& message = connection->queue.front();
        const long result = sendMessage(connection->fd, message.data.data(), message.data.size(), message.fds.data(), message.fds.size());
        if (result == -EAGAIN || result == -ENOBUFS)
            return false;
        for (int fd : message.fds)
            close(fd);
        connection->queue.pop_front();
    }
    return true;
}

int machBroker()
{
    Broker broker;
    const std::string path = socketPath();
    rlimit limit = {};
    if (getrlimit(RLIMIT_NOFILE, &limit) == 0 && limit.rlim_cur < limit.rlim_max) {
        limit.rlim_cur = limit.rlim_max;
        setrlimit(RLIMIT_NOFILE, &limit);
    }
    // One broker for the session: the lock decides, and a broker that answers already wins.
    const std::string lock = path + ".lock";
    const int lockFd = open(lock.c_str(), O_RDWR | O_CREAT | O_CLOEXEC, 0600);
    if (lockFd < 0 || flock(lockFd, LOCK_EX) != 0)
        return 0;
    const int existing = connectTo(path);
    if (existing >= 0) {
        close(existing);
        return 0;
    }
    unlink(path.c_str());
    broker.listener = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    sockaddr_un address = {};
    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, sizeof(address.sun_path), "%s", path.c_str());
    const mode_t mask = umask(077);
    const int bound = bind(broker.listener, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    umask(mask);
    if (bound != 0 || listen(broker.listener, 128) != 0)
        return 1;
    signal(SIGPIPE, SIG_IGN);
    broker.epoll = epoll_create1(EPOLL_CLOEXEC);
    epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = broker.listener;
    epoll_ctl(broker.epoll, EPOLL_CTL_ADD, broker.listener, &event);
    epoll_event events[64];
    for (;;) {
        // Without connections it ends when none comes for 20 seconds.
        int timeout = broker.connections.empty() ? 20000 : -1;
        int count;
        while ((count = epoll_wait(broker.epoll, events, std::size(events), timeout)) == 0) {
            if (broker.connections.empty()) {
                unlink(path.c_str());
                return 0;
            }
            timeout = -1;
        }
        for (int i = 0; i < count; i++) {
            const int fd = events[i].data.fd;
            if (fd == broker.listener) {
                int accepted;
                while ((accepted = accept4(broker.listener, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC)) >= 0)
                    addConnection(broker, accepted);
                continue;
            }
            auto it = broker.connections.find(fd);
            if (it == broker.connections.end())
                continue;
            Connection* connection = it->second;
            if ((events[i].events & EPOLLOUT) && writeQueue(connection)) {
                epoll_event readable = {};
                readable.events = EPOLLIN;
                readable.data.fd = connection->fd;
                epoll_ctl(broker.epoll, EPOLL_CTL_MOD, connection->fd, &readable);
                connection->writing = false;
            }
            if (events[i].events & (EPOLLIN | EPOLLERR | EPOLLHUP))
                readConnection(broker, connection);
        }
    }
}

} // namespace cutefish::reef

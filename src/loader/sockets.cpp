// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Sockets with Darwin's addresses, options, flags and control messages; SO_NOSIGPIPE; and ICMP sockets that behave as raw ones:
// whole packets with the IP header, the identifiers the program chose, and the errors of the network as ICMP messages.

#include "host.h"

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <vector>

#include <arpa/inet.h>
#include <linux/errqueue.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>

namespace cutefish::reef {

// struct addrinfo, msghdr, cmsghdr and xucred of Darwin.
struct DarwinAddressInfo
{
    int flags;
    int family;
    int type;
    int protocol;
    uint32_t addressLength;
    char* canonicalName;
    void* address;
    DarwinAddressInfo* next;
};

struct DarwinMessage
{
    void* name;
    uint32_t nameLength;
    iovec* vectors;
    int vectorCount;
    void* control;
    uint32_t controlLength;
    int flags;
};

struct DarwinControl
{
    uint32_t length;
    int level;
    int type;
};

struct DarwinCredentials
{
    uint32_t version;
    uint32_t user;
    short groupCount;
    uint32_t groups[16];
};

// An echo request that was sent: the kernel replaces the identifier and the checksum.
struct PingRequest
{
    bool used;
    uint16_t sequence;
    uint16_t identifier;
    uint16_t checksum;
};

struct Ping
{
    bool v6;
    PingRequest requests[256];
};

// What the control messages of a packet say.
struct PacketInformation
{
    bool hasDestination = false;
    in_addr destination = {};
    in6_addr destination6 = {};
    uint8_t ttl = 64;
    uint8_t tos = 0;
    const sock_extended_err* error = nullptr;
};

static std::mutex noSignalMutex;
static std::set<int> noSignalSockets;
static std::atomic<bool> anyNoSignal;
static std::mutex pingMutex;
static std::map<int, Ping> pings;

static bool socketOption(int level, int name, int* hostLevel, int* hostName)
{
    if (level == 0xffff) {
        *hostLevel = SOL_SOCKET;
        switch (name) {
        case 0x1:
            *hostName = SO_DEBUG;
            return true;
        case 0x2:
            *hostName = SO_ACCEPTCONN;
            return true;
        case 0x4:
            *hostName = SO_REUSEADDR;
            return true;
        case 0x8:
            *hostName = SO_KEEPALIVE;
            return true;
        case 0x10:
            *hostName = SO_DONTROUTE;
            return true;
        case 0x20:
            *hostName = SO_BROADCAST;
            return true;
        case 0x80:
            *hostName = SO_LINGER;
            return true;
        case 0x100:
            *hostName = SO_OOBINLINE;
            return true;
        case 0x200:
            *hostName = SO_REUSEPORT;
            return true;
        case 0x1001:
            *hostName = SO_SNDBUF;
            return true;
        case 0x1002:
            *hostName = SO_RCVBUF;
            return true;
        case 0x1003:
            *hostName = SO_SNDLOWAT;
            return true;
        case 0x1004:
            *hostName = SO_RCVLOWAT;
            return true;
        case 0x1005:
            *hostName = SO_SNDTIMEO;
            return true;
        case 0x1006:
            *hostName = SO_RCVTIMEO;
            return true;
        case 0x1007:
            *hostName = SO_ERROR;
            return true;
        case 0x1008:
            *hostName = SO_TYPE;
            return true;
        }
        return false;
    }
    if (level == IPPROTO_TCP) {
        *hostLevel = IPPROTO_TCP;
        switch (name) {
        case 0x1:
            *hostName = 1;
            return true;
        case 0x10:
            *hostName = 4;
            return true;
        case 0x101:
            *hostName = 5;
            return true;
        case 0x102:
            *hostName = 6;
            return true;
        }
        return false;
    }
    if (level == IPPROTO_IPV6) {
        *hostLevel = IPPROTO_IPV6;
        switch (name) {
        case 4:
            *hostName = IPV6_UNICAST_HOPS;
            return true;
        case 9:
            *hostName = IPV6_MULTICAST_IF;
            return true;
        case 10:
            *hostName = IPV6_MULTICAST_HOPS;
            return true;
        case 11:
            *hostName = IPV6_MULTICAST_LOOP;
            return true;
        case 12:
            *hostName = IPV6_JOIN_GROUP;
            return true;
        case 13:
            *hostName = IPV6_LEAVE_GROUP;
            return true;
        case 27:
            *hostName = IPV6_V6ONLY;
            return true;
        case 36:
            *hostName = IPV6_TCLASS;
            return true;
        case 61:
            *hostName = IPV6_RECVPKTINFO;
            return true;
        case 62:
            *hostName = IPV6_DONTFRAG;
            return true;
        }
        return false;
    }
    if (level == IPPROTO_IP) {
        *hostLevel = IPPROTO_IP;
        switch (name) {
        case 2:
            *hostName = IP_HDRINCL;
            return true;
        case 3:
            *hostName = IP_TOS;
            return true;
        case 4:
            *hostName = IP_TTL;
            return true;
        case 9:
            *hostName = IP_MULTICAST_IF;
            return true;
        case 10:
            *hostName = IP_MULTICAST_TTL;
            return true;
        case 11:
            *hostName = IP_MULTICAST_LOOP;
            return true;
        case 12:
            *hostName = IP_ADD_MEMBERSHIP;
            return true;
        case 13:
            *hostName = IP_DROP_MEMBERSHIP;
            return true;
        case 24:
            *hostName = IP_RECVTTL;
            return true;
        case 26:
            *hostName = IP_PKTINFO;
            return true;
        case 27:
            *hostName = IP_RECVTOS;
            return true;
        }
        return false;
    }
    return false;
}

static uint16_t load16(const uint8_t* data) { return uint16_t(data[0] << 8 | data[1]); }

static void store16(uint8_t* data, uint16_t value)
{
    data[0] = uint8_t(value >> 8);
    data[1] = uint8_t(value);
}

static void store32(uint8_t* data, uint32_t value)
{
    data[0] = uint8_t(value >> 24);
    data[1] = uint8_t(value >> 16);
    data[2] = uint8_t(value >> 8);
    data[3] = uint8_t(value);
}

static uint32_t sum16(const uint8_t* data, size_t size, uint32_t sum = 0)
{
    for (size_t i = 0; i + 1 < size; i += 2)
        sum += load16(data + i);
    if (size & 1)
        sum += uint32_t(data[size - 1]) << 8;
    return sum;
}

static uint16_t checksum(uint32_t sum)
{
    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);
    return uint16_t(~sum);
}

// The IP header in front of a packet as a raw socket of Darwin returns it: the length is in the host's order.
static void writeIpHeader(uint8_t* header, uint16_t length, uint8_t tos, uint8_t ttl, in_addr source, in_addr destination)
{
    memset(header, 0, 20);
    header[0] = 0x45;
    header[1] = tos;
    memcpy(header + 2, &length, sizeof length);
    header[8] = ttl;
    header[9] = IPPROTO_ICMP;
    memcpy(header + 12, &source, sizeof source);
    memcpy(header + 16, &destination, sizeof destination);
    store16(header + 10, checksum(sum16(header, 20)));
}

static bool hostAddress(const void* address, unsigned length, sockaddr_storage* host, socklen_t* hostLength)
{
    if (!address || length < 2)
        return false;
    const uint8_t* bytes = static_cast<const uint8_t*>(address);
    const uint8_t family = bytes[1];
    memset(host, 0, sizeof *host);
    host->ss_family = family == 30 ? AF_INET6 : family;
    uint8_t* data = reinterpret_cast<uint8_t*>(host) + 2;
    switch (host->ss_family) {
    case AF_INET:
        if (length < 16)
            return false;
        memcpy(data, bytes + 2, 14);
        *hostLength = 16;
        return true;
    case AF_INET6:
        if (length < 28)
            return false;
        memcpy(data, bytes + 2, 26);
        *hostLength = 28;
        return true;
    case AF_UNIX: {
        const size_t size = strnlen(reinterpret_cast<const char*>(bytes + 2), std::min<size_t>(length - 2, 104));
        memcpy(data, bytes + 2, size);
        *hostLength = socklen_t(size + 3);
        return true;
    }
    }
    return false;
}

static int hostInetPton(int family, const char* text, void* address)
{
    return inet_pton(family == 30 ? AF_INET6 : family, text, address);
}

static const char* hostInetNtop(int family, const void* address, char* text, uint32_t size)
{
    return inet_ntop(family == 30 ? AF_INET6 : family, address, text, size);
}

template <typename T>
static void answer(const T& value, void* out, uint32_t* length)
{
    memcpy(out, &value, std::min<size_t>(*length, sizeof value));
    *length = sizeof value;
}

// LOCAL_PEERCRED, LOCAL_PEERPID and LOCAL_PEEREPID.
static int peerCredentials(int fd, int name, void* value, uint32_t* length)
{
    ucred peer = {};
    socklen_t size = sizeof peer;
    if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &peer, &size) < 0)
        return -darwinError(errno);
    if (name != 1) {
        answer(int(peer.pid), value, length);
        return 0;
    }
    DarwinCredentials credentials = {};
    credentials.user = peer.uid;
    credentials.groupCount = 1;
    credentials.groups[0] = peer.gid;
    gid_t groups[64];
    socklen_t groupsSize = sizeof groups;
    if (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, groups, &groupsSize) == 0) {
        for (size_t i = 0; i < groupsSize / sizeof(gid_t) && credentials.groupCount < 16; i++) {
            if (groups[i] != peer.gid)
                credentials.groups[credentials.groupCount++] = groups[i];
        }
    }
    memcpy(value, &credentials, std::min<size_t>(*length, sizeof credentials));
    *length = std::min<uint32_t>(*length, sizeof credentials);
    return 0;
}

static int hostSocketpair(int family, int type, int protocol, int* fds)
{
    if (socketpair(family == 30 ? AF_INET6 : family, type, protocol, fds) < 0)
        return -darwinError(errno);
    return 0;
}

static void darwinAddress(const sockaddr_storage* host, socklen_t hostLength, void* address, uint32_t* length)
{
    const uint8_t* data = reinterpret_cast<const uint8_t*>(host) + 2;
    uint8_t bytes[128] = {};
    uint32_t size;
    uint8_t family;
    switch (host->ss_family) {
    case AF_INET:
        memcpy(bytes + 2, data, 14);
        size = 16;
        family = AF_INET;
        break;
    case AF_INET6:
        memcpy(bytes + 2, data, 26);
        size = 28;
        family = 30;
        break;
    case AF_UNIX: {
        const size_t path = hostLength > 2 ? std::min<size_t>(hostLength - 2, 103) : 0;
        memcpy(bytes + 2, data, path);
        size = uint32_t(path + 3);
        family = AF_UNIX;
        break;
    }
    default:
        size = 2;
        family = uint8_t(host->ss_family);
        break;
    }
    bytes[0] = uint8_t(size);
    bytes[1] = family;
    memcpy(address, bytes, std::min(size, *length));
    *length = size;
}

static int resolverError(int error)
{
    switch (error) {
    case 0:
        return 0;
    case EAI_AGAIN:
        return 2;
    case EAI_BADFLAGS:
        return 3;
    case EAI_FAIL:
        return 4;
    case EAI_FAMILY:
        return 5;
    case EAI_MEMORY:
        return 6;
    case EAI_NODATA:
        return 7;
    case EAI_NONAME:
        return 8;
    case EAI_SERVICE:
        return 9;
    case EAI_SOCKTYPE:
        return 10;
    case EAI_SYSTEM:
        return 11;
    case EAI_OVERFLOW:
        return 14;
    }
    return 4;
}

static int hostGetaddrinfo(const char* node, const char* service, const DarwinAddressInfo* hints, DarwinAddressInfo** result)
{
    addrinfo host = {};
    if (hints) {
        host.ai_flags = hints->flags & 7;
        if (hints->flags & 0x100)
            host.ai_flags |= AI_ALL;
        if (hints->flags & 0x400)
            host.ai_flags |= AI_ADDRCONFIG;
        if (hints->flags & 0x800)
            host.ai_flags |= AI_V4MAPPED;
        if (hints->flags & 0x1000)
            host.ai_flags |= AI_NUMERICSERV;
        host.ai_family = hints->family == 30 ? AF_INET6 : hints->family;
        host.ai_socktype = hints->type;
        host.ai_protocol = hints->protocol;
    }
    addrinfo* list = nullptr;
    const int error = getaddrinfo(node, service, hints ? &host : nullptr, &list);
    if (error)
        return resolverError(error);
    DarwinAddressInfo* first = nullptr;
    DarwinAddressInfo** next = &first;
    for (const addrinfo* entry = list; entry; entry = entry->ai_next) {
        if (entry->ai_family != AF_INET && entry->ai_family != AF_INET6)
            continue;
        DarwinAddressInfo* copy = static_cast<DarwinAddressInfo*>(calloc(1, sizeof(DarwinAddressInfo)));
        copy->flags = hints ? hints->flags : 0;
        copy->family = entry->ai_family == AF_INET6 ? 30 : 2;
        copy->type = entry->ai_socktype;
        copy->protocol = entry->ai_protocol;
        sockaddr_storage address = {};
        memcpy(&address, entry->ai_addr, std::min<size_t>(entry->ai_addrlen, sizeof address));
        uint32_t length = 28;
        copy->address = calloc(1, length);
        if (copy->address)
            darwinAddress(&address, entry->ai_addrlen, copy->address, &length);
        copy->addressLength = length;
        if (entry->ai_canonname)
            copy->canonicalName = strdup(entry->ai_canonname);
        *next = copy;
        next = &copy->next;
    }
    freeaddrinfo(list);
    *result = first;
    return first ? 0 : 8;
}

static int hostAccept(int fd, void* address, uint32_t* length)
{
    sockaddr_storage host;
    socklen_t hostLength = sizeof host;
    const int accepted = accept(fd, reinterpret_cast<sockaddr*>(&host), &hostLength);
    if (accepted < 0)
        return -darwinError(errno);
    if (address && length)
        darwinAddress(&host, hostLength, address, length);
    return accepted;
}

static PacketInformation readControl(msghdr& message)
{
    PacketInformation information;
    for (cmsghdr* header = CMSG_FIRSTHDR(&message); header; header = CMSG_NXTHDR(&message, header)) {
        if (header->cmsg_level == IPPROTO_IP && header->cmsg_type == IP_PKTINFO) {
            information.hasDestination = true;
            information.destination = reinterpret_cast<const in_pktinfo*>(CMSG_DATA(header))->ipi_addr;
        } else if (header->cmsg_level == IPPROTO_IP && header->cmsg_type == IP_TTL) {
            information.ttl = uint8_t(*reinterpret_cast<const int*>(CMSG_DATA(header)));
        } else if (header->cmsg_level == IPPROTO_IP && header->cmsg_type == IP_TOS) {
            information.tos = *CMSG_DATA(header);
        } else if (header->cmsg_level == IPPROTO_IPV6 && header->cmsg_type == IPV6_PKTINFO) {
            information.hasDestination = true;
            information.destination6 = reinterpret_cast<const in6_pktinfo*>(CMSG_DATA(header))->ipi6_addr;
        } else if ((header->cmsg_level == IPPROTO_IP && header->cmsg_type == IP_RECVERR) ||
                   (header->cmsg_level == IPPROTO_IPV6 && header->cmsg_type == IPV6_RECVERR)) {
            information.error = reinterpret_cast<const sock_extended_err*>(CMSG_DATA(header));
        }
    }
    return information;
}

static int hostShutdown(int fd, int how)
{
    if (shutdown(fd, how) < 0)
        return -darwinError(errno);
    return 0;
}

static int hostListen(int fd, int backlog)
{
    if (listen(fd, backlog) < 0)
        return -darwinError(errno);
    return 0;
}

// Notes the identifier of an echo request that goes out.
static void rememberPing(int fd, const iovec* vectors, size_t count)
{
    uint8_t header[8];
    size_t size = 0;
    for (size_t i = 0; i < count && size < sizeof header; i++) {
        const size_t part = std::min(vectors[i].iov_len, sizeof header - size);
        memcpy(header + size, vectors[i].iov_base, part);
        size += part;
    }
    if (size < sizeof header)
        return;
    std::lock_guard lock(pingMutex);
    const auto found = pings.find(fd);
    if (found == pings.end())
        return;
    Ping& ping = found->second;
    if (header[0] != (ping.v6 ? 128 : 8))
        return;
    PingRequest& request = ping.requests[header[7]];
    request.used = true;
    request.sequence = load16(header + 6);
    request.identifier = load16(header + 4);
    request.checksum = load16(header + 2);
}

// Puts the identifier of the program back in a packet: in the request itself, or in the reply to it.
static void restorePing(int fd, uint8_t* packet, size_t size, bool isRequest)
{
    if (size < 8)
        return;
    std::lock_guard lock(pingMutex);
    const auto found = pings.find(fd);
    if (found == pings.end())
        return;
    const PingRequest& request = found->second.requests[packet[7]];
    if (!request.used || request.sequence != load16(packet + 6))
        return;
    uint16_t sum;
    if (isRequest) {
        sum = request.checksum;
    } else {
        uint32_t partial = uint16_t(~load16(packet + 2)) + uint16_t(~load16(packet + 4)) + request.identifier;
        partial = (partial & 0xffff) + (partial >> 16);
        partial += partial >> 16;
        sum = uint16_t(~partial);
    }
    store16(packet + 2, sum);
    store16(packet + 4, request.identifier);
}

static int hostGetpeername(int fd, void* address, uint32_t* length)
{
    sockaddr_storage host;
    socklen_t hostLength = sizeof host;
    if (getpeername(fd, reinterpret_cast<sockaddr*>(&host), &hostLength) < 0)
        return -darwinError(errno);
    if (address && length)
        darwinAddress(&host, hostLength, address, length);
    return 0;
}

static int hostGetsockname(int fd, void* address, uint32_t* length)
{
    sockaddr_storage host;
    socklen_t hostLength = sizeof host;
    if (getsockname(fd, reinterpret_cast<sockaddr*>(&host), &hostLength) < 0)
        return -darwinError(errno);
    if (address && length)
        darwinAddress(&host, hostLength, address, length);
    return 0;
}

static int hostGetsockopt(int fd, int level, int name, void* value, uint32_t* length)
{
    if (level == 0 && name > 0 && name < 4) {
        int domain = 0;
        socklen_t size = sizeof domain;
        if (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &domain, &size) == 0 && domain == AF_UNIX)
            return peerCredentials(fd, name, value, length);
    }
    if (level == 0xffff && name == 0x1022) {
        answer(int(socketNoSignal(fd)), value, length);
        return 0;
    }
    if ((level == IPPROTO_IP && name == 25) || (level == IPPROTO_IPV6 && name == 125)) {
        char device[IF_NAMESIZE] = {};
        socklen_t size = sizeof device;
        if (getsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, device, &size) < 0)
            return -darwinError(errno);
        answer(device[0] ? if_nametoindex(device) : 0u, value, length);
        return 0;
    }
    if (level == IPPROTO_IP && name == 28) {
        int discover = 0;
        socklen_t size = sizeof discover;
        if (getsockopt(fd, IPPROTO_IP, IP_MTU_DISCOVER, &discover, &size) < 0)
            return -darwinError(errno);
        answer(int(discover == IP_PMTUDISC_DO), value, length);
        return 0;
    }
    int hostLevel;
    int hostName;
    if (!socketOption(level, name, &hostLevel, &hostName))
        return -42;
    if (hostLevel == SOL_SOCKET && (hostName == SO_RCVTIMEO || hostName == SO_SNDTIMEO)) {
        timeval host;
        socklen_t size = sizeof host;
        if (getsockopt(fd, SOL_SOCKET, hostName, &host, &size) < 0)
            return -darwinError(errno);
        const DarwinTimeval timeout = {host.tv_sec, int(host.tv_usec)};
        answer(timeout, value, length);
        return 0;
    }
    socklen_t size = *length;
    if (getsockopt(fd, hostLevel, hostName, value, &size) < 0)
        return -darwinError(errno);
    *length = size;
    if (hostLevel == SOL_SOCKET && hostName == SO_ERROR && size >= sizeof(int)) {
        int* error = static_cast<int*>(value);
        *error = *error ? darwinError(*error) : 0;
    }
    return 0;
}

static int hostMessageFlags(int flags)
{
    int host = flags & 7;
    if (flags & 0x8)
        host |= MSG_EOR;
    if (flags & 0x10)
        host |= MSG_TRUNC;
    if (flags & 0x20)
        host |= MSG_CTRUNC;
    if (flags & 0x40)
        host |= MSG_WAITALL;
    if (flags & 0x80)
        host |= MSG_DONTWAIT;
    return host;
}

static int darwinMessageFlags(int flags)
{
    int darwin = flags & 7;
    if (flags & MSG_EOR)
        darwin |= 0x8;
    if (flags & MSG_TRUNC)
        darwin |= 0x10;
    if (flags & MSG_CTRUNC)
        darwin |= 0x20;
    if (flags & MSG_WAITALL)
        darwin |= 0x40;
    if (flags & MSG_DONTWAIT)
        darwin |= 0x80;
    return darwin;
}

static long hostSendto(int fd, const void* data, size_t size, int flags, const void* address, uint32_t length)
{
    sockaddr_storage host;
    socklen_t hostLength;
    if (address && !hostAddress(address, length, &host, &hostLength))
        return -22;
    if (pingSocket(fd)) {
        const iovec vector = {const_cast<void*>(data), size};
        rememberPing(fd, &vector, 1);
    }
    int hostFlags = hostMessageFlags(flags);
    if (socketNoSignal(fd))
        hostFlags |= MSG_NOSIGNAL;
    const ssize_t sent = sendto(fd, data, size, hostFlags, address ? reinterpret_cast<const sockaddr*>(&host) : nullptr, hostLength);
    if (sent < 0)
        return -darwinError(errno);
    return sent;
}

static long hostSendmsg(int fd, const DarwinMessage* message, int flags)
{
    std::vector<uint8_t> control;
    const uint8_t* bytes = static_cast<const uint8_t*>(message->control);
    for (size_t offset = 0; message->controlLength && offset + sizeof(DarwinControl) <= message->controlLength;) {
        DarwinControl header;
        memcpy(&header, bytes + offset, sizeof header);
        if (header.length < sizeof header || offset + header.length > message->controlLength || header.level != 0xffff || header.type != 1)
            return -45;
        const size_t size = header.length - sizeof header;
        const size_t start = control.size();
        control.resize(start + CMSG_SPACE(size));
        cmsghdr* host = reinterpret_cast<cmsghdr*>(control.data() + start);
        host->cmsg_len = CMSG_LEN(size);
        host->cmsg_level = SOL_SOCKET;
        host->cmsg_type = SCM_RIGHTS;
        memcpy(CMSG_DATA(host), bytes + offset + sizeof header, size);
        offset += (header.length + 3) & ~size_t(3);
    }
    sockaddr_storage address;
    socklen_t addressLength = 0;
    if (message->name && !hostAddress(message->name, message->nameLength, &address, &addressLength))
        return -22;
    msghdr host = {};
    host.msg_name = message->name ? &address : nullptr;
    host.msg_namelen = addressLength;
    host.msg_iov = message->vectors;
    host.msg_iovlen = size_t(message->vectorCount);
    host.msg_control = control.empty() ? nullptr : control.data();
    host.msg_controllen = control.size();
    if (pingSocket(fd))
        rememberPing(fd, message->vectors, size_t(message->vectorCount));
    int hostFlags = hostMessageFlags(flags);
    if (socketNoSignal(fd))
        hostFlags |= MSG_NOSIGNAL;
    const ssize_t sent = sendmsg(fd, &host, hostFlags);
    if (sent < 0)
        return -darwinError(errno);
    return sent;
}

static int hostBind(int fd, const void* address, uint32_t length)
{
    sockaddr_storage host;
    socklen_t hostLength;
    if (!hostAddress(address, length, &host, &hostLength))
        return -22;
    if (bind(fd, reinterpret_cast<const sockaddr*>(&host), hostLength) < 0)
        return -darwinError(errno);
    return 0;
}

static int hostConnect(int fd, const void* address, uint32_t length)
{
    sockaddr_storage host;
    socklen_t hostLength;
    if (!hostAddress(address, length, &host, &hostLength))
        return -22;
    if (connect(fd, reinterpret_cast<const sockaddr*>(&host), hostLength) < 0)
        return -darwinError(errno);
    return 0;
}

static int hostGetnameinfo(const void* address, uint32_t length, char* node, uint32_t nodeSize, char* service, uint32_t serviceSize, int flags)
{
    sockaddr_storage host;
    socklen_t hostLength;
    if (!hostAddress(address, length, &host, &hostLength))
        return 5;
    int hostFlags = 0;
    if (flags & 0x1)
        hostFlags |= NI_NOFQDN;
    if (flags & 0x2)
        hostFlags |= NI_NUMERICHOST;
    if (flags & 0x4)
        hostFlags |= NI_NAMEREQD;
    if (flags & 0x8)
        hostFlags |= NI_NUMERICSERV;
    if (flags & 0x10)
        hostFlags |= NI_DGRAM;
    return resolverError(getnameinfo(reinterpret_cast<const sockaddr*>(&host), hostLength, node, nodeSize, service, serviceSize, hostFlags));
}

// An error of the network for a request, as the ICMP message a raw socket of Darwin gets; 0 when there is none.
static ssize_t receivePingError(int fd, bool v6, std::vector<uint8_t>& packet, sockaddr_storage* source, socklen_t* sourceLength)
{
    uint8_t data[1280];
    alignas(cmsghdr) uint8_t control[512];
    sockaddr_storage target = {};
    iovec vector = {data, sizeof data};
    msghdr message = {};
    message.msg_name = &target;
    message.msg_namelen = sizeof target;
    message.msg_iov = &vector;
    message.msg_iovlen = 1;
    message.msg_control = control;
    message.msg_controllen = sizeof control;
    const ssize_t size = recvmsg(fd, &message, MSG_ERRQUEUE | MSG_DONTWAIT);
    if (size < 0)
        return errno == EAGAIN ? 0 : -errno;
    const PacketInformation information = readControl(message);
    const sock_extended_err* error = information.error;
    if (!error)
        return 0;
    if (error->ee_origin != (v6 ? SO_EE_ORIGIN_ICMP6 : SO_EE_ORIGIN_ICMP))
        return -int(error->ee_errno);
    in_addr local = information.destination;
    in6_addr local6 = information.destination6;
    if (!information.hasDestination) {
        sockaddr_storage name;
        socklen_t length = sizeof name;
        if (getsockname(fd, reinterpret_cast<sockaddr*>(&name), &length) >= 0) {
            if (v6)
                local6 = reinterpret_cast<const sockaddr_in6*>(&name)->sin6_addr;
            else
                local = reinterpret_cast<const sockaddr_in*>(&name)->sin_addr;
        }
    }
    restorePing(fd, data, size_t(size), true);
    memset(source, 0, sizeof *source);
    if (!v6) {
        const sockaddr_in* offender = reinterpret_cast<const sockaddr_in*>(SO_EE_OFFENDER(error));
        packet.assign(size_t(size) + 48, 0);
        writeIpHeader(packet.data(), uint16_t(size + 28), information.tos, information.ttl, offender->sin_addr, local);
        uint8_t* icmp = packet.data() + 20;
        icmp[0] = error->ee_type;
        icmp[1] = error->ee_code;
        if (error->ee_type == 3 && error->ee_code == 4)
            store16(icmp + 6, uint16_t(error->ee_info));
        else if (error->ee_type == 12)
            icmp[4] = uint8_t(error->ee_info);
        else if (error->ee_type == 5)
            store32(icmp + 4, error->ee_info);
        uint8_t* inner = icmp + 8;
        inner[0] = 0x45;
        store16(inner + 2, uint16_t(size + 20));
        inner[8] = 1;
        inner[9] = IPPROTO_ICMP;
        memcpy(inner + 12, &local, sizeof local);
        memcpy(inner + 16, &reinterpret_cast<const sockaddr_in*>(&target)->sin_addr, sizeof(in_addr));
        store16(inner + 10, checksum(sum16(inner, 20)));
        memcpy(inner + 20, data, size_t(size));
        store16(icmp + 2, checksum(sum16(icmp, size_t(size) + 28)));
        sockaddr_in* address = reinterpret_cast<sockaddr_in*>(source);
        address->sin_family = AF_INET;
        address->sin_addr = offender->sin_addr;
        *sourceLength = sizeof *address;
        return ssize_t(packet.size());
    }
    const sockaddr_in6* offender = reinterpret_cast<const sockaddr_in6*>(SO_EE_OFFENDER(error));
    packet.assign(size_t(size) + 48, 0);
    packet[0] = error->ee_type;
    packet[1] = error->ee_code;
    store32(packet.data() + 4, error->ee_info);
    uint8_t* inner = packet.data() + 8;
    inner[0] = 0x60;
    store16(inner + 4, uint16_t(size));
    inner[6] = IPPROTO_ICMPV6;
    inner[7] = 1;
    memcpy(inner + 8, &local6, sizeof local6);
    memcpy(inner + 24, &reinterpret_cast<const sockaddr_in6*>(&target)->sin6_addr, sizeof(in6_addr));
    memcpy(inner + 40, data, size_t(size));
    uint8_t pseudo[40] = {};
    memcpy(pseudo, &offender->sin6_addr, sizeof(in6_addr));
    memcpy(pseudo + 16, &local6, sizeof local6);
    store32(pseudo + 32, uint32_t(packet.size()));
    pseudo[39] = IPPROTO_ICMPV6;
    store16(packet.data() + 2, checksum(sum16(packet.data(), packet.size(), sum16(pseudo, sizeof pseudo))));
    sockaddr_in6* address = reinterpret_cast<sockaddr_in6*>(source);
    address->sin6_family = AF_INET6;
    address->sin6_addr = offender->sin6_addr;
    *sourceLength = sizeof *address;
    return ssize_t(packet.size());
}

static ssize_t receivePing(int fd, bool v6, int flags, std::vector<uint8_t>& packet, sockaddr_storage* source, socklen_t* sourceLength)
{
    if (const ssize_t size = receivePingError(fd, v6, packet, source, sourceLength))
        return size;
    uint8_t data[65536];
    alignas(cmsghdr) uint8_t control[256];
    iovec vector = {data, sizeof data};
    msghdr message = {};
    message.msg_name = source;
    message.msg_namelen = sizeof *source;
    message.msg_iov = &vector;
    message.msg_iovlen = 1;
    message.msg_control = control;
    message.msg_controllen = sizeof control;
    const ssize_t size = recvmsg(fd, &message, flags);
    if (size < 0) {
        const int error = errno;
        if (error != EAGAIN && error != EINTR) {
            if (const ssize_t found = receivePingError(fd, v6, packet, source, sourceLength))
                return found;
        }
        return -error;
    }
    *sourceLength = message.msg_namelen;
    restorePing(fd, data, size_t(size), false);
    if (!v6) {
        PacketInformation information = readControl(message);
        if (!information.hasDestination) {
            sockaddr_storage name;
            socklen_t length = sizeof name;
            if (getsockname(fd, reinterpret_cast<sockaddr*>(&name), &length) >= 0)
                information.destination = reinterpret_cast<const sockaddr_in*>(&name)->sin_addr;
        }
        packet.assign(size_t(size) + 20, 0);
        writeIpHeader(packet.data(), uint16_t(size), information.tos, information.ttl, reinterpret_cast<const sockaddr_in*>(source)->sin_addr,
                      information.destination);
        memcpy(packet.data() + 20, data, size_t(size));
        return ssize_t(packet.size());
    }
    packet.assign(data, data + size);
    return size;
}

// Whether a socket is an ICMP one, and of which family.
static std::optional<bool> pingFamily(int fd)
{
    std::lock_guard lock(pingMutex);
    const auto found = pings.find(fd);
    if (found == pings.end())
        return std::nullopt;
    return found->second.v6;
}

static long hostRecvmsg(int fd, DarwinMessage* message, int flags)
{
    if (const std::optional<bool> v6 = pingFamily(fd)) {
        std::vector<uint8_t> packet;
        sockaddr_storage source;
        socklen_t sourceLength = sizeof source;
        const ssize_t size = receivePing(fd, *v6, hostMessageFlags(flags), packet, &source, &sourceLength);
        if (size < 0)
            return -darwinError(int(-size));
        size_t copied = 0;
        for (int i = 0; i < message->vectorCount && copied < packet.size(); i++) {
            const size_t part = std::min(message->vectors[i].iov_len, packet.size() - copied);
            memcpy(message->vectors[i].iov_base, packet.data() + copied, part);
            copied += part;
        }
        if (message->name)
            darwinAddress(&source, sourceLength, message->name, &message->nameLength);
        message->controlLength = 0;
        message->flags = copied < packet.size() ? 0x10 : 0;
        return flags & 0x10 ? size : ssize_t(copied);
    }
    sockaddr_storage address;
    msghdr host = {};
    if (message->name) {
        host.msg_name = &address;
        host.msg_namelen = sizeof address;
    }
    host.msg_iov = message->vectors;
    host.msg_iovlen = size_t(message->vectorCount);
    std::vector<uint8_t> control(message->control && message->controlLength ? size_t(message->controlLength) * 2 + 64 : 0);
    host.msg_control = control.data();
    host.msg_controllen = control.size();
    const ssize_t size = recvmsg(fd, &host, hostMessageFlags(flags) | MSG_CMSG_CLOEXEC);
    if (size < 0)
        return -darwinError(errno);
    if (message->name)
        darwinAddress(&address, host.msg_namelen, message->name, &message->nameLength);
    message->flags = darwinMessageFlags(host.msg_flags);
    uint8_t* out = static_cast<uint8_t*>(message->control);
    const size_t capacity = message->controlLength;
    size_t used = 0;
    bool truncated = false;
    for (cmsghdr* header = CMSG_FIRSTHDR(&host); header; header = CMSG_NXTHDR(&host, header)) {
        const size_t length = header->cmsg_len - CMSG_LEN(0);
        DarwinControl darwin = {uint32_t(length + sizeof darwin), 0xffff, 0};
        if (header->cmsg_level == SOL_SOCKET && header->cmsg_type == SCM_RIGHTS)
            darwin.type = 1;
        else if (header->cmsg_level == SOL_SOCKET && header->cmsg_type == SCM_TIMESTAMP)
            darwin.type = 2;
        else
            continue;
        const size_t next = used + ((darwin.length + 3) & ~size_t(3));
        if (!out || next > capacity) {
            if (header->cmsg_type == SCM_RIGHTS) {
                const int* fds = reinterpret_cast<const int*>(CMSG_DATA(header));
                for (size_t i = 0; i < length / sizeof(int); i++)
                    close(fds[i]);
            }
            truncated = true;
            continue;
        }
        memcpy(out + used, &darwin, sizeof darwin);
        memcpy(out + used + sizeof darwin, CMSG_DATA(header), length);
        used = next;
    }
    message->controlLength = uint32_t(used);
    if (truncated)
        message->flags |= 0x20;
    return size;
}

static long hostRecvfrom(int fd, void* data, size_t capacity, int flags, void* address, uint32_t* length)
{
    sockaddr_storage source;
    socklen_t sourceLength = sizeof source;
    if (const std::optional<bool> v6 = pingFamily(fd)) {
        std::vector<uint8_t> packet;
        const ssize_t size = receivePing(fd, *v6, hostMessageFlags(flags), packet, &source, &sourceLength);
        if (size < 0)
            return -darwinError(int(-size));
        const size_t copied = std::min(packet.size(), capacity);
        if (copied)
            memcpy(data, packet.data(), copied);
        if (address && sourceLength) {
            if (length)
                darwinAddress(&source, sourceLength, address, length);
        } else if (length) {
            *length = 0;
        }
        return flags & 0x10 ? size : ssize_t(copied);
    }
    const ssize_t size = recvfrom(fd, data, capacity, hostMessageFlags(flags), reinterpret_cast<sockaddr*>(&source), &sourceLength);
    if (size < 0)
        return -darwinError(errno);
    if (address && sourceLength) {
        if (length)
            darwinAddress(&source, sourceLength, address, length);
    } else if (length) {
        *length = 0;
    }
    return size;
}

static int hostSocket(int family, int type, int protocol)
{
    const int hostFamily = family == 30 ? AF_INET6 : family;
    const int fd = socket(hostFamily, type, protocol);
    if (fd < 0)
        return -darwinError(errno);
    if (type == SOCK_DGRAM) {
        const int on = 1;
        if (hostFamily == AF_INET && protocol == IPPROTO_ICMP) {
            setsockopt(fd, IPPROTO_IP, IP_RECVERR, &on, sizeof on);
            setsockopt(fd, IPPROTO_IP, IP_PKTINFO, &on, sizeof on);
            setsockopt(fd, IPPROTO_IP, IP_RECVTTL, &on, sizeof on);
            setsockopt(fd, IPPROTO_IP, IP_RECVTOS, &on, sizeof on);
        } else if (hostFamily == AF_INET6 && protocol == IPPROTO_ICMPV6) {
            setsockopt(fd, IPPROTO_IPV6, IPV6_RECVERR, &on, sizeof on);
            setsockopt(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on, sizeof on);
        } else {
            return fd;
        }
        std::lock_guard lock(pingMutex);
        pings[fd] = {hostFamily == AF_INET6, {}};
    }
    return fd;
}

bool socketNoSignal(int fd)
{
    if (!anyNoSignal)
        return false;
    std::lock_guard lock(noSignalMutex);
    return noSignalSockets.contains(fd);
}

void setSocketNoSignal(int fd, bool on)
{
    std::lock_guard lock(noSignalMutex);
    if (on)
        noSignalSockets.insert(fd);
    else
        noSignalSockets.erase(fd);
    anyNoSignal.store(!noSignalSockets.empty(), std::memory_order_relaxed);
}

static int hostSetsockopt(int fd, int level, int name, const void* value, uint32_t length)
{
    if (level == 0xffff && name == 0x1022) {
        int on = 0;
        memcpy(&on, value, std::min<size_t>(length, sizeof on));
        setSocketNoSignal(fd, on != 0);
        return 0;
    }
    if ((level == IPPROTO_IP && name == 25) || (level == IPPROTO_IPV6 && name == 125)) {
        unsigned index = 0;
        memcpy(&index, value, std::min<size_t>(length, sizeof index));
        char device[IF_NAMESIZE] = {};
        if (index != 0 && !if_indextoname(index, device))
            return -darwinError(errno);
        if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, device, socklen_t(strlen(device))) < 0)
            return -darwinError(errno);
        return 0;
    }
    if (level == IPPROTO_IP && name == 28) {
        int on = 0;
        memcpy(&on, value, std::min<size_t>(length, sizeof on));
        const int discover = on ? IP_PMTUDISC_DO : IP_PMTUDISC_WANT;
        if (setsockopt(fd, IPPROTO_IP, IP_MTU_DISCOVER, &discover, sizeof discover) < 0)
            return -darwinError(errno);
        return 0;
    }
    int hostLevel;
    int hostName;
    if (!socketOption(level, name, &hostLevel, &hostName))
        return -42;
    if (hostLevel == SOL_SOCKET && (hostName == SO_RCVTIMEO || hostName == SO_SNDTIMEO)) {
        DarwinTimeval timeout = {};
        memcpy(&timeout, value, std::min<size_t>(length, sizeof timeout));
        const timeval host = {timeout.seconds, timeout.microseconds};
        if (setsockopt(fd, SOL_SOCKET, hostName, &host, sizeof host) < 0)
            return -darwinError(errno);
        return 0;
    }
    if (setsockopt(fd, hostLevel, hostName, value, length) < 0)
        return -darwinError(errno);
    return 0;
}

// Runs a call that may raise SIGPIPE without the signal.
long withoutSignal(const std::function<long()>& call)
{
    sigset_t pipe;
    sigset_t old;
    sigset_t pending;
    sigemptyset(&pipe);
    sigaddset(&pipe, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &pipe, &old);
    sigpending(&pending);
    const bool wasPending = sigismember(&pending, SIGPIPE) == 1;
    const long result = call();
    const int error = errno;
    if (!wasPending && result < 0 && error == EPIPE) {
        const timespec now = {};
        sigtimedwait(&pipe, nullptr, &now);
    }
    pthread_sigmask(SIG_SETMASK, &old, nullptr);
    errno = error;
    return result;
}

void forgetSocket(int fd)
{
    if (anyNoSignal)
        setSocketNoSignal(fd, false);
    std::lock_guard lock(pingMutex);
    pings.erase(fd);
}

bool pingSocket(int fd)
{
    std::lock_guard lock(pingMutex);
    return pings.contains(fd);
}

void addSocketServices(HostServices& services)
{
    services.socket = hostSocket;
    services.socketpair = hostSocketpair;
    services.bind = hostBind;
    services.connect = hostConnect;
    services.listen = hostListen;
    services.accept = hostAccept;
    services.sendto = hostSendto;
    services.recvfrom = hostRecvfrom;
    services.sendmsg = hostSendmsg;
    services.recvmsg = hostRecvmsg;
    services.shutdown = hostShutdown;
    services.getsockopt = hostGetsockopt;
    services.setsockopt = hostSetsockopt;
    services.getsockname = hostGetsockname;
    services.getpeername = hostGetpeername;
    services.inetNtop = hostInetNtop;
    services.inetPton = hostInetPton;
    services.getaddrinfo = hostGetaddrinfo;
    services.getnameinfo = hostGetnameinfo;
}

} // namespace cutefish::reef

// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-3-Clause
//
// Sockets, poll, select and kqueue.
#include "darwin.h"

static void option(int fd, int level, int name)
{
    unsigned char value[128];
    uint32_t size = sizeof value;
    memset(value, 0xaa, sizeof value);
    int result = getsockopt(fd, level, name, value, &size);
    printf("getsockopt %x %x = %d", level, name, result);
    if (result < 0)
        printf(" (%d)\n", errno);
    else
        printf(" size %u value %x\n", size, *(int*)value == getpid() ? 0xffff : *(int*)value);
}

int main(void)
{
    int pair[2], on = 1;
    char buffer[256];
    CHECK(socketpair(1, 1, 0, pair));
    CHECK(send(pair[0], "hello", 5, 0));
    CHECK(recv(pair[1], buffer, sizeof buffer, 2));
    CHECK(recv(pair[1], buffer, sizeof buffer, 0));
    CHECK(recv(pair[1], buffer, sizeof buffer, 0x80));
    struct pollfd fds[2] = {{pair[0], 0x1 | 0x4 | 0x40, 0}, {pair[1], 0x1 | 0x100, 0}};
    CHECK(poll(fds, 2, 0));
    printf("poll %x %x, events %x %x\n", fds[0].revents, fds[1].revents, fds[0].events, fds[1].events);
    CHECK(send(pair[0], "x", 1, 0));
    CHECK(poll(fds, 2, 1000));
    printf("poll %x %x\n", fds[0].revents, fds[1].revents);
    fd_set set;
    memset(&set, 0, sizeof set);
    set.bits[pair[1] / 32] |= 1 << pair[1] % 32;
    struct timeval limit = {1, 500000};
    CHECK(select(pair[1] + 1, &set, NULL, NULL, &limit));
    printf("select %d\n", (set.bits[pair[1] / 32] >> pair[1] % 32) & 1);
    for (int name = 0; name < 8; name++)
        option(pair[0], 0, name);
    static const int names[] = {1, 2, 4, 8, 0x10, 0x20, 0x80, 0x100, 0x200, 0x400, 0x1003, 0x1004, 0x1005, 0x1006, 0x1007, 0x1008, 0x1009, 0x1022};
    for (unsigned i = 0; i < sizeof names / sizeof names[0]; i++)
        option(pair[0], 0xffff, names[i]);
    CHECK(setsockopt(pair[0], 0xffff, 0x1022, &on, sizeof on));
    option(pair[0], 0xffff, 0x1022);
    struct timeval timeout = {2, 250000}, read_back = {0, 0};
    uint32_t length = sizeof read_back;
    CHECK(setsockopt(pair[0], 0xffff, 0x1006, &timeout, sizeof timeout));
    CHECK(getsockopt(pair[0], 0xffff, 0x1006, &read_back, &length));
    printf("timeout %ld %d, %u\n", read_back.tv_sec, read_back.tv_usec, length);
    // A descriptor through a message.
    char control[64];
    struct iovec vector = {"fd", 2};
    struct msghdr message = {NULL, 0, &vector, 1, control, 16, 0};
    struct cmsghdr* header = (struct cmsghdr*)control;
    header->cmsg_len = 16;
    header->cmsg_level = 0xffff;
    header->cmsg_type = 1;
    int pipes[2];
    pipe(pipes);
    memcpy(control + 12, &pipes[1], 4);
    CHECK(sendmsg(pair[0], &message, 0));
    vector.iov_base = buffer;
    vector.iov_len = sizeof buffer;
    memset(control, 0, sizeof control);
    message.msg_controllen = sizeof control;
    recv(pair[1], buffer, 1, 0);
    CHECK(recvmsg(pair[1], &message, 0));
    printf("control %u level %x type %d flags %x\n", header->cmsg_len, header->cmsg_level, header->cmsg_type, message.msg_flags);
    int received;
    memcpy(&received, control + 12, 4);
    CHECK(write(received, "through", 7));
    CHECK(read(pipes[0], buffer, sizeof buffer));
    header->cmsg_type = 9;
    message.msg_controllen = 16;
    vector.iov_len = 1;
    CHECK(sendmsg(pair[0], &message, 0));
    CHECK(shutdown(pair[0], 1));
    CHECK(recv(pair[1], buffer, sizeof buffer, 0));
    CHECK(close(pair[1]));
    CHECK(send(pair[0], "x", 1, 0));
    // The loopback.
    int server = socket(2, 1, 0), client = socket(2, 1, 0);
    struct sockaddr_in address = {16, 2, 0, 0x0100007f, {0}};
    CHECK(setsockopt(server, 0xffff, 4, &on, sizeof on));
    CHECK(bind(server, &address, sizeof address));
    CHECK(bind(server, &address, 8));
    CHECK(listen(server, 5));
    length = sizeof address;
    CHECK(getsockname(server, &address, &length));
    printf("name %d %d %u port %d\n", address.sin_len, address.sin_family, length, address.sin_port != 0);
    CHECK(connect(client, &address, sizeof address));
    struct sockaddr_in peer;
    length = sizeof peer;
    int accepted = accept(server, &peer, &length);
    printf("accepted %d %d %d %x\n", accepted >= 0, peer.sin_len, peer.sin_family, peer.sin_addr);
    CHECK(getpeername(accepted, &peer, &length));
    CHECK(send(client, "tcp", 3, 0));
    CHECK(recvfrom(accepted, buffer, sizeof buffer, 0, &peer, &length));
    option(client, 6, 1);
    CHECK(setsockopt(client, 6, 1, &on, sizeof on));
    option(client, 6, 1);
    option(client, 6, 0x10);
    option(client, 6, 0x77);
    for (int name = 0; name < 30; name++)
        option(client, 0, name);
    int six = socket(30, 2, 0);
    CHECK(six >= 0);
    for (int name = 0; name < 70; name++)
        option(six, 41, name);
    CHECK(socket(99, 1, 0));
    close(client);
    CHECK(connect(socket(2, 1, 0), (struct sockaddr_in[]){{16, 2, 0x0100, 0x0100007f, {0}}}, 16));
    // Unix sockets.
    struct sockaddr_un unix_address = {0, 1, "reef-test-socket"};
    unlink(unix_address.sun_path);
    int stream = socket(1, 1, 0);
    CHECK(bind(stream, &unix_address, sizeof unix_address));
    CHECK(listen(stream, 1));
    int other = socket(1, 1, 0);
    CHECK(connect(other, &unix_address, sizeof unix_address));
    length = sizeof unix_address;
    CHECK(getpeername(other, &unix_address, &length));
    printf("unix %d %d %u %s\n", unix_address.sun_len, unix_address.sun_family, length, unix_address.sun_path);
    for (int name = 0; name < 4; name++)
        option(other, 0, name);
    unlink("reef-test-socket");
    // Names.
    char text[64];
    CHECK(inet_pton(2, "127.0.0.1", buffer));
    printf("%s\n", inet_ntop(2, buffer, text, sizeof text));
    CHECK(inet_pton(30, "fe80::1", buffer));
    printf("%s\n", inet_ntop(30, buffer, text, sizeof text));
    CHECK(inet_pton(30, "bad", buffer));
    struct addrinfo hints = {0x1004, 2, 1, 0, 0, NULL, NULL, NULL}, *found = NULL;
    int error = getaddrinfo("127.0.0.1", "80", &hints, &found);
    printf("getaddrinfo %d\n", error);
    for (struct addrinfo* entry = found; entry; entry = entry->ai_next)
        printf("flags %x family %d type %d protocol %d length %u: %d %d port %x address %x\n", entry->ai_flags, entry->ai_family, entry->ai_socktype,
               entry->ai_protocol, entry->ai_addrlen, entry->ai_addr->sa_len, entry->ai_addr->sa_family, ((struct sockaddr_in*)entry->ai_addr)->sin_port,
               ((struct sockaddr_in*)entry->ai_addr)->sin_addr);
    if (found) {
        char node[128], service[32];
        error = getnameinfo(found->ai_addr, found->ai_addrlen, node, sizeof node, service, sizeof service, 2 | 8);
        printf("getnameinfo %d %s %s\n", error, node, service);
        freeaddrinfo(found);
    }
    hints.ai_flags = 4;
    error = getaddrinfo("not an address", NULL, &hints, &found);
    printf("getaddrinfo %d\n", error);
    // kqueue.
    int queue = kqueue();
    CHECK(queue >= 0);
    struct kevent changes[4] = {{pipes[0], -1, 1, 0, 0, (void*)1}, {7, -7, 1 | 0x10, 0, 20, (void*)2}, {1, -10, 1, 0, 0, (void*)3}};
    CHECK(kevent(queue, changes, 3, NULL, 0, NULL));
    write(pipes[1], "kq", 2);
    struct kevent trigger = {1, -10, 0, 0x01000000, 0, (void*)3};
    CHECK(kevent(queue, &trigger, 1, NULL, 0, NULL));
    usleep(60000);
    struct kevent events[8];
    struct timespec wait = {1, 0};
    int count = kevent(queue, NULL, 0, events, 8, &wait);
    int seen = 0;
    for (int i = 0; i < count; i++)
        seen |= 1 << (int)(long)events[i].udata;
    printf("kevent %d seen %x\n", count, seen);
    return 0;
}

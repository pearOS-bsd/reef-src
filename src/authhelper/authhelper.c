// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The process that runs programs with the rights an authorization gave: it listens on a socket of the calling user,
// checks the cookie it got on standard input, and starts what it is sent (Mach-O programs through the loader).

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

enum { CookieSize = 16, MaxArguments = 4096, MaxData = 1 << 20 };

// Reads exactly `size` bytes; a descriptor that comes with them is stored in `passed`.
static bool receive(int fd, void *buffer, size_t size, int *passed)
{
    size_t done = 0;
    while (done < size) {
        struct iovec part = {(char *)buffer + done, size - done};
        _Alignas(struct cmsghdr) char control[CMSG_SPACE(sizeof(int))];
        struct msghdr message;
        memset(&message, 0, sizeof(message));
        message.msg_iov = &part;
        message.msg_iovlen = 1;
        message.msg_control = control;
        message.msg_controllen = sizeof(control);
        ssize_t got = recvmsg(fd, &message, 0);
        if (got <= 0)
            return false;
        for (struct cmsghdr *header = CMSG_FIRSTHDR(&message); header; header = CMSG_NXTHDR(&message, header)) {
            if (header->cmsg_level == SOL_SOCKET && header->cmsg_type == SCM_RIGHTS && passed)
                memcpy(passed, CMSG_DATA(header), sizeof(int));
        }
        done += (size_t)got;
    }
    return true;
}

static bool isMachO(const char *path)
{
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return false;
    uint32_t magic = 0;
    ssize_t got = pread(fd, &magic, sizeof(magic), 0);
    close(fd);
    return got == sizeof(magic)
        && (magic == 0xfeedface || magic == 0xfeedfacf || magic == 0xcefaedfe || magic == 0xcffaedfe
            || magic == 0xcafebabe || magic == 0xbebafeca);
}

// The error of starting `command`, 0 once it runs.
static int start(char **command, int io)
{
    int status[2];
    pipe2(status, O_CLOEXEC);
    pid_t child = fork();
    if (child == 0) {
        setsid();
        if (io >= 0) {
            dup2(io, 0);
            dup2(io, 1);
        }
        signal(SIGCHLD, SIG_DFL);
        signal(SIGPIPE, SIG_DFL);
        execv(command[0], command);
        int error = errno;
        write(status[1], &error, sizeof(error));
        _exit(127);
    }
    close(status[1]);
    int error = 0;
    if (child < 0)
        error = errno;
    else if (read(status[0], &error, sizeof(error)) != sizeof(error))
        error = 0;
    close(status[0]);
    return error;
}

static void serve(int connection, const unsigned char *cookie, char *loader)
{
    unsigned char received[CookieSize];
    uint32_t count = 0;
    int io = -1;
    int32_t result = EPERM;
    if (receive(connection, received, sizeof(received), &io) && memcmp(received, cookie, CookieSize) == 0
        && receive(connection, &count, sizeof(count), &io) && count >= 1 && count <= MaxArguments) {
        uint32_t size = 0;
        if (receive(connection, &size, sizeof(size), &io) && size <= MaxData) {
            char *data = malloc((size_t)size + 1);
            if (receive(connection, data, size, &io)) {
                data[size] = '\0';
                char **args = calloc((size_t)count + 2, sizeof(char *));
                uint32_t n = 0;
                for (size_t offset = 0; n < count && offset < size;) {
                    args[n++] = data + offset;
                    offset += strlen(data + offset) + 1;
                }
                char **command = args;
                if (isMachO(args[0])) {
                    command = calloc((size_t)count + 2, sizeof(char *));
                    command[0] = loader;
                    for (uint32_t i = 0; i < n; i++)
                        command[i + 1] = args[i];
                }
                result = start(command, io);
                if (io >= 0)
                    close(io);
                write(connection, &result, sizeof(result));
                if (command != args)
                    free(command);
                free(args);
                free(data);
                return;
            }
            free(data);
        }
    }
    if (io >= 0)
        close(io);
    write(connection, &result, sizeof(result));
}

int main(int argc, char **argv)
{
    if (argc < 4)
        return 2;
    char *loader = argv[1];
    const char *path = argv[2];
    uid_t uid = (uid_t)strtoul(argv[3], NULL, 10);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    unsigned char cookie[CookieSize];
    if (!receive(0, cookie, sizeof(cookie), NULL))
        return 1;
    int listener = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    struct sockaddr_un address = {.sun_family = AF_UNIX};
    if (strlen(path) >= sizeof(address.sun_path))
        return 1;
    strcpy(address.sun_path, path);
    unlink(path);
    if (bind(listener, (struct sockaddr *)&address, sizeof(address)) != 0 || listen(listener, 8) != 0)
        return 1;
    chown(path, uid, (gid_t)-1);
    chmod(path, 0600);
    char ready = 1;
    write(0, &ready, 1);

    for (;;) {
        struct pollfd fds[2] = {{0, POLLIN, 0}, {listener, POLLIN, 0}};
        if (poll(fds, 2, -1) < 0) {
            if (errno == EINTR)
                continue;
            break;
        }
        if (fds[0].revents) {
            char byte;
            if (read(0, &byte, 1) <= 0)
                break;
        }
        if (!(fds[1].revents & POLLIN))
            continue;
        int connection = accept4(listener, NULL, NULL, SOCK_CLOEXEC);
        if (connection < 0)
            continue;
        serve(connection, cookie, loader);
        close(connection);
    }
    unlink(path);
    return 0;
}

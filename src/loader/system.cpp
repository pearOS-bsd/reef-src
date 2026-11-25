// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Host services over Linux system calls: poll and select with Darwin's flags, signal masks and time values, epoll, eventfd,
// timerfd, futex, sysconf and the terminal ioctls.

#include "host.h"

#include <cerrno>
#include <climits>
#include <poll.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>
#include <sys/uio.h>
#include <unistd.h>

namespace cutefish::reef {

static int hostSignalfd(int fd, uint32_t mask, int flags)
{
    return emulatedSignalfd(fd, mask, flags);
}

static long hostWritev(int fd, const iovec* vectors, int count)
{
    long written;
    if (!socketNoSignal(fd))
        written = writev(fd, vectors, count);
    else
        written = withoutSignal([&] { return writev(fd, vectors, count); });
    if (written < 0)
        return -darwinError(errno);
    return written;
}

// Darwin's POLLWRBAND is 0x100; Linux has POLLWRNORM there.
static short linuxPollEvents(short events)
{
    short host = events & 0xff;
    if (events & 0x100)
        host |= POLLWRBAND;
    return host;
}

static short darwinPollEvents(short events)
{
    short darwin = events & 0xff;
    if (events & POLLWRNORM)
        darwin |= POLLOUT;
    if (events & POLLWRBAND)
        darwin |= 0x100;
    return darwin;
}

static int hostPpoll(pollfd* fds, unsigned count, const timespec* timeout, const uint32_t* mask)
{
    sigset_t set;
    if (mask) {
        pthread_sigmask(SIG_SETMASK, nullptr, &set);
        for (int signal = 1; signal < 32; signal++)
            sigdelset(&set, signal);
        addDarwinSignals(*mask, &set);
    }
    for (unsigned i = 0; i < count; i++)
        fds[i].events = linuxPollEvents(fds[i].events);
    const int ready = ppoll(fds, count, timeout, mask ? &set : nullptr);
    const int error = errno;
    for (unsigned i = 0; i < count; i++) {
        // An error for an ICMP socket is a message to read.
        if ((fds[i].revents & POLLERR) && (fds[i].events & POLLIN) && pingSocket(fds[i].fd))
            fds[i].revents = (fds[i].revents & ~POLLERR) | POLLIN;
        fds[i].events = darwinPollEvents(fds[i].events);
        fds[i].revents = darwinPollEvents(fds[i].revents);
    }
    if (ready < 0)
        return -darwinError(error);
    return ready;
}

static int hostIoctl(int fd, unsigned long request, void* argument)
{
    unsigned long host;
    switch (request) {
    case 0x20006601:
        host = FIOCLEX;
        break;
    case 0x20006602:
        host = FIONCLEX;
        break;
    case 0x2000740d:
        host = TIOCEXCL;
        break;
    case 0x2000740e:
        host = TIOCNXCL;
        break;
    case 0x20007461:
        host = TIOCSCTTY;
        break;
    case 0x20007471:
        host = TIOCNOTTY;
        break;
    case 0x40047201:
        host = TIOCOUTQ;
        break;
    case 0x4004667f:
        host = FIONREAD;
        break;
    case 0x40047477:
        host = TIOCGPGRP;
        break;
    case 0x40087468:
        host = TIOCGWINSZ;
        break;
    case 0x8004667d:
        host = FIOASYNC;
        break;
    case 0x8004667e:
        host = FIONBIO;
        break;
    case 0x80047476:
        host = TIOCSPGRP;
        break;
    case 0x80087467:
        host = TIOCSWINSZ;
        break;
    default:
        return -25;
    }
    if (ioctl(fd, host, argument) < 0)
        return -darwinError(errno);
    return 0;
}

static long hostSysconf(int name)
{
    int host;
    switch (name) {
    case 1:
        host = _SC_ARG_MAX;
        break;
    case 2:
        host = _SC_CHILD_MAX;
        break;
    case 3:
        host = _SC_CLK_TCK;
        break;
    case 4:
        host = _SC_NGROUPS_MAX;
        break;
    case 5:
        host = _SC_OPEN_MAX;
        break;
    case 15:
        host = _SC_LINE_MAX;
        break;
    case 29:
        host = _SC_PAGESIZE;
        break;
    case 56:
        host = _SC_IOV_MAX;
        break;
    case 57:
        host = _SC_NPROCESSORS_CONF;
        break;
    case 58:
        host = _SC_NPROCESSORS_ONLN;
        break;
    case 70:
        host = _SC_GETGR_R_SIZE_MAX;
        break;
    case 71:
        host = _SC_GETPW_R_SIZE_MAX;
        break;
    case 72:
        host = _SC_HOST_NAME_MAX;
        break;
    case 73:
        host = _SC_LOGIN_NAME_MAX;
        break;
    case 74:
        host = _SC_MONOTONIC_CLOCK;
        break;
    case 93:
        host = _SC_THREAD_STACK_MIN;
        break;
    case 101:
        host = _SC_TTY_NAME_MAX;
        break;
    case 120:
        host = _SC_SYMLOOP_MAX;
        break;
    case 200:
        host = _SC_PHYS_PAGES;
        break;
    default:
        return -22;
    }
    errno = 0;
    const long value = sysconf(host);
    if (value < 0) {
        // No limit; a negative value is an error for the caller.
        if (errno == 0)
            return LONG_MIN;
        return -darwinError(errno);
    }
    return value;
}

static int hostSelect(int count, void* read, void* write, void* error, DarwinTimeval* timeout)
{
    int ready;
    int saved;
    if (!timeout) {
        ready = select(count, static_cast<fd_set*>(read), static_cast<fd_set*>(write), static_cast<fd_set*>(error), nullptr);
        saved = errno;
    } else {
        timeval host = {timeout->seconds, timeout->microseconds};
        ready = select(count, static_cast<fd_set*>(read), static_cast<fd_set*>(write), static_cast<fd_set*>(error), &host);
        saved = errno;
        timeout->seconds = host.tv_sec;
        timeout->microseconds = int(host.tv_usec);
    }
    if (ready < 0)
        return -darwinError(saved);
    return ready;
}

static long hostRead(int fd, void* buffer, size_t size)
{
    const ssize_t done = read(fd, buffer, size);
    if (done < 0)
        return -darwinError(errno);
    return done;
}

static int hostGethostname(char* name, size_t size)
{
    if (gethostname(name, size) < 0)
        return -darwinError(errno);
    return 0;
}

static int hostTimerfdCreate(int clock, int flags)
{
    clockid_t host;
    if (!hostClock(clock, &host))
        return -22;
    const int fd = timerfd_create(host, flags);
    if (fd < 0)
        return -darwinError(errno);
    return fd;
}

static int hostPoll(pollfd* fds, unsigned count, int timeout)
{
    if (timeout >= 0) {
        const timespec limit = {timeout / 1000, (timeout % 1000) * 1000000L};
        return hostPpoll(fds, count, &limit, nullptr);
    }
    return hostPpoll(fds, count, nullptr, nullptr);
}

static int hostPselect(int count, void* read, void* write, void* error, const timespec* timeout, const uint32_t* mask)
{
    sigset_t set;
    sigset_t* signals = nullptr;
    if (mask) {
        signals = &set;
        pthread_sigmask(SIG_SETMASK, nullptr, signals);
        for (int signal = 1; signal < 32; signal++)
            sigdelset(signals, signal);
        addDarwinSignals(*mask, signals);
    }
    const int ready = pselect(count, static_cast<fd_set*>(read), static_cast<fd_set*>(write), static_cast<fd_set*>(error), timeout, signals);
    if (ready < 0)
        return -darwinError(errno);
    return ready;
}

static long hostReadv(int fd, const iovec* vectors, int count)
{
    const ssize_t done = readv(fd, vectors, count);
    if (done < 0)
        return -darwinError(errno);
    return done;
}

static long hostFutex(uint32_t* address, int operation, uint32_t value, const timespec* timeout, uint32_t* second)
{
    const long result = syscall(SYS_futex, address, operation, value, timeout, second);
    if (result < 0)
        return -darwinError(errno);
    return result;
}

static int hostEpollCreate(int flags)
{
    const int fd = epoll_create1(flags);
    if (fd < 0)
        return -darwinError(errno);
    return fd;
}

static int hostTimerfdGettime(int fd, itimerspec* value)
{
    if (timerfd_gettime(fd, value) < 0)
        return -darwinError(errno);
    return 0;
}

static int hostEventfd(unsigned value, int flags)
{
    const int fd = eventfd(value, flags);
    if (fd < 0)
        return -darwinError(errno);
    return fd;
}

static int hostTimerfdSettime(int fd, int flags, const itimerspec* value, itimerspec* old)
{
    if (timerfd_settime(fd, flags, value, old) < 0)
        return -darwinError(errno);
    return 0;
}

static int hostEpollWait(int epoll, epoll_event* events, int count, int timeout)
{
    const int ready = epoll_wait(epoll, events, count, timeout);
    if (ready < 0)
        return -darwinError(errno);
    return ready;
}

static int hostEpollCtl(int epoll, int operation, int fd, epoll_event* event)
{
    if (epoll_ctl(epoll, operation, fd, event) < 0)
        return -darwinError(errno);
    return 0;
}

void addSystemServices(HostServices& services)
{
    services.select = hostSelect;
    services.pselect = hostPselect;
    services.ppoll = hostPpoll;
    services.getpassword = getPassword;
    services.gethostname = hostGethostname;
    services.sysconf = hostSysconf;
    services.ioctl = hostIoctl;
    services.poll = hostPoll;
    services.readv = hostReadv;
    services.futex = hostFutex;
    services.writev = hostWritev;
    services.epollCreate = hostEpollCreate;
    services.epollCtl = hostEpollCtl;
    services.epollWait = hostEpollWait;
    services.eventfd = hostEventfd;
    services.timerfdCreate = hostTimerfdCreate;
    services.timerfdSettime = hostTimerfdSettime;
    services.timerfdGettime = hostTimerfdGettime;
    services.signalfd = hostSignalfd;
    services.readPlain = hostRead;
}

} // namespace cutefish::reef

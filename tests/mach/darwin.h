// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-3-Clause
//
// What the test programs use of Darwin's C library, declared here: there is no SDK where they are built.
#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef long ssize_t;
typedef unsigned long size_t;
typedef int pid_t;
typedef uint32_t uid_t;
typedef long time_t;
typedef uint32_t sigset_t;

#define NULL ((void*)0)

int printf(const char*, ...) __attribute__((format(printf, 1, 2)));
int snprintf(char*, size_t, const char*, ...) __attribute__((format(printf, 3, 4)));
int puts(const char*);
extern void* __stdoutp;
int fflush(void*);
int* __error(void);
#define errno (*__error())
char* strerror(int);
void* malloc(size_t);
void* calloc(size_t, size_t);
void* realloc(void*, size_t);
void free(void*);
void* memcpy(void*, const void*, size_t);
void* memset(void*, int, size_t);
int memcmp(const void*, const void*, size_t);
size_t strlen(const char*);
int strcmp(const char*, const char*);
int strncmp(const char*, const char*, size_t);
char* strrchr(const char*, int);
char* getenv(const char*);
int setenv(const char*, const char*, int);
void exit(int) __attribute__((noreturn));
void _exit(int) __attribute__((noreturn));
int atexit(void (*)(void));
void qsort(void*, size_t, size_t, int (*)(const void*, const void*));

// unistd.h, fcntl.h, sys/stat.h
int open(const char*, int, ...);
int close(int);
ssize_t read(int, void*, size_t);
ssize_t write(int, const void*, size_t);
ssize_t pread(int, void*, size_t, long long);
ssize_t pwrite(int, const void*, size_t, long long);
long long lseek(int, long long, int);
int ftruncate(int, long long);
int unlink(const char*);
int rename(const char*, const char*);
int mkdir(const char*, uint16_t);
int rmdir(const char*);
int symlink(const char*, const char*);
ssize_t readlink(const char*, char*, size_t);
int link(const char*, const char*);
int access(const char*, int);
int chmod(const char*, uint16_t);
int chdir(const char*);
char* getcwd(char*, size_t);
char* realpath(const char*, char*);
int dup(int);
int dup2(int, int);
int pipe(int*);
int fcntl(int, int, ...);
int isatty(int);
pid_t getpid(void);
pid_t getppid(void);
uid_t getuid(void);
int gethostname(char*, size_t);
long sysconf(int);
long pathconf(const char*, int);
unsigned sleep(unsigned);
int usleep(unsigned);
unsigned alarm(unsigned);

#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_RDWR 0x0002
#define O_NONBLOCK 0x0004
#define O_APPEND 0x0008
#define O_CREAT 0x0200
#define O_TRUNC 0x0400
#define O_EXCL 0x0800
#define O_NOFOLLOW 0x0100
#define O_DIRECTORY 0x100000
#define O_CLOEXEC 0x1000000

struct timespec { time_t tv_sec; long tv_nsec; };
struct timeval { time_t tv_sec; int tv_usec; };
struct stat {
    int st_dev; uint16_t st_mode; uint16_t st_nlink; uint64_t st_ino; uint32_t st_uid; uint32_t st_gid; int st_rdev;
    struct timespec st_atimespec, st_mtimespec, st_ctimespec, st_birthtimespec;
    long long st_size; long long st_blocks; int st_blksize; uint32_t st_flags; uint32_t st_gen; int st_lspare; long long st_qspare[2];
};
int stat(const char*, struct stat*) __asm("_stat$INODE64");
int lstat(const char*, struct stat*) __asm("_lstat$INODE64");
int fstat(int, struct stat*) __asm("_fstat$INODE64");
struct dirent { uint64_t d_ino; uint64_t d_seekoff; uint16_t d_reclen; uint16_t d_namlen; uint8_t d_type; char d_name[1024]; };
void* opendir(const char*) __asm("_opendir$INODE64");
struct dirent* readdir(void*) __asm("_readdir$INODE64");
int closedir(void*);
int setxattr(const char*, const char*, const void*, size_t, uint32_t, int);
ssize_t getxattr(const char*, const char*, void*, size_t, uint32_t, int);
ssize_t listxattr(const char*, char*, size_t, int);
int removexattr(const char*, const char*, int);

// sys/mman.h
void* mmap(void*, size_t, int, int, int, long long);
int munmap(void*, size_t);
int mprotect(void*, size_t, int);
#define MAP_FAILED ((void*)-1)

// processes
pid_t fork(void);
int execve(const char*, char* const*, char* const*);
pid_t waitpid(pid_t, int*, int);
int kill(pid_t, int);
int raise(int);
int posix_spawn(pid_t*, const char*, void* const*, void* const*, char* const*, char* const*);
int posix_spawn_file_actions_init(void**);
int posix_spawn_file_actions_addopen(void**, int, const char*, int, uint16_t);
int posix_spawn_file_actions_adddup2(void**, int, int);
int posix_spawn_file_actions_addclose(void**, int);
int posix_spawn_file_actions_destroy(void**);
int posix_spawnattr_init(void**);
int posix_spawnattr_setflags(void**, short);
int posix_spawnattr_setsigdefault(void**, const sigset_t*);
int posix_spawnattr_setpgroup(void**, pid_t);
extern char** environ;

// signals
struct siginfo;
struct sigaction { union { void (*handler)(int); void (*action)(int, struct siginfo*, void*); } u; sigset_t sa_mask; int sa_flags; };
struct siginfo { int si_signo, si_errno, si_code; pid_t si_pid; uid_t si_uid; int si_status; void* si_addr; void* si_value; long si_band; unsigned long pad[7]; };
typedef struct { void* ss_sp; size_t ss_size; int ss_flags; } stack_t;
int sigaction(int, const struct sigaction*, struct sigaction*);
int sigprocmask(int, const sigset_t*, sigset_t*);
int pthread_sigmask(int, const sigset_t*, sigset_t*);
int sigpending(sigset_t*);
int sigsuspend(const sigset_t*);
int sigwait(const sigset_t*, int*);
int sigaltstack(const stack_t*, stack_t*);

// threads
typedef void* pthread_t;
int pthread_create(pthread_t*, const void*, void* (*)(void*), void*);
int pthread_join(pthread_t, void**);
int pthread_kill(pthread_t, int);
pthread_t pthread_self(void);
int pthread_main_np(void);
int pthread_setname_np(const char*);
int pthread_getname_np(pthread_t, char*, size_t);
int pthread_key_create(unsigned long*, void (*)(void*));
void* pthread_getspecific(unsigned long);
int pthread_setspecific(unsigned long, const void*);
typedef struct { long sig; char opaque[56]; } pthread_mutex_t;
typedef struct { long sig; char opaque[40]; } pthread_cond_t;
int pthread_mutex_init(pthread_mutex_t*, const void*);
int pthread_mutex_lock(pthread_mutex_t*);
int pthread_mutex_unlock(pthread_mutex_t*);
int pthread_cond_init(pthread_cond_t*, const void*);
int pthread_cond_wait(pthread_cond_t*, pthread_mutex_t*);
int pthread_cond_signal(pthread_cond_t*);

// poll, select, sockets
struct pollfd { int fd; short events; short revents; };
int poll(struct pollfd*, unsigned, int);
typedef struct { int bits[32]; } fd_set;
int select(int, fd_set*, fd_set*, fd_set*, struct timeval*);
struct sockaddr { uint8_t sa_len; uint8_t sa_family; char sa_data[14]; };
struct sockaddr_in { uint8_t sin_len; uint8_t sin_family; uint16_t sin_port; uint32_t sin_addr; char zero[8]; };
struct sockaddr_un { uint8_t sun_len; uint8_t sun_family; char sun_path[104]; };
struct iovec { void* iov_base; size_t iov_len; };
struct msghdr { void* msg_name; uint32_t msg_namelen; struct iovec* msg_iov; int msg_iovlen; void* msg_control; uint32_t msg_controllen; int msg_flags; };
struct cmsghdr { uint32_t cmsg_len; int cmsg_level; int cmsg_type; };
struct addrinfo { int ai_flags, ai_family, ai_socktype, ai_protocol; uint32_t ai_addrlen; char* ai_canonname; struct sockaddr* ai_addr; struct addrinfo* ai_next; };
int socket(int, int, int);
int socketpair(int, int, int, int*);
int bind(int, const void*, uint32_t);
int listen(int, int);
int accept(int, void*, uint32_t*);
int connect(int, const void*, uint32_t);
ssize_t send(int, const void*, size_t, int);
ssize_t recv(int, void*, size_t, int);
ssize_t sendto(int, const void*, size_t, int, const void*, uint32_t);
ssize_t recvfrom(int, void*, size_t, int, void*, uint32_t*);
ssize_t sendmsg(int, const struct msghdr*, int);
ssize_t recvmsg(int, struct msghdr*, int);
int shutdown(int, int);
int getsockopt(int, int, int, void*, uint32_t*);
int setsockopt(int, int, int, const void*, uint32_t);
int getsockname(int, void*, uint32_t*);
int getpeername(int, void*, uint32_t*);
int getaddrinfo(const char*, const char*, const struct addrinfo*, struct addrinfo**);
void freeaddrinfo(struct addrinfo*);
const char* gai_strerror(int);
int getnameinfo(const void*, uint32_t, char*, uint32_t, char*, uint32_t, int);
const char* inet_ntop(int, const void*, char*, uint32_t);
int inet_pton(int, const char*, void*);

// kqueue
struct kevent { unsigned long ident; short filter; uint16_t flags; uint32_t fflags; long data; void* udata; };
int kqueue(void);
int kevent(int, const struct kevent*, int, struct kevent*, int, const struct timespec*);

// time, users, dyld
int clock_gettime(int, struct timespec*);
int gettimeofday(struct timeval*, void*);
int nanosleep(const struct timespec*, struct timespec*);
uint64_t mach_absolute_time(void);
struct passwd { char* pw_name; char* pw_passwd; uid_t pw_uid; uid_t pw_gid; time_t pw_change; char* pw_class; char* pw_gecos; char* pw_dir; char* pw_shell; time_t pw_expire; };
struct passwd* getpwuid(uid_t);
struct passwd* getpwnam(const char*);
void* dlopen(const char*, int);
void* dlsym(void*, const char*);
int dlclose(void*);
char* dlerror(void);
struct dl_info { const char* dli_fname; void* dli_fbase; const char* dli_sname; void* dli_saddr; };
int dladdr(const void*, struct dl_info*);
uint32_t _dyld_image_count(void);
const char* _dyld_get_image_name(uint32_t);

#define CHECK(expression) do { long value_ = (long)(expression); int error_ = errno; printf("%s = %ld", #expression, value_); if (value_ < 0) printf(" (%d %s)", error_, strerror(error_)); printf("\n"); } while (0)

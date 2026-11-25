// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-3-Clause
//
// Threads, thread-local storage, keys, time, users and images.
#include "darwin.h"

static __thread int local = 41;
static __thread char zero[4096];
static pthread_mutex_t mutex;
static pthread_cond_t condition;
static int ready;
static unsigned long key;
static int destroyed;

static void destroy(void* value) { destroyed += (int)(long)value; }

static void* run(void* argument)
{
    local += (int)(long)argument;
    zero[10] = 1;
    pthread_setspecific(key, argument);
    pthread_setname_np("worker");
    char name[64];
    pthread_getname_np(pthread_self(), name, sizeof name);
    printf("thread %ld: local %d, main %d, name %s, specific %ld\n", (long)argument, local, pthread_main_np(), name, (long)pthread_getspecific(key));
    pthread_mutex_lock(&mutex);
    ready++;
    pthread_cond_signal(&condition);
    pthread_mutex_unlock(&mutex);
    return (void*)((long)argument * 10);
}

static void atEnd(void) { printf("at exit\n"); }
int exportedFunction(int value) { return value + 1; }

int main(int argc, char** argv)
{
    atexit(atEnd);
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condition, NULL);
    CHECK(pthread_key_create(&key, destroy));
    for (long i = 1; i < 4; i++) {
        pthread_t thread;
        void* result;
        CHECK(pthread_create(&thread, NULL, run, (void*)i));
        pthread_mutex_lock(&mutex);
        while (ready < i)
            pthread_cond_wait(&condition, &mutex);
        pthread_mutex_unlock(&mutex);
        CHECK(pthread_join(thread, &result));
        printf("result %ld\n", (long)result);
    }
    printf("local %d, zero %d, destroyed %d, main %d\n", local, zero[10], destroyed, pthread_main_np());
    struct timespec now, later;
    for (int clock = 0; clock < 20; clock++) {
        errno = 0;
        int result = clock_gettime(clock, &now);
        printf("clock %d: %d %d, positive %d\n", clock, result, result ? errno : 0, result == 0 && now.tv_sec >= 0);
    }
    clock_gettime(6, &now);
    usleep(2000);
    clock_gettime(6, &later);
    printf("monotonic %d\n", later.tv_sec > now.tv_sec || later.tv_nsec > now.tv_nsec);
    uint64_t start = mach_absolute_time();
    usleep(1000);
    printf("absolute %d\n", mach_absolute_time() > start);
    struct passwd* user = getpwuid(getuid());
    printf("user %s %s %s\n", user->pw_name, user->pw_dir, user->pw_shell);
    printf("unknown user %d\n", getpwnam("no-such-user") == NULL);
    struct dl_info information;
    CHECK(dladdr((void*)exportedFunction, &information));
    printf("%s in %s\n", information.dli_sname, strrchr(information.dli_fname, '/') + 1);
    void* self = dlopen(NULL, 2);
    int (*function)(int) = dlsym(self, "exportedFunction");
    printf("dlsym %d\n", function ? function(4) : -1);
    printf("dlsym missing %d\n", dlsym(self, "noSuchFunction") == NULL);
    printf("dlerror %d\n", dlerror() != NULL);
    void* library = dlopen("/usr/lib/libz.1.dylib", 1);
    printf("libz %d\n", library != NULL && dlsym(library, "zlibVersion") != NULL);
    printf("missing library %d\n", dlopen("/usr/lib/libmissing.dylib", 1) == NULL);
    printf("%s\n", dlerror());
    printf("images %d, first %s\n", _dyld_image_count() > 3, strrchr(_dyld_get_image_name(0), '/') + 1);
    char host[256];
    CHECK(gethostname(host, sizeof host));
    void* memory = malloc(100);
    memory = realloc(memory, 100000);
    memset(memory, 1, 100000);
    free(memory);
    printf("%d %s\n", argc, strrchr(argv[0], '/') + 1);
    return 9;
}

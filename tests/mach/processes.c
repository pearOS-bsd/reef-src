// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-3-Clause
//
// Processes: fork, exec, spawn, waiting, and what the child inherits.
#include "darwin.h"

static void report(const char* what, int status)
{
    printf("%s: exited %d code %d signal %d\n", what, (status & 0x7f) == 0, (status >> 8) & 0xff, status & 0x7f);
}

int main(int argc, char** argv)
{
    int status;
    if (argc > 1 && strcmp(argv[1], "child") == 0) {
        printf("child: %d arguments, %s, %s, VALUE=%s\n", argc, strrchr(argv[0], '/') ? strrchr(argv[0], '/') + 1 : argv[0], argc > 2 ? argv[2] : "-",
               getenv("VALUE") ? getenv("VALUE") : "(none)");
        char buffer[64];
        ssize_t size = read(0, buffer, sizeof buffer);
        printf("child: stdin %ld bytes, fd 9 open %d\n", size, fcntl(9, 1) >= 0);
        fflush(__stdoutp);
        return 5;
    }
    printf("parent %d\n", getpid() > 1);
    fflush(__stdoutp);
    pid_t pid = fork();
    if (pid == 0)
        _exit(7);
    CHECK(waitpid(pid, &status, 0) == pid);
    report("fork", status);
    pid = fork();
    if (pid == 0) {
        raise(15);
        _exit(1);
    }
    waitpid(pid, &status, 0);
    report("raise", status);
    pid = fork();
    if (pid == 0) {
        kill(getpid(), 17);
        _exit(4);
    }
    CHECK(waitpid(pid, &status, 2) == pid);
    printf("stopped %x\n", status);
    CHECK(kill(pid, 19));
    CHECK(waitpid(pid, &status, 0) == pid);
    report("continued", status);
    CHECK(waitpid(pid, &status, 0));
    CHECK(waitpid(-1, &status, 1));
    pid = fork();
    if (pid == 0) {
        char* arguments[] = {"renamed", "child", "exec", NULL};
        char* environment[] = {"VALUE=exec", NULL};
        int null = open("/dev/null", O_RDONLY);
        dup2(null, 0);
        execve(argv[0], arguments, environment);
        _exit(99);
    }
    waitpid(pid, &status, 0);
    report("exec", status);
    char* arguments[] = {argv[0], "child", "spawn", NULL};
    char* environment[] = {"VALUE=spawn", NULL};
    void* actions;
    posix_spawn_file_actions_init(&actions);
    posix_spawn_file_actions_addopen(&actions, 0, "/etc/passwd", O_RDONLY, 0);
    posix_spawn_file_actions_adddup2(&actions, 0, 9);
    CHECK(posix_spawn(&pid, argv[0], &actions, NULL, arguments, environment));
    waitpid(pid, &status, 0);
    report("spawn", status);
    posix_spawn_file_actions_destroy(&actions);
    CHECK(posix_spawn(&pid, "/nonexistent/program", NULL, NULL, arguments, environment));
    void* attributes;
    posix_spawnattr_init(&attributes);
    posix_spawnattr_setflags(&attributes, 0x4000 | 2);
    posix_spawnattr_setpgroup(&attributes, 0);
    posix_spawn_file_actions_init(&actions);
    posix_spawn_file_actions_addopen(&actions, 0, "/dev/null", O_RDONLY, 0);
    posix_spawn_file_actions_adddup2(&actions, 1, 1);
    posix_spawn_file_actions_adddup2(&actions, 2, 2);
    dup2(1, 9);
    CHECK(posix_spawn(&pid, argv[0], &actions, &attributes, arguments, environment));
    waitpid(pid, &status, 0);
    report("spawn with closed files", status);
    char* script[] = {"/bin/sh", "-c", "echo host program $VALUE; exit 6", NULL};
    CHECK(posix_spawn(&pid, "/bin/sh", NULL, NULL, script, environment));
    waitpid(pid, &status, 0);
    report("host program", status);
    CHECK(execve("/etc/passwd", arguments, environment));
    CHECK(execve("/nonexistent", arguments, environment));
    return 0;
}

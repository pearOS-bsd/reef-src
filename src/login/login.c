// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The platform's login: `login -f` starts the user's shell itself, anything else goes to the host's login.

#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char hostLogin[] = "/bin/login";
static const char defaultPath[] = "/usr/local/bin:/usr/bin:/bin";

static void usage(void)
{
    fprintf(stderr, "usage: login [-pq] [-h hostname] [username]\n"
                    "       login -f [-lpq] [-h hostname] [username [prog [arg ...]]]\n");
    exit(1);
}

int main(int argc, char **argv)
{
    bool force = false;
    bool keepDirectory = false;
    bool preserve = false;
    char *host = NULL;

    int option;
    while ((option = getopt(argc, argv, "+fh:lpq")) != -1) {
        switch (option) {
        case 'f':
            force = true;
            break;
        case 'h':
            host = optarg;
            break;
        case 'l':
            keepDirectory = true;
            break;
        case 'p':
            preserve = true;
            break;
        case 'q':
            break;
        default:
            usage();
        }
    }

    if (!force) {
        char *args[6];
        int count = 0;
        args[count++] = (char *)hostLogin;
        if (preserve)
            args[count++] = "-p";
        if (host) {
            args[count++] = "-h";
            args[count++] = host;
        }
        if (optind < argc)
            args[count++] = argv[optind];
        args[count] = NULL;
        execv(hostLogin, args);
        fprintf(stderr, "login: %s: %s\n", hostLogin, strerror(errno));
        return 1;
    }

    uid_t self = getuid();
    struct passwd *user = optind < argc ? getpwnam(argv[optind]) : getpwuid(self);
    if (!user) {
        fprintf(stderr, "login: unknown user %s\n", optind < argc ? argv[optind] : "");
        return 1;
    }
    if (user->pw_uid != self && self != 0) {
        fprintf(stderr, "login: %s: permission denied\n", user->pw_name);
        return 1;
    }
    if (optind < argc)
        optind++;

    char *name = strdup(user->pw_name);
    char *home = strdup(user->pw_dir);
    char *shell = strdup(user->pw_shell && *user->pw_shell ? user->pw_shell : "/bin/sh");
    if (self == 0 && user->pw_uid != 0) {
        if (initgroups(name, user->pw_gid) != 0 || setgid(user->pw_gid) != 0 || setuid(user->pw_uid) != 0) {
            fprintf(stderr, "login: %s: %s\n", name, strerror(errno));
            return 1;
        }
    }

    if (!preserve) {
        char *term = getenv("TERM");
        if (term)
            term = strdup(term);
        clearenv();
        if (term) {
            setenv("TERM", term, 1);
            free(term);
        }
        setenv("PATH", defaultPath, 1);
    }
    setenv("HOME", home, 1);
    setenv("SHELL", shell, 1);
    setenv("USER", name, 1);
    setenv("LOGNAME", name, 1);

    if (!keepDirectory && chdir(home) != 0) {
        fprintf(stderr, "login: no home directory %s, logging in with HOME=/\n", home);
        setenv("HOME", "/", 1);
        if (chdir("/") != 0)
            return 1;
    }

    if (optind < argc) {
        execvp(argv[optind], argv + optind);
        fprintf(stderr, "login: %s: %s\n", argv[optind], strerror(errno));
        return 1;
    }

    const char *base = strrchr(shell, '/');
    base = base ? base + 1 : shell;
    size_t length = strlen(base);
    char *title = malloc(length + 2);
    title[0] = '-';
    memcpy(title + 1, base, length + 1);
    char *args[] = {title, NULL};
    execv(shell, args);
    fprintf(stderr, "login: %s: %s\n", shell, strerror(errno));
    return 1;
}

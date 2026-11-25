// The home of the user is $TEST_HOME (the toolkit asks the password file).
#define _GNU_SOURCE
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
int getpwuid_r(uid_t uid, struct passwd* entry, char* buffer, size_t size, struct passwd** result)
{
    const char* home = getenv("TEST_HOME");
    strncpy(buffer, home ? home : "/", size);
    memset(entry, 0, sizeof *entry);
    entry->pw_uid = uid;
    entry->pw_dir = buffer;
    entry->pw_name = entry->pw_gecos = entry->pw_shell = entry->pw_passwd = buffer;
    *result = entry;
    return 0;
}

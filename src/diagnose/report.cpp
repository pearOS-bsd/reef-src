// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The text of the report, and what its sections need: files, commands, and the report without names.

#include "report.h"

#include <cctype>
#include <cerrno>
#include <csignal>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <poll.h>
#include <pwd.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

namespace cutefish::reef::diagnose {

// A whole word: not beside a letter, a digit, '_' or '-'.
static void replaceWord(std::string& text, const std::string& word, const std::string& replacement)
{
    auto inWord = [](unsigned char c) { return isalnum(c) || c == '_' || c == '-'; };
    size_t at = 0;
    while ((at = text.find(word, at)) != std::string::npos) {
        const size_t end = at + word.size();
        if ((at > 0 && inWord(text[at - 1])) || (end < text.size() && inWord(text[end]))) {
            at = end;
            continue;
        }
        text.replace(at, word.size(), replacement);
        at += replacement.size();
    }
}

std::string trimmed(const std::string& text)
{
    const size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return {};
    const size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

std::string format(const char* format, ...)
{
    char buffer[2048];
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(buffer, sizeof(buffer), format, arguments);
    va_end(arguments);
    return buffer;
}

std::string readFile(const std::string& path)
{
    FILE* file = fopen(path.c_str(), "re");
    if (!file)
        return {};
    std::string text;
    char buffer[4096];
    size_t count;
    while ((count = fread(buffer, 1, sizeof(buffer), file)) > 0)
        text.append(buffer, count);
    fclose(file);
    return text;
}

std::string anonymous(std::string text)
{
    char host[256] = {};
    if (gethostname(host, sizeof(host) - 1) == 0) {
        const std::string name = host;
        if (name.size() > 2)
            replaceWord(text, name, "<host>");
    }
    const passwd* user = getpwuid(getuid());
    if (!user)
        return text;
    if (user->pw_dir && strlen(user->pw_dir) > 1) {
        const std::string replacement = "~";
        const std::string home = user->pw_dir;
        size_t at = 0;
        while (!home.empty() && (at = text.find(home, at)) != std::string::npos) {
            text.replace(at, home.size(), replacement);
            at += replacement.size();
        }
    }
    const std::string name = user->pw_name;
    if (name.size() > 2)
        replaceWord(text, name, "<user>");
    if (user->pw_gecos) {
        const std::string fullName(user->pw_gecos, strcspn(user->pw_gecos, ","));
        if (fullName.size() > 2)
            replaceWord(text, fullName, "<user>");
    }
    return text;
}

static double monotonic()
{
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec / 1e9;
}

CommandResult run(const std::vector<std::string>& arguments, int seconds)
{
    CommandResult result;
    int ends[2];
    if (pipe2(ends, O_CLOEXEC) != 0)
        return result;
    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);
    posix_spawn_file_actions_addopen(&actions, 0, "/dev/null", O_RDONLY, 0);
    posix_spawn_file_actions_adddup2(&actions, ends[1], 1);
    posix_spawn_file_actions_adddup2(&actions, ends[1], 2);
    posix_spawnattr_t attributes;
    posix_spawnattr_init(&attributes);
    posix_spawnattr_setflags(&attributes, POSIX_SPAWN_SETPGROUP);
    posix_spawnattr_setpgroup(&attributes, 0);
    std::vector<char*> argv;
    for (const std::string& argument : arguments)
        argv.push_back(const_cast<char*>(argument.c_str()));
    argv.push_back(nullptr);
    pid_t child = 0;
    const int error = posix_spawnp(&child, argv[0], &actions, &attributes, argv.data(), environ);
    posix_spawn_file_actions_destroy(&actions);
    posix_spawnattr_destroy(&attributes);
    close(ends[1]);
    if (error != 0) {
        close(ends[0]);
        return result;
    }
    result.started = true;
    const double end = monotonic() + seconds;
    for (;;) {
        const double left = end - monotonic();
        if (left <= 0) {
            result.timedOut = true;
            kill(-child, SIGKILL);
            break;
        }
        pollfd input = {ends[0], POLLIN, 0};
        if (poll(&input, 1, int(left * 1000) + 1) <= 0)
            continue;
        char buffer[8192];
        const ssize_t count = read(ends[0], buffer, sizeof(buffer));
        if (count <= 0)
            break;
        result.output.append(buffer, count);
    }
    close(ends[0]);
    int status = 0;
    while (waitpid(child, &status, 0) < 0 && errno == EINTR) {
    }
    result.status = WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);
    return result;
}

void Report::line(const std::string& line)
{
    text += line + "\n";
}

void Report::section(const std::string& title)
{
    text += "\n== " + title + " ==\n\n";
}

void Report::field(const std::string& name, const std::string& value)
{
    std::string label = name + ":";
    if (label.size() < 28)
        label.resize(28, ' ');
    else
        label += ' ';
    text += label + value + "\n";
}

void Report::result(const std::string& status, const std::string& check, const std::string& message)
{
    const std::string result = status + " " + check + ": " + message;
    line(result);
    if (status == "FAIL")
        failed++;
    else if (status == "WARN")
        warnings++;
    else
        return;
    problems.push_back(result);
}

void Report::result(const std::string& line)
{
    const size_t space = line.find(' ');
    const size_t colon = line.find(": ", space);
    if (space == std::string::npos || colon == std::string::npos) {
        this->line(line);
        return;
    }
    result(line.substr(0, space), line.substr(space + 1, colon - space - 1), line.substr(colon + 2));
}

} // namespace cutefish::reef::diagnose

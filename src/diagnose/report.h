// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The report of reef-diagnose: plain text in sections, with results of checks that are counted and listed at its start.

#pragma once

#include <string>
#include <vector>

namespace cutefish::reef::diagnose {

struct Report
{
    std::string text;
    // The results that failed or warn.
    std::vector<std::string> problems;
    int failed = 0;
    int warnings = 0;

    void line(const std::string& line);
    void section(const std::string& title);
    void field(const std::string& name, const std::string& value);
    // `status` is PASS, FAIL, WARN, INFO or SKIP.
    void result(const std::string& status, const std::string& check, const std::string& message);
    // A result as a probe prints it, in one line.
    void result(const std::string& line);
};

struct CommandResult
{
    int status = -1;
    bool timedOut = false;
    bool started = false;
    std::string output;
};

// Runs a command without input; its output and errors together.
CommandResult run(const std::vector<std::string>& arguments, int seconds);

std::string readFile(const std::string& path);
std::string trimmed(const std::string& text);
std::string format(const char* format, ...) __attribute__((format(printf, 1, 2)));
// The text without the names of the machine and of the user, and with the home folder as ~.
std::string anonymous(std::string text);

} // namespace cutefish::reef::diagnose

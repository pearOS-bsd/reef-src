// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// reef-diagnose: a report on the graphics of this machine, to attach to an issue.

#include "sections.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <pwd.h>
#include <sstream>
#include <unistd.h>

using namespace cutefish::reef::diagnose;

static std::string programFolder()
{
    char path[4096];
    const ssize_t length = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (length <= 0)
        return {};
    path[length] = 0;
    const std::string program = path;
    return program.substr(0, program.rfind('/'));
}

// The probe is a program of the platform that reef runs; it prints its results line by line.
static void addProbe(Report& report)
{
    report.section("reef GPU probe");
    const std::string folder = programFolder();
    const std::string reef = folder + "/reef";
    const std::string probe = folder + "/../lib/cutefish-reef/usr/libexec/reef-gpu-probe";
    if (access(reef.c_str(), X_OK) != 0 || access(probe.c_str(), R_OK) != 0) {
        report.result("FAIL", "probe", "reef's GPU probe is not installed beside " + reef);
        return;
    }
    const CommandResult result = run({reef, probe, "-ReefHeadless", "YES"}, 240);
    std::string test;
    std::istringstream lines(result.output);
    std::string line;
    bool done = false;
    while (std::getline(lines, line)) {
        if (line.starts_with("RUN ")) {
            test = line.substr(4);
        } else if (line.starts_with("DONE ")) {
            done = true;
        } else if (line.starts_with("PASS ") || line.starts_with("FAIL ") || line.starts_with("WARN ") || line.starts_with("INFO ")
                   || line.starts_with("SKIP ")) {
            report.result(line);
            test.clear();
        } else {
            report.line("  | " + line);
        }
    }
    if (result.timedOut)
        report.result("FAIL", "probe", "stopped after 240 seconds" + (test.empty() ? std::string() : " in " + test));
    else if (!done)
        report.result("FAIL", "probe", format("ended with status %d before it finished", result.status)
                      + (test.empty() ? std::string() : " in " + test));
}

int main(int argc, char** argv)
{
    std::string path;
    if (argc >= 2) {
        if (strcmp(argv[1], "--import-check") == 0)
            return importCheck(argc, argv);
        if (argc != 2 || argv[1][0] == '-') {
            fprintf(stderr, "Usage: reef-diagnose [report file]\n"
                            "Writes a report on reef's graphics on this machine, by default to ~/reef-diagnose.txt.\n");
            return 2;
        }
        path = argv[1];
    } else {
        const passwd* user = getpwuid(getuid());
        path = std::string(user && user->pw_dir ? user->pw_dir : ".") + "/reef-diagnose.txt";
    }
    printf("Collecting the report; the GPU checks take up to a few minutes.\n");
    fflush(stdout);
    Report report;
    addSystem(report);
    addKernelLog(report);
    addActivity(report);
    addEgl(report);
    addDriver(report);
    const std::vector<std::string> resets = resetLines();
    ClockSampler sampler;
    sampler.start();
    addProbe(report);
    addGlass(report);
    sampler.stop();
    report.section("GPU clocks during the probe and the glass");
    sampler.add(report);
    addResets(report, resets);
    addEglInfo(report);

    const time_t now = time(nullptr);
    char date[64];
    strftime(date, sizeof(date), "%Y-%m-%d %H:%M %z", localtime(&now));
    std::string text = "reef-diagnose report\n";
    text += std::string("Written ") + date + "\n";
    text += format("Summary: %d failed, %d warnings\n", report.failed, report.warnings);
    for (const std::string& problem : report.problems)
        text += "  " + problem + "\n";
    text += report.text;
    text = anonymous(text);
    FILE* file = fopen(path.c_str(), "we");
    if (!file || fwrite(text.data(), 1, text.size(), file) != text.size() || fclose(file) != 0) {
        fprintf(stderr, "reef-diagnose: cannot write %s: %s\n", path.c_str(), strerror(errno));
        return 1;
    }
    printf("%d failed, %d warnings.\n", report.failed, report.warnings);
    for (const std::string& problem : report.problems)
        printf("  %s\n", anonymous(problem).c_str());
    printf("The report is in %s. It names neither you nor this machine; read it, then attach it to the issue.\n", path.c_str());
    return 0;
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The GPU work that reef's programs logged, and what of it ran at each GPU hang of the kernel log.

#include "sections.h"

#include <cstdlib>
#include <ctime>
#include <pwd.h>
#include <regex>
#include <sstream>
#include <unistd.h>

namespace cutefish::reef::diagnose {

// One line of the activity log.
struct Activity
{
    double time;
    std::string program;
    long process;
    long thread;
    long milliseconds;
    std::string work;
    std::string line;
};

struct Hang
{
    double time;
    std::string program;
    long thread;
    std::string line;
};

// Seconds since 1970 of a time as journalctl (short-iso-precise) and the activity log write it; 0 without one.
static double timeOf(const std::string& text)
{
    static const std::regex pattern("^(\\d{4})-(\\d{2})-(\\d{2})T(\\d{2}):(\\d{2}):(\\d{2})\\.(\\d+)([+-])(\\d{2}):?(\\d{2})");
    std::smatch match;
    if (!std::regex_search(text, match, pattern))
        return 0;
    tm parts = {};
    parts.tm_year = std::stoi(match[1].str()) - 1900;
    parts.tm_mon = std::stoi(match[2].str()) - 1;
    parts.tm_mday = std::stoi(match[3].str());
    parts.tm_hour = std::stoi(match[4].str());
    parts.tm_min = std::stoi(match[5].str());
    parts.tm_sec = std::stoi(match[6].str());
    const std::string fraction = match[7].str();
    const int minutes = (std::stoi(match[9].str()) * 60 + std::stoi(match[10].str())) * (match[8] == "-" ? -1 : 1);
    return timegm(&parts) - minutes * 60 + std::stod("0." + fraction);
}

// The activity log of the user, the older file first.
static std::vector<Activity> loggedActivities()
{
    std::vector<Activity> activities;
    const passwd* user = getpwuid(getuid());
    if (!user || !user->pw_dir)
        return activities;
    static const std::regex pattern("^(\\S+) (.+) pid ([0-9]+) tid ([0-9]+) (SLOW|RUNNING) (.+ for ([0-9]+) ms: .*)$");
    const std::string path = std::string(user->pw_dir) + "/.cache/cutefish-reef/gpu-activity.log";
    for (const std::string& file : {path + ".old", path}) {
        std::istringstream lines(readFile(file));
        std::string line;
        while (std::getline(lines, line)) {
            std::smatch match;
            if (!std::regex_search(line, match, pattern))
                continue;
            activities.push_back({timeOf(match[1].str()), match[2].str(), strtol(match[3].str().c_str(), nullptr, 10),
                                  strtol(match[4].str().c_str(), nullptr, 10), strtol(match[7].str().c_str(), nullptr, 10),
                                  match[6].str(), line});
        }
    }
    return activities;
}

// The hangs of this boot that name a program: i915 and amdgpu.
static std::vector<Hang> loggedHangs()
{
    std::vector<Hang> hangs;
    const CommandResult log = run({"journalctl", "-k", "-b", "-o", "short-iso-precise", "--no-pager", "-q"}, 30);
    if (!log.started || log.status != 0)
        return hangs;
    static const std::regex i915("([^ \\[]+)\\[([0-9]+)\\] context reset due to GPU hang");
    static const std::regex amdgpu("Process information: process (\\S+) pid ([0-9]+)");
    std::istringstream lines(log.output);
    std::string line;
    while (std::getline(lines, line)) {
        std::smatch match;
        if (std::regex_search(line, match, i915) || std::regex_search(line, match, amdgpu))
            hangs.push_back({timeOf(line), match[1].str(), strtol(match[2].str().c_str(), nullptr, 10), line});
    }
    return hangs;
}

void addActivity(Report& report)
{
    report.section("GPU work of reef programs at each GPU hang");
    report.line("reef's programs log GPU work that runs past 100 ms; each hang the kernel logged this boot is set");
    report.line("against what that program had running then. Hangs from before this reef was installed have none.");
    const std::vector<Activity> activities = loggedActivities();
    const std::vector<Hang> hangs = loggedHangs();
    if (hangs.empty())
        report.result("INFO", "hang", "the kernel logged no GPU hang this boot");
    for (const Hang& hang : hangs) {
        report.line("");
        report.line(hang.line);
        std::vector<const Activity*> running;
        for (const Activity& activity : activities) {
            if (activity.thread != hang.thread && activity.program != hang.program)
                continue;
            if (activity.time < hang.time + 0.5 && activity.time + activity.milliseconds / 1000.0 > hang.time - 0.5)
                running.push_back(&activity);
        }
        const std::string process = hang.program + "[" + std::to_string(hang.thread) + "]";
        const Activity* best = nullptr;
        for (const Activity* activity : running) {
            report.line("  " + activity->line);
            const bool named = activity->thread == hang.thread;
            if (!best || (named != (best->thread == hang.thread) ? named : activity->milliseconds > best->milliseconds))
                best = activity;
        }
        if (best) {
            const std::string work = best->work.substr(0, best->work.find(" | also running: "));
            report.result("FAIL", "hang", process + " hung the GPU in " + work
                          + (best->thread == hang.thread ? " (issued by the thread the kernel names)"
                                                         : " (the longest work of the program then)"));
        } else {
            const std::string note = activities.empty() ? " (the activity log is empty)" : "";
            report.result("FAIL", "hang", process + " hung the GPU, and reef logged no long GPU work of it then" + note);
        }
    }
    report.section("GPU activity log, last entries");
    if (activities.empty()) {
        report.line("No reef program logged GPU work that ran long since this reef was installed.");
        return;
    }
    for (size_t i = activities.size() > 200 ? activities.size() - 200 : 0; i < activities.size(); i++)
        report.line(activities[i].line);
}

} // namespace cutefish::reef::diagnose

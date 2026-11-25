// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the report tells of the system: its packages, the GPU devices, the kernel log, and the clocks of the GPU.

#include "sections.h"

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <map>
#include <mutex>
#include <regex>
#include <sstream>
#include <sys/utsname.h>
#include <thread>
#include <unistd.h>

namespace cutefish::reef::diagnose {

static const char* const throttleReasons[] = {"pl1", "pl2", "pl4", "thermal", "prochot", "ratl", "vr_tdc", "vr_thermalert"};

// The names in a folder, sorted, without hidden ones.
static std::vector<std::string> namesIn(const std::string& path)
{
    std::vector<std::string> names;
    DIR* directory = opendir(path.c_str());
    if (!directory)
        return names;
    while (const dirent* entry = readdir(directory)) {
        if (entry->d_name[0] != '.')
            names.push_back(entry->d_name);
    }
    closedir(directory);
    std::sort(names.begin(), names.end());
    return names;
}

// A file that has one number.
static bool readNumber(const std::string& path, long& value)
{
    const std::string text = trimmed(readFile(path));
    if (text.empty() || text[0] < '0' || text[0] > '9')
        return false;
    value = std::stol(text);
    return true;
}

// The value in "key : value" or, for a key that ends in '=', "key=value" lines.
static std::string valueOf(const std::string& text, const std::string& key)
{
    std::istringstream lines(text);
    std::string line;
    while (std::getline(lines, line)) {
        if (!line.starts_with(key))
            continue;
        if (key.ends_with('='))
            return trimmed(line.substr(key.size()));
        const size_t colon = line.find(':');
        return trimmed(colon == std::string::npos ? line.substr(key.size()) : line.substr(colon + 1));
    }
    return {};
}

static std::string osRelease(const std::string& key)
{
    std::istringstream lines(readFile("/etc/os-release"));
    std::string line;
    while (std::getline(lines, line)) {
        if (!line.starts_with(key + "="))
            continue;
        std::string value = line.substr(key.size() + 1);
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
            value = value.substr(1, value.size() - 2);
        return value;
    }
    return {};
}

static std::string linkName(const std::string& path)
{
    char target[PATH_MAX];
    const ssize_t length = readlink(path.c_str(), target, sizeof(target) - 1);
    if (length <= 0)
        return {};
    target[length] = 0;
    const char* slash = strrchr(target, '/');
    return slash ? slash + 1 : target;
}

struct ClockSampler::State
{
    std::thread thread;
    std::atomic<bool> running = false;
    std::mutex mutex;
    std::vector<std::string> limits;
    std::map<std::string, std::vector<long>> values;
    std::map<std::string, long> throttled;
    long samples = 0;
};

// The clock that amdgpu marks as the present one.
static bool readPresentClock(const std::string& path, long& value)
{
    std::istringstream lines(readFile(path));
    std::string line;
    while (std::getline(lines, line)) {
        if (line.find('*') == std::string::npos)
            continue;
        const size_t colon = line.find(':');
        if (colon == std::string::npos)
            return false;
        value = strtol(line.c_str() + colon + 1, nullptr, 10);
        return true;
    }
    return false;
}

ClockSampler::ClockSampler()
    : m_state(std::make_unique<State>())
{
}

ClockSampler::~ClockSampler()
{
    stop();
}

void ClockSampler::start()
{
    std::vector<std::string> cards;
    for (const std::string& name : namesIn("/sys/class/drm")) {
        if (name.starts_with("card") && name.find('-') == std::string::npos)
            cards.push_back(name);
    }
    State* state = m_state.get();
    for (const std::string& card : cards) {
        const std::string path = "/sys/class/drm/" + card;
        long value = 0;
        if (readNumber(path + "/gt_RP0_freq_mhz", value))
            state->limits.push_back(format("%s: highest GPU clock %ld MHz", card.c_str(), value));
        if (readNumber(path + "/gt_max_freq_mhz", value))
            state->limits.push_back(format("%s: clock limit set to %ld MHz", card.c_str(), value));
        if (readNumber(path + "/gt_min_freq_mhz", value))
            state->limits.push_back(format("%s: lowest clock set to %ld MHz", card.c_str(), value));
    }
    state->running = true;
    state->thread = std::thread([state, cards] {
        while (state->running) {
            {
                std::lock_guard lock(state->mutex);
                state->samples++;
                for (const std::string& card : cards) {
                    const std::string path = "/sys/class/drm/" + card;
                    long value = 0;
                    if (readNumber(path + "/gt_act_freq_mhz", value))
                        state->values[card + " clock (MHz)"].push_back(value);
                    if (readNumber(path + "/device/tile0/gt0/freq0/act_freq", value))
                        state->values[card + " clock (MHz)"].push_back(value);
                    if (readNumber(path + "/device/gpu_busy_percent", value))
                        state->values[card + " busy (%)"].push_back(value);
                    if (readPresentClock(path + "/device/pp_dpm_sclk", value))
                        state->values[card + " clock (MHz)"].push_back(value);
                    const std::string gt = path + "/gt/gt0/";
                    if (!readNumber(gt + "throttle_reason_status", value) || value == 0)
                        continue;
                    state->throttled[card + " throttled"]++;
                    for (const char* reason : throttleReasons) {
                        long set = 0;
                        if (readNumber(gt + "throttle_reason_" + reason, set) && set != 0)
                            state->throttled[card + " throttled by " + reason]++;
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
}

void ClockSampler::stop()
{
    if (m_state->running.exchange(false) && m_state->thread.joinable())
        m_state->thread.join();
}

void ClockSampler::add(Report& report)
{
    std::lock_guard lock(m_state->mutex);
    for (const std::string& limit : m_state->limits)
        report.line(limit);
    if (m_state->values.empty()) {
        report.line("The kernel gives no GPU clock or load for this driver.");
        return;
    }
    for (const auto& [name, values] : m_state->values) {
        std::vector<long> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        report.field(name, format("lowest %ld, median %ld, highest %ld over %zu samples", sorted.front(), sorted[sorted.size() / 2],
                                  sorted.back(), sorted.size()));
    }
    for (const auto& [name, count] : m_state->throttled)
        report.field(name, format("%ld of %ld samples", count, m_state->samples));
}

void addSystem(Report& report)
{
    report.section("System");
    report.field("reef build", REEF_VERSION);
    report.field("Distribution", osRelease("PRETTY_NAME"));
    report.field("Debian version", trimmed(readFile("/etc/debian_version")));
    utsname names;
    if (uname(&names) == 0) {
        report.field("Kernel", std::string(names.release) + " " + names.version);
        report.field("Architecture", names.machine);
    }
    const std::string cpu = readFile("/proc/cpuinfo");
    report.field("CPU", valueOf(cpu, "model name"));
    report.field("CPU threads", std::to_string(sysconf(_SC_NPROCESSORS_ONLN)));
    report.field("Memory", valueOf(readFile("/proc/meminfo"), "MemTotal"));
    std::istringstream arguments(readFile("/proc/cmdline"));
    std::string argument, options;
    while (arguments >> argument) {
        const std::string_view view = argument;
        if (view.starts_with("i915.") || view.starts_with("xe.") || view.starts_with("amdgpu.") || view.starts_with("radeon.")
            || view.starts_with("nouveau.") || view.starts_with("nvidia") || view.starts_with("drm.") || view.starts_with("nomodeset"))
            options += (options.empty() ? "" : " ") + argument;
    }
    report.field("Graphics kernel options", options.empty() ? "none" : options);

    report.section("Packages");
    static const char* const packages[] = {
        "cutefish-reef", "cutefish-compositor", "cutefish-toolkit", "cutefish-webkit", "libgl1-mesa-dri", "mesa-libgallium",
        "libegl-mesa0", "libegl1", "libgbm1", "libdrm2", "libdrm-intel1", "libdrm-amdgpu1", "mesa-va-drivers",
        "intel-media-va-driver", "intel-media-va-driver-non-free", "firmware-intel-graphics", "firmware-amd-graphics",
        "firmware-misc-nonfree", "firmware-linux-nonfree",
    };
    std::vector<std::string> query = {"dpkg-query", "-W", "-f", "${Package} ${Version} ${db:Status-Abbrev}\\n"};
    for (const char* package : packages)
        query.push_back(package);
    const CommandResult installed = run(query, 20);
    if (installed.started) {
        std::istringstream lines(installed.output);
        std::string line;
        while (std::getline(lines, line)) {
            std::istringstream words(line);
            std::string name, version, status;
            words >> name >> version >> status;
            if (name.empty() || name.starts_with("dpkg-query"))
                continue;
            report.field(name, status.starts_with("ii") ? version : "not installed");
        }
    } else {
        report.line("dpkg-query is not available");
    }

    report.section("GPU devices");
    for (const std::string& name : namesIn("/sys/class/drm")) {
        const std::string path = "/sys/class/drm/" + name;
        if (name.find('-') != std::string::npos) {
            if (trimmed(readFile(path + "/status")) != "connected")
                continue;
            std::istringstream lines(readFile(path + "/modes"));
            std::string first, mode;
            std::getline(lines, first);
            std::string modes = first;
            for (int i = 1; std::getline(lines, mode) && i < 6; i++)
                modes += ", " + mode;
            report.field(name, "connected, modes " + modes);
        } else if (name.starts_with("card") || name.starts_with("renderD")) {
            const std::string uevent = readFile(path + "/device/uevent");
            const std::string driver = linkName(path + "/device/driver");
            const char* boot = trimmed(readFile(path + "/device/boot_vga")) == "1" ? ", boot display" : "";
            report.field(name, driver + ", PCI " + valueOf(uevent, "PCI_ID=") + " subsystem " + valueOf(uevent, "PCI_SUBSYS_ID=")
                         + " at " + valueOf(uevent, "PCI_SLOT_NAME=") + boot);
        }
    }
    for (const char* module : {"i915", "xe"}) {
        std::string parameters;
        for (const char* parameter : {"enable_guc", "enable_psr", "enable_fbc", "enable_dc", "force_probe", "mitigations"}) {
            const std::string value = trimmed(readFile(std::string("/sys/module/") + module + "/parameters/" + parameter));
            if (!value.empty())
                parameters += (parameters.empty() ? "" : ", ") + std::string(parameter) + "=" + value;
        }
        if (!parameters.empty())
            report.field(std::string(module) + " parameters", parameters);
    }
}

static CommandResult kernelLog()
{
    return run({"journalctl", "-k", "-b", "-o", "short-monotonic", "--no-pager", "-q"}, 30);
}

void addKernelLog(Report& report)
{
    report.section("Kernel log, GPU lines of this boot");
    const CommandResult log = kernelLog();
    if (!log.started || log.status != 0 || log.output.empty()) {
        report.line("The kernel log cannot be read by this account (it needs the adm or systemd-journal group).");
        return;
    }
    static const std::regex pattern("i915|\\bxe\\b|\\bdrm\\b|\\[drm|amdgpu|radeon|nouveau|nvidia|gpu hang|\\bguc\\b|\\bhuc\\b|\\bdmc\\b|"
                                    "gfx|render ring|rcs0|bcs0|vcs0|vecs0|ccs0|dma-buf|dmabuf|gem\\b|reset", std::regex::icase);
    std::vector<std::string> found;
    std::istringstream lines(log.output);
    std::string line;
    while (std::getline(lines, line)) {
        if (std::regex_search(line, pattern))
            found.push_back(line);
    }
    if (found.size() > 300) {
        for (size_t i = 0; i < 80; i++)
            report.line(found[i]);
        report.line(format("... %zu lines left out ...", found.size() - 300));
        for (size_t i = found.size() - 220; i < found.size(); i++)
            report.line(found[i]);
    } else {
        for (const std::string& line : found)
            report.line(line);
    }
    if (found.empty())
        report.line("No GPU lines.");
}

std::vector<std::string> resetLines()
{
    std::vector<std::string> found;
    const CommandResult log = kernelLog();
    if (!log.started || log.status != 0)
        return found;
    static const std::regex pattern("GPU HANG|context reset|Resetting [a-z]+[0-9]* for|gpu reset|ring timeout", std::regex::icase);
    std::istringstream lines(log.output);
    std::string line;
    while (std::getline(lines, line)) {
        if (std::regex_search(line, pattern))
            found.push_back(line);
    }
    return found;
}

void addResets(Report& report, const std::vector<std::string>& before)
{
    report.section("GPU resets while the report ran");
    std::vector<std::string> added;
    for (const std::string& line : resetLines()) {
        if (std::find(before.begin(), before.end(), line) == before.end())
            added.push_back(line);
    }
    if (added.empty()) {
        report.result("PASS", "resets", "the kernel logged no GPU hang or reset while the checks ran");
        return;
    }
    report.result("FAIL", "resets", format("the kernel logged %zu GPU hang or reset lines while the checks ran", added.size()));
    for (const std::string& line : added)
        report.line(line);
}

void addEglInfo(Report& report)
{
    report.section("eglinfo");
    const CommandResult info = run({"eglinfo"}, 60);
    if (!info.started)
        report.line("eglinfo is not installed (package mesa-utils); the EGL section above has what reef uses.");
    else
        report.line(info.output);
}

} // namespace cutefish::reef::diagnose

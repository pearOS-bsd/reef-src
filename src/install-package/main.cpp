// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Installs the applications of an installer package.

#include "package.h"

#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <memory>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <thread>
#include <unistd.h>

#include "base/dbus.h"
#include "base/event_loop.h"
#include "ui/application.h"
#include "ui/cell_layout.h"
#include "ui/color_id.h"
#include "ui/color_mixer.h"
#include "ui/dialog_window.h"
#include "ui/icon.h"
#include "ui/keys.h"
#include "ui/label.h"
#include "ui/progress_bar.h"
#include "ui/window.h"

using namespace cutefish;
using namespace cutefish::reef;

static constexpr char ApplicationsFolder[] = "/Applications";

// What was installed; the error is empty when the user cancelled, too.
struct Outcome
{
    std::vector<std::string> applications;
    std::string error;
};

// Calls the service that writes to the Applications folder. It asks for a password, which may take its time.
static bool call(const std::string& method, const std::function<void(base::dbus::Message&)>& fill, std::string* answer, std::string& error)
{
    base::EventLoop loop;
    const auto bus = base::dbus::Connection::open(base::dbus::Connection::Bus::System, &loop);
    if (!bus) {
        error = "The system bus cannot be reached.";
        return false;
    }
    base::dbus::Message message = bus->newMethodCall("org.cutefish.ApplicationsFolder", "/org/cutefish/ApplicationsFolder",
        "org.cutefish.ApplicationsFolder", method);
    fill(message);
    const base::dbus::Reply reply = bus->call(message, std::chrono::minutes(10));
    if (reply.isError()) {
        error = reply.error().name == "org.freedesktop.DBus.Error.AccessDenied" ? std::string() : reply.error().message;
        return false;
    }
    std::string text;
    if (answer && (!reply.read(text) || text.empty())) {
        error = "The Applications folder did not answer.";
        return false;
    }
    if (answer)
        *answer = text;
    return true;
}

static Outcome installThroughHelper(const package::Package& package, const std::function<void(uint64_t)>& progress)
{
    Outcome outcome;
    std::string folder;
    if (!call("prepare", [](base::dbus::Message&) {}, &folder, outcome.error))
        return outcome;
    const auto discard = [](const std::string& stage) {
        std::string ignored;
        call("discard", [&](base::dbus::Message& message) { message.append(stage); }, nullptr, ignored);
    };
    const package::Contents contents = package::extract(package, folder, progress);
    if (!contents.error.empty()) {
        discard(folder);
        outcome.error = contents.error;
        return outcome;
    }
    for (size_t i = 0; i < contents.applications.size(); i++) {
        const std::string& application = contents.applications[i];
        std::string stage = folder;
        // Each stage holds one application.
        if (i + 1 < contents.applications.size()) {
            if (!call("prepare", [](base::dbus::Message&) {}, &stage, outcome.error))
                break;
            if (rename((folder + "/" + application).c_str(), (stage + "/" + application).c_str()) != 0) {
                outcome.error = "Cannot stage " + application + ": " + strerror(errno);
                discard(stage);
                break;
            }
        }
        const bool replace = access((std::string(ApplicationsFolder) + "/" + application).c_str(), F_OK) == 0;
        std::string ignored;
        if (!call("publish", [&](base::dbus::Message& message) { message.append(stage, application, replace); }, &ignored, outcome.error))
            break;
        outcome.applications.push_back(application);
    }
    if (outcome.applications.size() < contents.applications.size())
        discard(folder);
    return outcome;
}

static void removeAll(const std::string& path)
{
    std::error_code error;
    std::filesystem::remove_all(path, error);
}

static void shareWithGroup(const std::filesystem::path& path)
{
    struct stat status;
    if (lstat(path.c_str(), &status) == 0 && !S_ISLNK(status.st_mode))
        chmod(path.c_str(), (status.st_mode & 0777) | (S_ISDIR(status.st_mode) ? 0775 : 0664));
}

static Outcome installInto(const package::Package& package, const std::string& folder, const std::function<void(uint64_t)>& progress)
{
    Outcome outcome;
    std::string pattern = folder + "/.reef-install-XXXXXX";
    if (!mkdtemp(pattern.data())) {
        outcome.error = "Cannot write to " + folder + ": " + strerror(errno);
        return outcome;
    }
    const std::string stage = pattern;
    const package::Contents contents = package::extract(package, stage, progress);
    if (!contents.error.empty()) {
        removeAll(stage);
        outcome.error = contents.error;
        return outcome;
    }
    for (const std::string& application : contents.applications) {
        const std::string from = stage + "/" + application;
        const std::string to = folder + "/" + application;
        struct stat status;
        // A folder shared by a group keeps its applications writable for the group.
        if (stat(folder.c_str(), &status) == 0 && (status.st_mode & (S_ISGID | S_IWGRP)) == (S_ISGID | S_IWGRP)) {
            shareWithGroup(from);
            std::error_code error;
            for (std::filesystem::recursive_directory_iterator i(from, error); !error && i != std::filesystem::recursive_directory_iterator(); i.increment(error))
                shareWithGroup(i->path());
        }
        if (syscall(SYS_renameat2, AT_FDCWD, from.c_str(), AT_FDCWD, to.c_str(), RENAME_NOREPLACE) != 0
            && (errno != EEXIST || syscall(SYS_renameat2, AT_FDCWD, from.c_str(), AT_FDCWD, to.c_str(), RENAME_EXCHANGE) != 0)) {
            outcome.error = "Cannot put " + application + " in place: " + strerror(errno);
            break;
        }
        outcome.applications.push_back(application);
    }
    removeAll(stage);
    return outcome;
}

static Outcome install(const package::Package& package, const std::string& folder, const std::function<void(uint64_t)>& progress)
{
    if (folder == ApplicationsFolder && access(ApplicationsFolder, W_OK) != 0 && errno == EACCES)
        return installThroughHelper(package, progress);
    return installInto(package, folder, progress);
}

// The applications' names, as a list in a sentence.
static std::string names(const std::vector<std::string>& applications)
{
    std::string text;
    for (size_t i = 0; i < applications.size(); i++) {
        text += "“" + applications[i].substr(0, applications[i].size() - 4) + "”";
        if (i + 2 < applications.size())
            text += ", ";
        else if (i + 2 == applications.size())
            text += " and ";
    }
    return text;
}

static int runCommand(int argc, char** argv)
{
    const std::string command = argv[1];
    const auto package = package::load(argv[2]);
    if (!package) {
        fprintf(stderr, "%s: not an installer package\n", argv[2]);
        return 1;
    }
    if (command == "--list") {
        const package::Contents contents = package::contents(*package);
        printf("title: %s\nversion: %s\n", package->title.c_str(), package->version.c_str());
        for (const std::string& application : contents.applications)
            printf("application: %s\n", application.c_str());
        printf("skipped: %llu\nscripts: %s\n", static_cast<unsigned long long>(contents.skipped), contents.hasScripts ? "yes" : "no");
        if (!contents.error.empty())
            fprintf(stderr, "%s\n", contents.error.c_str());
        return contents.error.empty() ? 0 : 1;
    }
    if (command == "--install" && argc > 4 && std::string(argv[3]) == "--into") {
        const Outcome outcome = install(*package, argv[4], {});
        for (const std::string& application : outcome.applications)
            printf("installed: %s\n", application.c_str());
        if (!outcome.error.empty())
            fprintf(stderr, "%s\n", outcome.error.c_str());
        return outcome.error.empty() ? 0 : 1;
    }
    fprintf(stderr, "Usage: reef-install-package [--list | --install <package> --into <folder>] <package>\n");
    return 2;
}

int main(int argc, char** argv)
{
    if (argc > 2 && strncmp(argv[1], "--", 2) == 0)
        return runCommand(argc, argv);
    if (argc != 2) {
        fprintf(stderr, "Usage: reef-install-package <package>\n");
        return 2;
    }
    ui::Application app(argc, argv);
    app.setApplicationId("cutefish-reef");
    const auto package = package::load(argv[1]);
    const package::Contents contents = package ? package::contents(*package) : package::Contents();
    const std::string name = package ? package->title : std::filesystem::path(argv[1]).filename().string();
    const bool installable = package && contents.error.empty() && !contents.applications.empty();

    std::string title;
    std::string text;
    std::vector<std::string> notes;
    if (!installable) {
        title = "“" + name + "” can’t be installed";
        text = !package ? "It is not an installer package."
            : contents.error.empty() ? "It puts no application into the Applications folder." : contents.error;
    } else {
        title = "Install “" + name + "”?";
        text = names(contents.applications) + (contents.applications.size() == 1 ? " is" : " are") + " put into the Applications folder.";
        if (!package->version.empty())
            notes.push_back("Version " + package->version);
        if (contents.hasScripts)
            notes.push_back("Its installation scripts are not run.");
        if (contents.skipped)
            notes.push_back("Files it would put elsewhere are left out.");
    }

    ui::Window window;
    window.setTitle(title);
    window.setSkipTaskbar(true);
    window.setHintedTextLayout(false);
    window.setBackgroundColor(ui::ColorId::SecondBackground);

    auto content = std::make_unique<ui::ColumnLayout>();
    content->setPadding(ui::Margins::all(16));
    content->setSpacing(16);
    auto* row = content->addChild<ui::RowLayout>();
    row->setSpacing(14);
    row->setLayoutHints({.fillWidth = true});
    auto* image = row->addChild<ui::ThemeIcon>("package-x-generic", 48);
    image->setLayoutHints({.preferredWidth = 48, .preferredHeight = 48, .verticalAlignment = ui::Alignment::Start});
    auto* texts = row->addChild<ui::ColumnLayout>();
    texts->setSpacing(4);
    texts->setLayoutHints({.fillWidth = true, .verticalAlignment = ui::Alignment::Start});
    auto* titleLabel = texts->addChild<ui::Label>(title);
    titleLabel->setLayoutHints({.fillWidth = true});
    titleLabel->setWrap(true);
    titleLabel->setFontSize(14);
    titleLabel->setWeight(600);
    std::vector<ui::Label*> labels;
    std::vector<std::string> lines = {text};
    lines.insert(lines.end(), notes.begin(), notes.end());
    for (const std::string& line : lines) {
        auto* label = texts->addChild<ui::Label>(line);
        label->setLayoutHints({.fillWidth = true});
        label->setWrap(true);
        label->setFontSize(12);
        label->setColor(ui::ColorId::DisabledText);
        labels.push_back(label);
    }
    auto* bar = texts->addChild<ui::ProgressBar>();
    bar->setLayoutHints({.fillWidth = true});
    bar->setVisible(false);

    auto* buttons = content->addChild<ui::RowLayout>();
    buttons->setSpacing(8);
    buttons->setLayoutHints({.fillWidth = true});
    buttons->addChild<ui::View>()->setLayoutHints({.fillWidth = true});
    auto* cancelButton = installable ? buttons->addChild<ui::DialogButton>("Cancel") : nullptr;
    auto* installButton = buttons->addChild<ui::DialogButton>(installable ? "Install" : "Close", true);

    uint64_t total = 0;
    if (installable) {
        for (const package::Component& component : package->components) {
            if (component.hasPayload)
                total += component.payload.length;
        }
    }
    std::atomic<bool> busy = false;
    std::thread worker;
    const base::LoopHandle loop = base::EventLoop::current()->handle();
    const auto quit = [&app] { app.quit(); };
    const auto finish = [&](const Outcome& outcome) {
        busy = false;
        bar->setVisible(false);
        for (size_t i = 1; i < labels.size(); i++)
            labels[i]->setVisible(false);
        if (outcome.error.empty() && !outcome.applications.empty()) {
            titleLabel->setText("The installation was successful.");
            labels.front()->setText(names(outcome.applications) + (outcome.applications.size() == 1 ? " is" : " are") + " in the Applications folder.");
        } else if (outcome.error.empty()) {
            titleLabel->setText("The installation was cancelled.");
            labels.front()->setText("Nothing was installed.");
        } else {
            titleLabel->setText("The installation failed.");
            labels.front()->setText(outcome.error);
        }
        if (cancelButton)
            cancelButton->setVisible(false);
        installButton->setText("Close");
        installButton->setEnabled(true);
        installButton->onClicked = quit;
    };
    const auto start = [&] {
        if (busy)
            return;
        busy = true;
        if (cancelButton)
            cancelButton->setEnabled(false);
        installButton->setEnabled(false);
        bar->setVisible(true);
        bar->setIndeterminate(total == 0);
        const package::Package copy = *package;
        worker = std::thread([copy, total, loop, &bar, &finish] {
            // The payloads are compressed: about a third of what they hold.
            auto progress = [total, loop, &bar, shown = 0.0](uint64_t written) mutable {
                const double value = total > 0 ? std::min(1.0, double(written) / double(total * 3)) : 0.0;
                if (value - shown < 0.005)
                    return;
                shown = value;
                loop.post([&bar, value] { bar->setValue(value); });
            };
            const Outcome outcome = install(copy, ApplicationsFolder, progress);
            loop.post([&finish, outcome] { finish(outcome); });
        });
    };
    if (cancelButton)
        cancelButton->onClicked = quit;
    if (installable)
        installButton->onClicked = start;
    else
        installButton->onClicked = quit;
    window.addShortcut(0, ui::key::Return, [&] {
        if (installButton->isEnabled() && installButton->onClicked)
            installButton->onClicked();
    });
    window.onCloseRequested = [&] {
        if (!busy)
            quit();
    };
    window.addShortcut(0, ui::key::Escape, [&] {
        if (!busy)
            quit();
    });

    window.setContentView(std::move(content));
    window.resize({420, 0});
    window.setFitsContentHeight(true);
    window.show();
    window.activate();
    installButton->setFocus(ui::FocusReason::Other);
    const int result = app.run();
    if (worker.joinable())
        worker.join();
    return result;
}

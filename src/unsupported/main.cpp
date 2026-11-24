// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Says why a program cannot run, or stopped: what it needs, with a report to copy.

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <sys/utsname.h>
#include <unistd.h>

#include "base/application_bundle.h"
#include "base/file.h"
#include "base/process.h"
#include "base/property_list.h"
#include "ui/application.h"
#include "ui/cell_layout.h"
#include "ui/clipboard.h"
#include "ui/color_id.h"
#include "ui/color_mixer.h"
#include "ui/dialog_window.h"
#include "ui/icon.h"
#include "ui/keys.h"
#include "ui/label.h"
#include "ui/painter.h"
#include "ui/scroll_view.h"
#include "ui/text_area.h"
#include "ui/theme.h"
#include "ui/window.h"

// The build system's.
#define REEF_VERSION "0.1"
#define REEF_MACOS_VERSION "27.0"
#define REEF_MACOS_BUILD "26A100"
#define REEF_LOADER_FROM_LIBEXEC "../../bin/reef"

using namespace cutefish;

struct Program
{
    std::string path;
    std::string bundle;
    std::string name;
    std::string icon;
    std::string identifier;
    std::string version;
    std::string build;
};

namespace {

Program describe(const std::string& path)
{
    Program program;
    program.path = path;
    program.bundle = base::bundle::bundleOfProgram(path);
    program.name = std::filesystem::path(path).filename().string();
    program.icon = "application-x-executable";
    if (program.bundle.empty())
        return program;
    if (const auto entry = base::bundle::read(program.bundle, base::DesktopEntry{})) {
        program.name = entry->name;
        if (!entry->icon.empty())
            program.icon = entry->icon;
    }
    if (const auto info = base::file::read(program.bundle + "/Contents/Info.plist")) {
        if (const auto strings = base::plist::rootStrings(*info)) {
            const auto value = [&strings](const char* key) {
                const auto found = strings->find(key);
                return found == strings->end() ? std::string() : found->second;
            };
            program.identifier = value("CFBundleIdentifier");
            program.version = value("CFBundleShortVersionString");
            program.build = value("CFBundleVersion");
        }
    }
    return program;
}

std::string reportText(const Program& program, const std::string& reason, bool running)
{
    std::string version = program.version;
    if (!program.build.empty() && program.build != program.version)
        version += (version.empty() ? "" : " ") + ("(" + program.build + ")");
    utsname system = {};
    uname(&system);

    std::string text = "Application: " + program.name + (version.empty() ? "" : " " + version) + "\n";
    if (!program.identifier.empty())
        text += "Identifier: " + program.identifier + "\n";
    text += "Path: " + (program.bundle.empty() ? program.path : program.bundle) + "\n";
    text += std::string("Reef: ") + REEF_VERSION + ", as macOS " + REEF_MACOS_VERSION + " (" + REEF_MACOS_BUILD + ")\n";
    text += std::string("Processor: ") + system.machine + "\n";
    text += std::string("Stopped: ") + (running ? "while running" : "before starting") + "\n\n";
    return text + reason + "\n";
}

// What the loader's reason comes to, in a sentence.
std::string explanation(const std::string& reason, bool running)
{
    std::string text;
    if (reason.find("has no slice for this processor") == std::string::npos
        && reason.find("is built for another processor") == std::string::npos) {
        text = running ? "It used a part of macOS that Cutefish does not support yet." : "It uses parts of macOS that Cutefish does not support yet.";
    } else {
        text = "It is not built for this computer’s processor.";
        const std::string has = ": it has only ";
        const std::string is = ", this computer is ";
        const size_t isAt = reason.rfind(is);
        const size_t hasAt = reason.find(has);
        if (isAt != std::string::npos && hasAt < isAt) {
            text = "It only runs on " + reason.substr(hasAt + has.size(), isAt - hasAt - has.size())
                + " processors, but this computer has an " + reason.substr(isAt + is.size()) + " processor.";
        }
    }
    return text;
}

std::string loaderPath()
{
    char buffer[4096];
    const ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (length <= 0)
        return {};
    std::string path(buffer, size_t(length));
    path.erase(path.rfind('/'));
    return path + "/" + REEF_LOADER_FROM_LIBEXEC;
}

int usage()
{
    fprintf(stderr, "Usage: reef-unsupported --launch|--running <program> <reason>\n");
    return 2;
}

} // namespace

// The window moves by its content.
class Content : public ui::ColumnLayout
{
public:
    bool acceptsPointer() const override { return true; }

protected:
    void pointerPress(const ui::PointerEvent& event) override
    {
        m_pressed = event.button == ui::MouseButton::Left;
        m_position = event.position;
    }

    void pointerMove(const ui::PointerEvent& event) override
    {
        if (!m_pressed || !window())
            return;
        if (std::abs(event.position.x - m_position.x) > 10 || std::abs(event.position.y - m_position.y) > 10) {
            m_pressed = false;
            window()->startMove();
        }
    }

    void pointerRelease(const ui::PointerEvent&) override { m_pressed = false; }

private:
    bool m_pressed = false;
    ui::PointF m_position;
};

// The report's card.
class ReportFrame : public ui::View
{
public:
    explicit ReportFrame(std::unique_ptr<ui::View> child)
        : m_child(child.get())
    {
        addChild(std::move(child));
    }

protected:
    void layout() override { m_child->setGeometry(localRect()); }

    void paint(ui::Painter& painter) override
    {
        const ui::Theme& theme = ui::Theme::current();
        painter.drawSmoothRectangle(localRect(), theme.smallRadius, theme.alternateBackground);
    }

private:
    ui::View* m_child;
};

int main(int argc, char** argv)
{
    if (argc != 4)
        return usage();
    const bool launch = strcmp(argv[1], "--launch") == 0;
    const bool running = strcmp(argv[1], "--running") == 0;
    if (!launch && !running)
        return usage();
    const Program program = describe(argv[2]);
    const std::string reason = argv[3];
    const std::string report = reportText(program, reason, running);

    ui::Application app(argc, argv);
    app.setApplicationId("cutefish-reef");
    const std::string title = running ? "“" + program.name + "” quit" : "“" + program.name + "” can’t be opened";
    std::string text = explanation(reason, running);

    ui::Window window;
    window.setTitle(title);
    window.setSkipTaskbar(true);
    window.setAlwaysOnTop(true);
    window.setHintedTextLayout(false);
    window.setBackgroundColor(ui::ColorId::SecondBackground);
    window.onCloseRequested = [&app] { app.quit(); };
    window.addShortcut(0, ui::key::Escape, [&app] { app.quit(); });
    window.addShortcut(0, ui::key::Return, [&app] { app.quit(); });

    auto content = std::make_unique<Content>();
    content->setPadding(ui::Margins::all(16));
    content->setSpacing(12);
    auto* row = content->addChild<ui::RowLayout>();
    row->setSpacing(14);
    row->setLayoutHints({.fillWidth = true});
    auto* image = row->addChild<ui::ThemeIcon>(program.icon, 48);
    image->setLayoutHints({.preferredWidth = 48, .preferredHeight = 48, .verticalAlignment = ui::Alignment::Start});
    auto* texts = row->addChild<ui::ColumnLayout>();
    texts->setSpacing(4);
    texts->setLayoutHints({.fillWidth = true, .verticalAlignment = ui::Alignment::Start});
    auto* titleLabel = texts->addChild<ui::Label>(title);
    titleLabel->setLayoutHints({.fillWidth = true});
    titleLabel->setWrap(true);
    titleLabel->setFontSize(14);
    titleLabel->setWeight(600);
    auto* textLabel = texts->addChild<ui::Label>(text);
    textLabel->setLayoutHints({.fillWidth = true});
    textLabel->setWrap(true);
    textLabel->setFontSize(12);
    textLabel->setColor(ui::ColorId::DisabledText);

    auto scroll = std::make_unique<ui::ScrollView>();
    scroll->setScrollDirections(true, true);
    scroll->setCornerRadius(ui::Theme::current().smallRadius);
    auto* area = scroll->setContent<ui::TextArea>();
    area->setReadOnly(true);
    area->setWrapMode(ui::TextArea::WrapMode::NoWrap);
    area->setFrameVisible(false);
    area->setPadding(ui::Margins::all(8));
    area->setFont(ui::Theme::current().fixedFontFamily, 11, 400);
    area->setLayoutHints({.fillWidth = true});
    area->setText(report);

    auto shared = std::make_shared<std::string>(report);
    base::Process process;
    const std::string loader = loaderPath();
    const std::string target = program.bundle.empty() ? program.path : program.bundle;
    process.onFinished = [&process, area, shared](int exitCode, bool crashed) {
        if (exitCode == 0 && !crashed && !process.standardOutput().empty())
            *shared += "\nEverything missing:\n" + process.standardOutput();
        area->setText(*shared);
    };
    if (!loader.empty() && access(loader.c_str(), X_OK) == 0 && process.start(loader, {"--list-missing", target}))
        area->setText(report + "\nListing everything missing…\n");

    content->addChild<ReportFrame>(std::move(scroll))->setLayoutHints({.fillWidth = true, .preferredHeight = 220});
    auto* buttons = content->addChild<ui::RowLayout>();
    buttons->setSpacing(8);
    buttons->setLayoutHints({.fillWidth = true});
    buttons->addChild<ui::View>()->setLayoutHints({.fillWidth = true});
    auto* copyButton = buttons->addChild<ui::DialogButton>("Copy Report");
    copyButton->onClicked = [copyButton, shared] {
        ui::Clipboard::setText(*shared);
        copyButton->setText("Copied");
    };
    auto* okButton = buttons->addChild<ui::DialogButton>("OK", true);
    okButton->onClicked = [&app] { app.quit(); };

    window.setContentView(std::move(content));
    window.resize({560, 0});
    window.setFitsContentHeight(true);
    window.show();
    window.activate();
    okButton->setFocus(ui::FocusReason::Other);
    return app.run();
}

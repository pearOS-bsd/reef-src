// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The file chooser of the desktop, for open and save panels.

#include "dialogs.h"

#include "base/event_loop.h"
#include "ui/file_chooser.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct reef_ui_file_chooser
{
    std::unique_ptr<cutefish::ui::FileChooser> dialog;
    std::shared_ptr<bool> alive = std::make_shared<bool>(true);
};

namespace cutefish::reef {

using Answer = void (*)(void* context, const char* const* paths, unsigned count, const char* const* options);
// An id and the option picked.
using Options = std::vector<std::pair<std::string, std::string>>;

// What the next dialog answers, in tests.
static std::vector<std::string> testPaths;
static Options testOptions;
static bool hasTestAnswer = false;

static void answer(Answer callback, void* context, const std::vector<std::string>& paths, const std::vector<std::string>& ids,
                   const Options& chosen)
{
    std::vector<const char*> pathPointers;
    for (const std::string& path : paths)
        pathPointers.push_back(path.c_str());
    std::vector<const char*> optionPointers;
    for (const std::string& id : ids) {
        const auto it = std::find_if(chosen.begin(), chosen.end(), [&](const auto& option) { return option.first == id; });
        optionPointers.push_back(it == chosen.end() ? nullptr : it->second.c_str());
    }
    callback(context, pathPointers.data(), unsigned(pathPointers.size()), optionPointers.data());
}

// 100
void hostFileChooserDestroy(reef_ui_file_chooser* chooser)
{
    if (!chooser)
        return;
    *chooser->alive = false;
    delete chooser;
}

static std::vector<std::string> split(const char* text)
{
    std::vector<std::string> parts;
    if (!text)
        return parts;
    std::string part;
    for (;; text++) {
        if (*text == ';' || !*text) {
            if (!part.empty())
                parts.push_back(part);
            part.clear();
            if (!*text)
                break;
        } else {
            part += *text;
        }
    }
    return parts;
}

static ui::FileChooser::Mode modeOf(int mode)
{
    switch (mode) {
    case 1:
        return ui::FileChooser::Mode::OpenFiles;
    case 2:
        return ui::FileChooser::Mode::OpenFolder;
    case 3:
        return ui::FileChooser::Mode::SaveFile;
    default:
        return ui::FileChooser::Mode::OpenFile;
    }
}

// 101
void hostTestFileChooserAnswer(const char* const* paths, unsigned count, const char* const* options, unsigned optionCount)
{
    hasTestAnswer = true;
    testPaths.clear();
    for (unsigned i = 0; paths && i < count; i++)
        testPaths.push_back(paths[i]);
    testOptions.clear();
    for (unsigned i = 0; options && i < optionCount; i++) {
        const std::string option = options[i] ? options[i] : "";
        const size_t equals = option.find('=');
        if (equals != std::string::npos)
            testOptions.emplace_back(option.substr(0, equals), option.substr(equals + 1));
    }
}

// 99
reef_ui_file_chooser* hostFileChooserOpen(reef_ui_window* window, const reef_ui_file_chooser_request* request, Answer callback, void* context)
{
    const int mode = request->mode;
    const char* title = request->title;
    const char* acceptLabel = request->acceptLabel;
    const char* folder = request->folder;
    const char* file = request->file;
    const reef_ui_file_chooser_filter* filters = request->filters;
    const unsigned filterCount = request->filterCount;
    const reef_ui_file_chooser_choice* choices = request->choices;
    const unsigned choiceCount = request->choiceCount;
    const int modal = request->modal;
    auto chooser = new reef_ui_file_chooser;
    std::vector<ui::FileChooser::Choice> choiceList;
    std::vector<std::string> ids;
    Options selected;
    for (unsigned i = 0; choices && i < choiceCount; i++) {
        ui::FileChooser::Choice choice;
        choice.id = choices[i].id ? choices[i].id : "";
        choice.label = choices[i].label ? choices[i].label : "";
        for (unsigned j = 0; j < choices[i].optionCount; j++) {
            choice.options.emplace_back(choices[i].optionIds[j] ? choices[i].optionIds[j] : "",
                                        choices[i].optionLabels[j] ? choices[i].optionLabels[j] : "");
        }
        choice.selected = choices[i].selected ? choices[i].selected : "";
        ids.push_back(choice.id);
        selected.emplace_back(choice.id, choice.selected);
        choiceList.push_back(std::move(choice));
    }
    if (headless) {
        const std::vector<std::string> paths = hasTestAnswer ? testPaths : std::vector<std::string>();
        Options chosen = selected;
        for (const auto& [id, option] : hasTestAnswer ? testOptions : Options()) {
            for (auto& [otherId, value] : chosen) {
                if (otherId == id)
                    value = option;
            }
        }
        hasTestAnswer = false;
        base::EventLoop::current()->post([callback, context, paths, ids, chosen, alive = chooser->alive] {
            if (*alive)
                answer(callback, context, paths, ids, paths.empty() ? Options() : chosen);
        });
        return chooser;
    }
    chooser->dialog = std::make_unique<ui::FileChooser>(window ? window->window : nullptr);
    ui::FileChooser* dialog = chooser->dialog.get();
    dialog->setMode(modeOf(mode));
    if (title && *title)
        dialog->setTitle(title);
    if (acceptLabel && *acceptLabel)
        dialog->setAcceptLabel(acceptLabel);
    if (folder && *folder)
        dialog->setCurrentFolder(folder);
    if (file && *file)
        dialog->setCurrentFile(file);
    std::vector<ui::FileChooser::Filter> filterList;
    for (unsigned i = 0; i < filterCount; i++) {
        ui::FileChooser::Filter filter;
        filter.name = filters[i].name ? filters[i].name : "";
        filter.patterns = split(filters[i].patterns);
        filter.mimeTypes = split(filters[i].mimeTypes);
        filterList.push_back(std::move(filter));
    }
    if (!filterList.empty())
        dialog->setFilters(std::move(filterList));
    if (!choiceList.empty())
        dialog->setChoices(std::move(choiceList));
    dialog->setModal(modal);
    dialog->onAccepted = [callback, context, ids, dialog, alive = chooser->alive](const std::vector<std::string>& paths, const std::string&) {
        if (*alive)
            answer(callback, context, paths, ids, dialog->chosenOptions());
    };
    dialog->onRejected = [callback, context, ids, alive = chooser->alive] {
        if (*alive)
            answer(callback, context, {}, ids, {});
    };
    dialog->open();
    return chooser;
}

} // namespace cutefish::reef

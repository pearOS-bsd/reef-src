// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The file chooser of the desktop, for open and save panels.

#pragma once

#include "host.h"

// The patterns and the MIME types have semicolons between them and may be null.
struct reef_ui_file_chooser_filter
{
    const char* name;
    const char* patterns;
    const char* mimeTypes;
};

// A control of the program's in the dialog: one of its options is picked, or it is a check box without any.
struct reef_ui_file_chooser_choice
{
    const char* id;
    const char* label;
    const char* const* optionIds;
    const char* const* optionLabels;
    unsigned optionCount;
    const char* selected;
};

// The texts may be null or empty.
struct reef_ui_file_chooser_request
{
    // 0 opens a file, 1 files, 2 a folder; 3 saves a file.
    int mode;
    int modal;
    const char* title;
    const char* acceptLabel;
    const char* folder;
    const char* file;
    const reef_ui_file_chooser_filter* filters;
    unsigned filterCount;
    const reef_ui_file_chooser_choice* choices;
    unsigned choiceCount;
};

struct reef_ui_file_chooser;

namespace cutefish::reef {

// The callback gets no paths when the dialog was cancelled, and what was picked for each of the choices, or null.
reef_ui_file_chooser* hostFileChooserOpen(reef_ui_window* window, const reef_ui_file_chooser_request* request,
                                          void (*callback)(void* context, const char* const* paths, unsigned count,
                                                           const char* const* options),
                                          void* context);
void hostFileChooserDestroy(reef_ui_file_chooser* chooser);
// For tests, which see no dialog: what the next one answers. The options are "id=option".
void hostTestFileChooserAnswer(const char* const* paths, unsigned count, const char* const* options, unsigned optionCount);

} // namespace cutefish::reef

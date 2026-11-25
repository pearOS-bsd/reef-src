// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The desktop around the program: types of files and what opens them, the file manager, the running instance of the
// application.

#pragma once

#include "now_playing.h"

namespace cutefish::reef {

// The functions with a buffer write as much as fits, ended with a zero, and return the length of the whole text.
int hostMimeTypeForFile(const char* path, char* buffer, int size);
int hostMimeIconName(const char* type, int generic, char* buffer, int size);
// The path of a desktop entry, or nothing.
int hostDefaultApplication(const char* type, char* buffer, int size);
// Starts a desktop entry with files or URLs; 0 when it was started.
int hostLaunchApplication(const char* path, const char* const* arguments, int count);
// Opens the folders, or the ones the files are in, with the files selected.
int hostShowInFileManager(const char* const* paths, unsigned count, int select);

// 1 when this process is the application's instance on the session bus, from now on or already; 0 when another one is and
// took the files or URLs; -1 when neither. Nothing is handed over unless `forward`.
int hostInstanceClaim(const char* identifier, int forward, int count, const char* const* items);
void hostInstanceSetShownInDock(int shown);
// The one this process was started with.
void hostSetActivationToken(const char* token);

// Nothing played is null, or a state without a status.
void hostNowPlayingUpdate(const reef_ui_now_playing* state, NowPlaying::CommandHandler command);

} // namespace cutefish::reef

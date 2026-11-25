// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the program plays, on the session bus as org.mpris.MediaPlayer2.

#pragma once

#include "base/dbus.h"

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

// The texts may be null.
struct reef_ui_now_playing
{
    const char* application;
    const char* title;
    const char* artist;
    const char* album;
    // A file.
    const char* artwork;
    // In seconds; no duration is zero.
    double duration;
    double position;
    double rate;
    // Nothing, playing, paused or stopped.
    int status;
    // A bit for each command the program handles.
    uint32_t commands;
    uint64_t track;
};

namespace cutefish::reef {

class NowPlaying
{
public:
    enum Status {
        Nothing,
        Playing,
        Paused,
        Stopped,
    };
    enum Command {
        Play,
        Pause,
        Toggle,
        Stop,
        Next,
        Previous,
        // By so many seconds, or to a position.
        Skip,
        SetPosition,
        Raise,
        Quit,
    };
    using CommandHandler = void (*)(int command, double value);

    NowPlaying(base::dbus::Connection* connection, std::string desktopEntry, CommandHandler command);
    ~NowPlaying();

    void update(const reef_ui_now_playing& state);

private:
    void exportPlayer();
    void withdraw();
    bool can(Command command) const { return m_state.commands >> command & 1; }
    // In microseconds.
    int64_t position() const;
    std::string status() const;
    base::dbus::VariantMap metadata() const;

    base::dbus::Connection* m_connection;
    std::string m_desktopEntry;
    CommandHandler m_command;
    std::string m_name;
    std::unique_ptr<base::dbus::Object> m_application;
    std::unique_ptr<base::dbus::Object> m_player;
    // Without the texts, which are the program's.
    reef_ui_now_playing m_state = {};
    std::string m_identity;
    std::string m_title;
    std::string m_artist;
    std::string m_album;
    std::string m_artworkUrl;
    uint64_t m_track = 0;
    std::chrono::steady_clock::time_point m_updated;
};

} // namespace cutefish::reef

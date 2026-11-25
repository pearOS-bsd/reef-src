// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the program plays, on the session bus as org.mpris.MediaPlayer2.

#include "now_playing.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <unistd.h>
#include <utility>
#include <vector>

namespace cutefish::reef {

using base::dbus::ObjectPath;
using base::dbus::VariantMap;

static constexpr const char* objectPath = "/org/mpris/MediaPlayer2";
static constexpr const char* applicationInterface = "org.mpris.MediaPlayer2";
static constexpr const char* playerInterface = "org.mpris.MediaPlayer2.Player";

static int64_t microseconds(double seconds)
{
    return std::llround(seconds * 1e6);
}

static std::string orEmpty(const char* text)
{
    return text ? std::string(text) : std::string();
}

NowPlaying::NowPlaying(base::dbus::Connection* connection, std::string desktopEntry, CommandHandler command)
    : m_connection(connection)
    , m_desktopEntry(std::move(desktopEntry))
    , m_command(command)
{
}

NowPlaying::~NowPlaying()
{
    withdraw();
}

void NowPlaying::withdraw()
{
    if (!m_player)
        return;
    if (m_connection && !m_name.empty())
        m_connection->releaseName(m_name);
    m_player.reset();
    m_application.reset();
    m_name.clear();
}

int64_t NowPlaying::position() const
{
    double position = m_state.position;
    if (m_state.status == Playing)
        position += m_state.rate * std::chrono::duration<double>(std::chrono::steady_clock::now() - m_updated).count();
    if (m_state.duration > 0)
        position = std::min(position, m_state.duration);
    return microseconds(std::max(position, 0.0));
}

std::string NowPlaying::status() const
{
    switch (m_state.status) {
    case Playing:
        return "Playing";
    case Paused:
        return "Paused";
    default:
        return "Stopped";
    }
}

VariantMap NowPlaying::metadata() const
{
    VariantMap metadata;
    metadata["mpris:trackid"] = ObjectPath{"/org/cutefish/reef/item/" + std::to_string(m_track)};
    if (m_state.duration > 0)
        metadata["mpris:length"] = microseconds(m_state.duration);
    if (!m_title.empty())
        metadata["xesam:title"] = m_title;
    if (!m_artist.empty())
        metadata["xesam:artist"] = std::vector<std::string>{m_artist};
    if (!m_album.empty())
        metadata["xesam:album"] = m_album;
    if (!m_artworkUrl.empty())
        metadata["mpris:artUrl"] = m_artworkUrl;
    return metadata;
}

// An element of a bus name.
static std::string nameElement(const std::string& text)
{
    std::string element;
    for (char c : text)
        element += isalnum(static_cast<unsigned char>(c)) || c == '_' ? c : '_';
    if (element.empty() || isdigit(static_cast<unsigned char>(element[0])))
        element.insert(element.begin(), '_');
    return element;
}

void NowPlaying::exportPlayer()
{
    if (m_player || !m_connection)
        return;
    m_application = std::make_unique<base::dbus::Object>(m_connection, objectPath, applicationInterface);
    m_application->addProperty("Identity", [this] { return m_identity; });
    m_application->addProperty("DesktopEntry", [this] { return m_desktopEntry; });
    m_application->addProperty("CanQuit", [this] { return can(Quit); });
    m_application->addProperty("CanRaise", [this] { return can(Raise); });
    m_application->addProperty("HasTrackList", [] { return false; });
    m_application->addProperty("SupportedUriSchemes", [] { return std::vector<std::string>(); });
    m_application->addProperty("SupportedMimeTypes", [] { return std::vector<std::string>(); });
    m_application->addMethod("Raise", [this] { m_command(Raise, 0); });
    m_application->addMethod("Quit", [this] { m_command(Quit, 0); });
    m_application->publish();

    m_player = std::make_unique<base::dbus::Object>(m_connection, objectPath, playerInterface);
    m_player->addProperty("PlaybackStatus", [this] { return status(); });
    m_player->addProperty("Rate", [this] { return m_state.rate > 0 ? m_state.rate : 1.0; });
    m_player->addProperty("MinimumRate", [] { return 1.0; });
    m_player->addProperty("MaximumRate", [] { return 1.0; });
    m_player->addProperty("Metadata", [this] { return metadata(); });
    m_player->addProperty("Position", [this] { return position(); });
    m_player->addProperty("CanGoNext", [this] { return can(Next); });
    m_player->addProperty("CanGoPrevious", [this] { return can(Previous); });
    m_player->addProperty("CanPlay", [this] { return can(Play) || can(Toggle); });
    m_player->addProperty("CanPause", [this] { return can(Pause) || can(Toggle); });
    m_player->addProperty("CanSeek", [this] { return can(SetPosition) || can(Skip); });
    m_player->addProperty("CanControl", [] { return true; });

    const auto playOrPause = [this](Command command) {
        if (can(command))
            m_command(command, 0);
        else if (can(Toggle) && (m_state.status == Playing) != (command == Play))
            m_command(Toggle, 0);
    };
    const auto send = [this](Command command) {
        if (can(command))
            m_command(command, 0);
    };
    m_player->addMethod("Play", [playOrPause] { playOrPause(Play); });
    m_player->addMethod("Pause", [playOrPause] { playOrPause(Pause); });
    m_player->addMethod("PlayPause", [this, playOrPause] {
        if (can(Toggle))
            m_command(Toggle, 0);
        else
            playOrPause(m_state.status == Playing ? Pause : Play);
    });
    m_player->addMethod("Stop", [send] { send(Stop); });
    m_player->addMethod("Next", [send] { send(Next); });
    m_player->addMethod("Previous", [send] { send(Previous); });
    m_player->addMethod("Seek", [this](int64_t offset) {
        if (can(SetPosition))
            m_command(SetPosition, std::max(0.0, (position() + offset) / 1e6));
        else if (can(Skip))
            m_command(Skip, offset / 1e6);
    });
    m_player->addMethod("SetPosition", [this](ObjectPath track, int64_t position) {
        if (track.path != "/org/cutefish/reef/item/" + std::to_string(m_track) || position < 0)
            return;
        if (m_state.duration > 0 && position > microseconds(m_state.duration))
            return;
        if (can(SetPosition))
            m_command(SetPosition, position / 1e6);
    });
    m_player->addMethod("OpenUri", [](std::string) {});
    m_player->addSignal("Seeked", "x");
    m_player->publish();

    m_name = "org.mpris.MediaPlayer2." + nameElement(m_desktopEntry.empty() ? m_identity : m_desktopEntry) + ".instance" + std::to_string(getpid());
    m_connection->requestName(m_name);
}

void NowPlaying::update(const reef_ui_now_playing& state)
{
    if (!state.status) {
        withdraw();
        m_state = {};
        return;
    }
    const int64_t position = this->position();
    const reef_ui_now_playing before = m_state;
    const std::string title = m_title;
    const std::string artist = m_artist;
    const std::string album = m_album;
    const std::string artworkUrl = m_artworkUrl;
    const std::string identity = m_identity;
    const uint64_t track = m_track;
    m_state = state;
    m_state.application = m_state.title = m_state.artist = m_state.album = m_state.artwork = nullptr;
    m_updated = std::chrono::steady_clock::now();
    m_identity = orEmpty(state.application);
    m_title = orEmpty(state.title);
    m_artist = orEmpty(state.artist);
    m_album = orEmpty(state.album);
    m_artworkUrl = state.artwork && *state.artwork ? "file://" + std::string(state.artwork) : std::string();
    m_track = state.track;
    if (!m_player) {
        exportPlayer();
        return;
    }

    std::vector<std::string> changed;
    if (m_identity != identity)
        changed.push_back("Identity");
    if ((m_state.commands ^ before.commands) & (1 << Raise | 1 << Quit)) {
        changed.push_back("CanQuit");
        changed.push_back("CanRaise");
    }
    if (!changed.empty())
        m_application->emitPropertiesChanged(changed);

    std::vector<std::string> playerChanged;
    if (m_state.status != before.status)
        playerChanged.push_back("PlaybackStatus");
    if (m_state.rate != before.rate)
        playerChanged.push_back("Rate");
    if (m_track != track || m_title != title || m_artist != artist || m_album != album || m_artworkUrl != artworkUrl || m_state.duration != before.duration)
        playerChanged.push_back("Metadata");
    if (m_state.commands != before.commands) {
        for (const char* name : {"CanGoNext", "CanGoPrevious", "CanPlay", "CanPause", "CanSeek"})
            playerChanged.push_back(name);
    }
    if (!playerChanged.empty())
        m_player->emitPropertiesChanged(playerChanged);
    if (track == m_track && std::abs(this->position() - position) > 1000000)
        m_player->emitSignal("Seeked", this->position());
}

} // namespace cutefish::reef

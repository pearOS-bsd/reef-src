// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The timing of displays, as the kernel's DRM devices know it.

#include "displays.h"

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <fcntl.h>
#include <iterator>
#include <map>
#include <mutex>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

namespace cutefish::reef {

namespace {

// What drives a display.
struct Crtc
{
    int fd = -1;
    uint32_t index = 0;
    uint64_t period = 0;
    std::chrono::steady_clock::time_point time;
};

} // namespace

static std::mutex mutex;
static std::vector<int> devices;
static bool devicesOpened = false;
static std::map<std::string, Crtc> crtcs;

static std::vector<uint32_t> resourceIds(int fd, uint32_t count, bool connectors)
{
    std::vector<uint32_t> ids(count);
    drm_mode_card_res resources = {};
    if (connectors) {
        resources.connector_id_ptr = reinterpret_cast<uintptr_t>(ids.data());
        resources.count_connectors = count;
    } else {
        resources.crtc_id_ptr = reinterpret_cast<uintptr_t>(ids.data());
        resources.count_crtcs = count;
    }
    if (ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &resources) != 0)
        ids.clear();
    return ids;
}

static const char* connectorType(uint32_t type)
{
    // In the order of DRM_MODE_CONNECTOR_*, as the kernel names them.
    static const char* const names[] = {"Unknown", "VGA", "DVI-I", "DVI-D", "DVI-A", "Composite", "SVIDEO", "LVDS", "Component",
                                        "DIN", "DP", "HDMI-A", "HDMI-B", "TV", "eDP", "Virtual", "DSI", "DPI", "Writeback",
                                        "SPI", "USB"};
    return type < std::size(names) ? names[type] : "Unknown";
}

static bool findCrtc(const std::string& name, Crtc& found)
{
    if (!devicesOpened) {
        devicesOpened = true;
        for (int i = 0; i < 8; i++) {
            char path[32];
            snprintf(path, sizeof path, "/dev/dri/card%d", i);
            const int fd = open(path, O_RDWR | O_CLOEXEC);
            if (fd >= 0)
                devices.push_back(fd);
        }
    }
    for (int fd : devices) {
        drm_mode_card_res resources = {};
        if (ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &resources) != 0 || resources.count_crtcs == 0)
            continue;
        const std::vector<uint32_t> ids = resourceIds(fd, resources.count_crtcs, false);
        uint32_t wanted = 0;
        if (!name.empty()) {
            const std::vector<uint32_t> connectors = resourceIds(fd, resources.count_connectors, true);
            for (uint32_t id : connectors) {
                drm_mode_get_connector connector = {};
                connector.connector_id = id;
                if (ioctl(fd, DRM_IOCTL_MODE_GETCONNECTOR, &connector) != 0)
                    continue;
                const char* type = connectorType(connector.connector_type);
                if (name != std::string(type) + "-" + std::to_string(connector.connector_type_id))
                    continue;
                drm_mode_get_encoder encoder = {};
                encoder.encoder_id = connector.encoder_id;
                if (encoder.encoder_id && ioctl(fd, DRM_IOCTL_MODE_GETENCODER, &encoder) == 0)
                    wanted = encoder.crtc_id;
                break;
            }
            if (wanted == 0)
                continue;
        }
        for (size_t i = 0; i < ids.size(); i++) {
            if (wanted && ids[i] != wanted)
                continue;
            drm_mode_crtc crtc = {};
            crtc.crtc_id = ids[i];
            if (ioctl(fd, DRM_IOCTL_MODE_GETCRTC, &crtc) != 0 || !crtc.mode_valid || !crtc.mode.clock)
                continue;
            found.fd = fd;
            found.index = uint32_t(i);
            found.period = uint64_t(crtc.mode.htotal) * crtc.mode.vtotal * 1000000 / crtc.mode.clock;
            return true;
        }
    }
    return false;
}

static bool crtcOf(const char* name, Crtc& result)
{
    const std::string key = name ? name : "";
    const auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(mutex);
    Crtc& crtc = crtcs[key];
    if (crtc.fd < 0 || now - crtc.time > std::chrono::seconds(1)) {
        Crtc found;
        if (!findCrtc(key, found) && !key.empty())
            findCrtc(std::string(), found);
        crtc = found;
        crtc.time = now;
    }
    result = crtc;
    return result.fd >= 0;
}

// 285
uint64_t hostDisplayPeriod(const char* name)
{
    Crtc crtc;
    return crtcOf(name, crtc) ? crtc.period : 0;
}

// 284
int hostDisplayWaitVblank(const char* name, uint64_t* time)
{
    Crtc crtc;
    if (!crtcOf(name, crtc))
        return 0;
    drm_wait_vblank wait = {};
    uint32_t type = _DRM_VBLANK_RELATIVE;
    if (crtc.index == 1)
        type |= _DRM_VBLANK_SECONDARY;
    else if (crtc.index > 1)
        type |= crtc.index << _DRM_VBLANK_HIGH_CRTC_SHIFT & _DRM_VBLANK_HIGH_CRTC_MASK;
    wait.request.type = drm_vblank_seq_type(type);
    wait.request.sequence = 1;
    do {
        if (ioctl(crtc.fd, DRM_IOCTL_WAIT_VBLANK, &wait) == 0) {
            *time = uint64_t(wait.reply.tval_sec) * 1000000000 + uint64_t(wait.reply.tval_usec) * 1000;
            return 1;
        }
    } while (errno == EINTR);
    return 0;
}

} // namespace cutefish::reef

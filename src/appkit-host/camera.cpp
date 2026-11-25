// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Cameras of V4L2. Frames are given as the camera makes them.

#include "camera.h"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <iterator>
#include <linux/videodev2.h>
#include <poll.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

struct reef_ui_camera
{
    int fd = -1;
    // What is mapped, and its size.
    std::vector<std::pair<void*, size_t>> buffers;
};

namespace cutefish::reef {

namespace {

struct FrameSize
{
    int width;
    int height;
};

} // namespace

static int request(int fd, unsigned long number, void* argument)
{
    int result;
    do
        result = ioctl(fd, number, argument);
    while (result == -1 && errno == EINTR);
    return result;
}

static void frameRates(int fd, uint32_t pixelFormat, int width, int height, double& minimum, double& maximum)
{
    minimum = 0;
    maximum = 0;
    v4l2_frmivalenum interval = {};
    interval.pixel_format = pixelFormat;
    interval.width = width;
    interval.height = height;
    for (; request(fd, VIDIOC_ENUM_FRAMEINTERVALS, &interval) == 0; interval.index++) {
        if (interval.type != V4L2_FRMIVAL_TYPE_DISCRETE) {
            const v4l2_frmival_stepwise& steps = interval.stepwise;
            if (steps.min.numerator && steps.max.numerator) {
                maximum = double(steps.min.denominator) / steps.min.numerator;
                minimum = double(steps.max.denominator) / steps.max.numerator;
            }
            break;
        }
        if (interval.discrete.numerator) {
            const double rate = double(interval.discrete.denominator) / interval.discrete.numerator;
            minimum = minimum == 0 ? rate : std::min(minimum, rate);
            maximum = std::max(maximum, rate);
        }
    }
    if (maximum == 0) {
        maximum = 30;
        minimum = 30;
    }
}

static void addFormat(int fd, reef_ui_camera_info& camera, uint32_t pixelFormat, FrameSize size)
{
    if (camera.formatCount >= int(std::size(camera.formats)))
        return;
    reef_ui_camera_format& format = camera.formats[camera.formatCount++];
    format.pixelFormat = pixelFormat;
    format.width = size.width;
    format.height = size.height;
    frameRates(fd, pixelFormat, size.width, size.height, format.minimumRate, format.maximumRate);
}

static void addFormats(int fd, reef_ui_camera_info& camera, uint32_t pixelFormat)
{
    // For cameras that scale to any size.
    static constexpr FrameSize sizes[] = {{320, 240}, {640, 360}, {640, 480}, {960, 540}, {1280, 720}, {1920, 1080}, {3840, 2160}};
    v4l2_frmsizeenum frame = {};
    frame.pixel_format = pixelFormat;
    for (; request(fd, VIDIOC_ENUM_FRAMESIZES, &frame) == 0; frame.index++) {
        if (frame.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
            addFormat(fd, camera, pixelFormat, {int(frame.discrete.width), int(frame.discrete.height)});
            continue;
        }
        const v4l2_frmsize_stepwise& steps = frame.stepwise;
        for (const FrameSize size : sizes) {
            const uint32_t width = uint32_t(size.width);
            const uint32_t height = uint32_t(size.height);
            if (width >= steps.min_width && width <= steps.max_width && height >= steps.min_height && height <= steps.max_height
                && (steps.step_width == 0 || (width - steps.min_width) % steps.step_width == 0)
                && (steps.step_height == 0 || (height - steps.min_height) % steps.step_height == 0))
                addFormat(fd, camera, pixelFormat, size);
        }
        break;
    }
}

// 291
int hostCameraList(reef_ui_camera_info* cameras, int count)
{
    std::vector<std::string> paths;
    DIR* directory = opendir("/dev");
    if (!directory)
        return 0;
    while (const dirent* entry = readdir(directory)) {
        if (strncmp(entry->d_name, "video", 5) == 0)
            paths.push_back(std::string("/dev/") + entry->d_name);
    }
    closedir(directory);
    std::sort(paths.begin(), paths.end(), [](const std::string& a, const std::string& b) {
        return a.size() != b.size() ? a.size() < b.size() : a < b;
    });
    int found = 0;
    for (auto path = paths.begin(); path != paths.end() && found < count; ++path) {
        const int fd = open(path->c_str(), O_RDWR | O_NONBLOCK | O_CLOEXEC);
        if (fd < 0)
            continue;
        v4l2_capability capability = {};
        const int result = request(fd, VIDIOC_QUERYCAP, &capability);
        const uint32_t capabilities = capability.capabilities & V4L2_CAP_DEVICE_CAPS ? capability.device_caps : capability.capabilities;
        const uint32_t needed = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
        if (result != 0 || (capabilities & needed) != needed) {
            close(fd);
            continue;
        }
        reef_ui_camera_info& camera = cameras[found];
        camera = {};
        snprintf(camera.path, sizeof camera.path, "%s", path->c_str());
        snprintf(camera.name, sizeof camera.name, "%s", capability.card);
        snprintf(camera.bus, sizeof camera.bus, "%s", capability.bus_info);
        v4l2_fmtdesc description = {};
        description.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        for (; request(fd, VIDIOC_ENUM_FMT, &description) == 0; description.index++) {
            const uint32_t pixelFormat = description.pixelformat;
            if (pixelFormat == V4L2_PIX_FMT_YUYV || pixelFormat == V4L2_PIX_FMT_UYVY || pixelFormat == V4L2_PIX_FMT_NV12
                || pixelFormat == V4L2_PIX_FMT_MJPEG)
                addFormats(fd, camera, pixelFormat);
        }
        const bool usable = camera.formatCount > 0;
        close(fd);
        found += usable;
    }
    return found;
}

// 292
reef_ui_camera* hostCameraOpen(const char* path, uint32_t pixelFormat, int width, int height, double rate)
{
    reef_ui_camera* camera = new reef_ui_camera;
    camera->fd = open(path, O_RDWR | O_NONBLOCK | O_CLOEXEC);
    if (camera->fd < 0) {
        delete camera;
        return nullptr;
    }
    const int fd = camera->fd;
    v4l2_format format = {};
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = width;
    format.fmt.pix.height = height;
    format.fmt.pix.pixelformat = pixelFormat;
    format.fmt.pix.field = V4L2_FIELD_NONE;
    bool good = request(fd, VIDIOC_S_FMT, &format) == 0 && format.fmt.pix.pixelformat == pixelFormat
        && int(format.fmt.pix.width) == width && int(format.fmt.pix.height) == height;
    if (good && rate > 0) {
        v4l2_streamparm parameters = {};
        parameters.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        parameters.parm.capture.timeperframe.numerator = 1000;
        parameters.parm.capture.timeperframe.denominator = uint32_t(rate * 1000 + 0.5);
        request(fd, VIDIOC_S_PARM, &parameters);
    }
    v4l2_requestbuffers buffers = {};
    buffers.count = 4;
    buffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffers.memory = V4L2_MEMORY_MMAP;
    good = good && request(fd, VIDIOC_REQBUFS, &buffers) == 0 && buffers.count > 0;
    for (uint32_t i = 0; good && i < buffers.count; i++) {
        v4l2_buffer buffer = {};
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;
        void* data = MAP_FAILED;
        if (request(fd, VIDIOC_QUERYBUF, &buffer) == 0)
            data = mmap(nullptr, buffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buffer.m.offset);
        good = data != MAP_FAILED;
        if (good) {
            camera->buffers.emplace_back(data, buffer.length);
            good = request(fd, VIDIOC_QBUF, &buffer) == 0;
        }
    }
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (!good || request(fd, VIDIOC_STREAMON, &type) != 0) {
        hostCameraClose(camera);
        return nullptr;
    }
    return camera;
}

// 293
int hostCameraRead(reef_ui_camera* camera, void* data, size_t capacity, size_t* size, uint64_t* time, int timeout)
{
    pollfd events = {camera->fd, POLLIN, 0};
    const int ready = poll(&events, 1, timeout);
    if (ready == 0 || (ready < 0 && errno == EINTR))
        return 1;
    if (ready < 0 || (events.revents & (POLLERR | POLLHUP | POLLNVAL)))
        return -1;
    v4l2_buffer buffer = {};
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory = V4L2_MEMORY_MMAP;
    if (request(camera->fd, VIDIOC_DQBUF, &buffer) != 0)
        return errno == EAGAIN ? 1 : -1;
    const auto& [source, sourceSize] = camera->buffers[buffer.index];
    size_t length = sourceSize;
    if (buffer.bytesused)
        length = std::min<size_t>(buffer.bytesused, length);
    if (length > capacity)
        length = capacity;
    memcpy(data, source, length);
    *size = length;
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    *time = uint64_t(now.tv_sec) * 1000000000 + now.tv_nsec;
    if ((buffer.flags & V4L2_BUF_FLAG_TIMESTAMP_MASK) == V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC
        && (buffer.timestamp.tv_sec || buffer.timestamp.tv_usec))
        *time = std::min(*time, uint64_t(buffer.timestamp.tv_sec) * 1000000000 + buffer.timestamp.tv_usec * 1000);
    return request(camera->fd, VIDIOC_QBUF, &buffer) == 0 ? 0 : -1;
}

// 294
void hostCameraClose(reef_ui_camera* camera)
{
    if (!camera)
        return;
    if (camera->fd >= 0) {
        int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        request(camera->fd, VIDIOC_STREAMOFF, &type);
    }
    for (const auto& [data, size] : camera->buffers)
        munmap(data, size);
    if (camera->fd >= 0)
        close(camera->fd);
    delete camera;
}

} // namespace cutefish::reef

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The GPU's side of video decoding: the display, the surfaces and the contexts of VA-API.

#include "video_private.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>

namespace cutefish::reef::video {

static void ignoreMessage(void*, const char*)
{
}

template<typename Function>
static void resolve(void* library, Function& function, const char* name)
{
    function = reinterpret_cast<Function>(dlsym(library, name));
}

static VaApi load()
{
    VaApi api = {};
    void* library = dlopen("libva.so.2", RTLD_NOW);
    if (!library)
        return api;
    void* drm = dlopen("libva-drm.so.2", RTLD_NOW);
    if (!drm)
        return api;
    resolve(library, api.initialize, "vaInitialize");
    resolve(library, api.terminate, "vaTerminate");
    resolve(library, api.errorStr, "vaErrorStr");
    resolve(library, api.setErrorCallback, "vaSetErrorCallback");
    resolve(library, api.setInfoCallback, "vaSetInfoCallback");
    resolve(library, api.maxNumProfiles, "vaMaxNumProfiles");
    resolve(library, api.queryConfigProfiles, "vaQueryConfigProfiles");
    resolve(library, api.maxNumEntrypoints, "vaMaxNumEntrypoints");
    resolve(library, api.queryConfigEntrypoints, "vaQueryConfigEntrypoints");
    resolve(library, api.getConfigAttributes, "vaGetConfigAttributes");
    resolve(library, api.createConfig, "vaCreateConfig");
    resolve(library, api.destroyConfig, "vaDestroyConfig");
    resolve(library, api.createSurfaces, "vaCreateSurfaces");
    resolve(library, api.destroySurfaces, "vaDestroySurfaces");
    resolve(library, api.createContext, "vaCreateContext");
    resolve(library, api.destroyContext, "vaDestroyContext");
    resolve(library, api.createBuffer, "vaCreateBuffer");
    resolve(library, api.destroyBuffer, "vaDestroyBuffer");
    resolve(library, api.beginPicture, "vaBeginPicture");
    resolve(library, api.renderPicture, "vaRenderPicture");
    resolve(library, api.endPicture, "vaEndPicture");
    resolve(library, api.syncSurface, "vaSyncSurface");
    resolve(library, api.maxNumImageFormats, "vaMaxNumImageFormats");
    resolve(library, api.queryImageFormats, "vaQueryImageFormats");
    resolve(library, api.createImage, "vaCreateImage");
    resolve(library, api.deriveImage, "vaDeriveImage");
    resolve(library, api.getImage, "vaGetImage");
    resolve(library, api.destroyImage, "vaDestroyImage");
    resolve(library, api.mapBuffer, "vaMapBuffer");
    resolve(library, api.unmapBuffer, "vaUnmapBuffer");
    resolve(library, api.exportSurfaceHandle, "vaExportSurfaceHandle");
    resolve(drm, api.getDisplayDRM, "vaGetDisplayDRM");
    api.loaded = api.initialize && api.terminate && api.maxNumProfiles && api.queryConfigProfiles && api.maxNumEntrypoints
        && api.queryConfigEntrypoints && api.createConfig && api.destroyConfig && api.createSurfaces && api.destroySurfaces
        && api.createContext && api.destroyContext && api.createBuffer && api.destroyBuffer && api.beginPicture
        && api.renderPicture && api.endPicture && api.syncSurface && api.maxNumImageFormats && api.queryImageFormats
        && api.createImage && api.getImage && api.destroyImage && api.mapBuffer && api.unmapBuffer && api.getDisplayDRM;
    return api;
}

const VaApi& vaApi()
{
    static const VaApi api = load();
    return api;
}

std::shared_ptr<VaDisplay> VaDisplay::open()
{
    static std::weak_ptr<VaDisplay> shared;
    static std::mutex mutex;
    std::lock_guard lock(mutex);
    if (std::shared_ptr<VaDisplay> display = shared.lock())
        return display;
    const VaApi& va = vaApi();
    if (!va.loaded)
        return nullptr;
    for (int i = 128; i < 136; i++) {
        char path[32];
        snprintf(path, sizeof(path), "/dev/dri/renderD%d", i);
        const int fd = ::open(path, O_RDWR | O_CLOEXEC);
        if (fd < 0)
            continue;
        VADisplay handle = va.getDisplayDRM(fd);
        if (!handle) {
            close(fd);
            continue;
        }
        if (va.setErrorCallback)
            va.setErrorCallback(handle, ignoreMessage, nullptr);
        if (va.setInfoCallback)
            va.setInfoCallback(handle, ignoreMessage, nullptr);
        int major = 0;
        int minor = 0;
        if (va.initialize(handle, &major, &minor) != VA_STATUS_SUCCESS) {
            close(fd);
            continue;
        }
        std::vector<VAProfile> profiles(std::max(va.maxNumProfiles(handle), 1));
        int count = 0;
        if (va.queryConfigProfiles(handle, profiles.data(), &count) != VA_STATUS_SUCCESS)
            count = 0;
        profiles.resize(count);
        std::vector<VAProfile> decodable;
        for (VAProfile profile : profiles) {
            std::vector<VAEntrypoint> entrypoints(std::max(va.maxNumEntrypoints(handle), 1));
            int count = 0;
            if (va.queryConfigEntrypoints(handle, profile, entrypoints.data(), &count) != VA_STATUS_SUCCESS)
                continue;
            entrypoints.resize(count);
            if (std::find(entrypoints.begin(), entrypoints.end(), VAEntrypointVLD) != entrypoints.end())
                decodable.push_back(profile);
        }
        if (decodable.empty()) {
            va.terminate(handle);
            close(fd);
            continue;
        }
        std::shared_ptr<VaDisplay> display(new VaDisplay);
        display->m_api = &va;
        display->m_handle = handle;
        display->m_fd = fd;
        display->m_profiles = std::move(decodable);
        shared = display;
        return display;
    }
    return nullptr;
}

VaDisplay::~VaDisplay()
{
    if (m_handle)
        m_api->terminate(m_handle);
    if (m_fd >= 0)
        close(m_fd);
}

bool VaDisplay::supports(VAProfile profile) const
{
    return std::find(m_profiles.begin(), m_profiles.end(), profile) != m_profiles.end();
}

SurfacePool::SurfacePool(std::shared_ptr<VaDisplay> display, uint32_t format, uint32_t fourcc, int width, int height)
    : m_display(std::move(display))
    , m_format(format)
    , m_fourcc(fourcc)
    , m_width(width)
    , m_height(height)
{
}

SurfacePool::~SurfacePool()
{
    if (m_hasImage)
        m_display->api().destroyImage(m_display->handle(), m_image.image_id);
    if (!m_all.empty())
        m_display->api().destroySurfaces(m_display->handle(), m_all.data(), int(m_all.size()));
}

std::shared_ptr<Surface> SurfacePool::take()
{
    VASurfaceID id = VA_INVALID_SURFACE;
    {
        std::lock_guard lock(m_mutex);
        if (!m_free.empty()) {
            id = m_free.back();
            m_free.pop_back();
        }
    }
    if (id == VA_INVALID_SURFACE) {
        VASurfaceAttrib attribute = {};
        attribute.type = VASurfaceAttribPixelFormat;
        attribute.flags = VA_SURFACE_ATTRIB_SETTABLE;
        attribute.value.type = VAGenericValueTypeInteger;
        attribute.value.value.i = int(m_fourcc);
        if (m_display->api().createSurfaces(m_display->handle(), m_format, m_width, m_height, &id, 1, &attribute, 1)
            != VA_STATUS_SUCCESS)
            return nullptr;
        std::lock_guard lock(m_mutex);
        m_all.push_back(id);
    }
    return std::make_shared<Surface>(shared_from_this(), id);
}

void SurfacePool::put(VASurfaceID id)
{
    std::lock_guard lock(m_mutex);
    m_free.push_back(id);
}

static bool findImageFormat(const VaDisplay& display, uint32_t fourcc, VAImageFormat& format)
{
    const VaApi& va = display.api();
    std::vector<VAImageFormat> formats(std::max(va.maxNumImageFormats(display.handle()), 1));
    int count = 0;
    if (va.queryImageFormats(display.handle(), formats.data(), &count) != VA_STATUS_SUCCESS)
        return false;
    for (int i = 0; i < count; i++) {
        if (formats[i].fourcc == fourcc) {
            format = formats[i];
            return true;
        }
    }
    return false;
}

const VAImage* SurfacePool::image()
{
    if (m_hasImage)
        return &m_image;
    if (m_imageFailed)
        return nullptr;
    VAImageFormat format;
    if (!findImageFormat(*m_display, m_fourcc, format)
        || m_display->api().createImage(m_display->handle(), &format, m_width, m_height, &m_image) != VA_STATUS_SUCCESS) {
        m_imageFailed = true;
        return nullptr;
    }
    m_hasImage = true;
    return &m_image;
}

Surface::~Surface()
{
    m_pool->put(m_id);
}

static bool copyImage(const VaDisplay& display, const VAImage& image, uint8_t* const* planes, const size_t* strides, int width,
                      int height, int sampleSize)
{
    const VaApi& va = display.api();
    void* data = nullptr;
    if (va.mapBuffer(display.handle(), image.buf, &data) != VA_STATUS_SUCCESS)
        return false;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    const int rows[2] = {height, (height + 1) / 2};
    const size_t lengths[2] = {size_t(width) * sampleSize, size_t((width + 1) / 2) * sampleSize * 2};
    for (int plane = 0; plane < 2 && plane < int(image.num_planes); plane++) {
        const uint8_t* source = bytes + image.offsets[plane];
        const size_t length = std::min(lengths[plane], std::min<size_t>(strides[plane], image.pitches[plane]));
        for (int row = 0; row < rows[plane]; row++)
            memcpy(planes[plane] + strides[plane] * row, source + size_t(image.pitches[plane]) * row, length);
    }
    va.unmapBuffer(display.handle(), image.buf);
    return true;
}

bool Surface::copy(uint8_t* const* planes, const size_t* strides, int width, int height) const
{
    SurfacePool& pool = *m_pool;
    const VaDisplay& display = pool.display();
    const VaApi& va = display.api();
    if (va.syncSurface(display.handle(), m_id) != VA_STATUS_SUCCESS)
        return false;
    const int sampleSize = pool.fourcc() == VA_FOURCC_P010 ? 2 : 1;
    {
        std::lock_guard lock(pool.imageMutex());
        const VAImage* image = pool.image();
        if (image
            && va.getImage(display.handle(), m_id, 0, 0, pool.width(), pool.height(), image->image_id) == VA_STATUS_SUCCESS
            && copyImage(display, *image, planes, strides, width, height, sampleSize))
            return true;
    }
    if (!va.deriveImage)
        return false;
    VAImage image;
    if (va.deriveImage(display.handle(), m_id, &image) != VA_STATUS_SUCCESS)
        return false;
    const bool copied = image.format.fourcc == pool.fourcc()
        && copyImage(display, image, planes, strides, width, height, sampleSize);
    va.destroyImage(display.handle(), image.image_id);
    return copied;
}

static void closeObjects(const VADRMPRIMESurfaceDescriptor& descriptor)
{
    for (uint32_t i = 0; i < descriptor.num_objects; i++)
        close(descriptor.objects[i].fd);
}

bool Surface::exportPlanes(Planes& planes) const
{
    const VaDisplay& display = m_pool->display();
    const VaApi& va = display.api();
    if (!va.exportSurfaceHandle)
        return false;
    VADRMPRIMESurfaceDescriptor descriptor = {};
    if (va.exportSurfaceHandle(display.handle(), m_id, VA_SURFACE_ATTRIB_MEM_TYPE_DRM_PRIME_2,
                               VA_EXPORT_SURFACE_READ_ONLY | VA_EXPORT_SURFACE_SEPARATE_LAYERS, &descriptor)
        != VA_STATUS_SUCCESS)
        return false;
    if (va.syncSurface(display.handle(), m_id) != VA_STATUS_SUCCESS || descriptor.num_layers > 4) {
        closeObjects(descriptor);
        return false;
    }
    planes.fourcc = descriptor.fourcc;
    planes.count = int(descriptor.num_layers);
    for (uint32_t i = 0; i < descriptor.num_layers; i++) {
        const auto& layer = descriptor.layers[i];
        const auto& object = descriptor.objects[layer.object_index[0]];
        planes.fds[i] = fcntl(object.fd, F_DUPFD_CLOEXEC, 0);
        planes.formats[i] = layer.drm_format;
        planes.offsets[i] = layer.offset[0];
        planes.pitches[i] = layer.pitch[0];
        planes.modifiers[i] = object.drm_format_modifier;
    }
    closeObjects(descriptor);
    return true;
}

Context::Context(std::shared_ptr<VaDisplay> display, VAProfile profile, int width, int height, int depth)
    : m_display(std::move(display))
{
    const VaApi& va = m_display->api();
    const uint32_t format = depth > 8 ? VA_RT_FORMAT_YUV420_10 : VA_RT_FORMAT_YUV420;
    VAConfigAttrib attribute = {VAConfigAttribRTFormat, 0};
    if (va.getConfigAttributes(m_display->handle(), profile, VAEntrypointVLD, &attribute, 1) != VA_STATUS_SUCCESS
        || !(attribute.value & format))
        return;
    attribute.value = format;
    if (va.createConfig(m_display->handle(), profile, VAEntrypointVLD, &attribute, 1, &m_config) != VA_STATUS_SUCCESS) {
        m_config = VA_INVALID_ID;
        return;
    }
    if (va.createContext(m_display->handle(), m_config, width, height, VA_PROGRESSIVE, nullptr, 0, &m_context)
        != VA_STATUS_SUCCESS) {
        m_context = VA_INVALID_ID;
        return;
    }
    m_format = format;
    m_depth = depth;
    const uint32_t fourcc = depth > 8 ? VA_FOURCC_P010 : VA_FOURCC_NV12;
    m_pool = std::make_shared<SurfacePool>(m_display, format, fourcc, width, height);
}

Context::~Context()
{
    discard();
    const VaApi& va = m_display->api();
    if (m_context != VA_INVALID_ID)
        va.destroyContext(m_display->handle(), m_context);
    if (m_config != VA_INVALID_ID)
        va.destroyConfig(m_display->handle(), m_config);
}

void Context::resize(int width, int height)
{
    const uint32_t fourcc = m_depth > 8 ? VA_FOURCC_P010 : VA_FOURCC_NV12;
    m_pool = std::make_shared<SurfacePool>(m_display, m_format, fourcc, width, height);
}

bool Context::addBuffer(VABufferType type, const void* data, size_t size)
{
    VABufferID buffer = VA_INVALID_ID;
    if (m_display->api().createBuffer(m_display->handle(), m_context, type, size, 1, const_cast<void*>(data), &buffer)
        != VA_STATUS_SUCCESS)
        return false;
    m_buffers.push_back(buffer);
    return true;
}

bool Context::render(VASurfaceID target)
{
    const VaApi& va = m_display->api();
    bool ok = va.beginPicture(m_display->handle(), m_context, target) == VA_STATUS_SUCCESS
        && va.renderPicture(m_display->handle(), m_context, m_buffers.data(), int(m_buffers.size())) == VA_STATUS_SUCCESS;
    ok = va.endPicture(m_display->handle(), m_context) == VA_STATUS_SUCCESS && ok;
    discard();
    return ok;
}

void Context::discard()
{
    for (VABufferID buffer : m_buffers)
        m_display->api().destroyBuffer(m_display->handle(), buffer);
    m_buffers.clear();
}

} // namespace cutefish::reef::video

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Downloads.

#include "web.h"

#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <WebKit/WKDownloadDelegate.h>
#include <WebKit/WKHTTPHeaders.h>
#include <WebKit/WKURLRequest.h>
#include <WebKit/WKURLResponse.h>

namespace cutefish::reef::web {

struct Destination;

struct DownloadClient
{
    void (*redirect)(void* context, const char* url, WKDownloadListenerRef listener);
    void (*challenge)(void* context, const Challenge* challenge);
    void (*destination)(void* context, const char* url, const char* type, int64_t size, const char* name, Destination* destination);
    void (*progress)(void* context, int64_t written, int64_t expected);
    void (*finished)(void* context);
    void (*failed)(void* context, const Error* error);
    void (*resumable)(void* context, const char* url, const char* path, int64_t size, const char* validator);
};

struct Download
{
    WKDownloadRef download;
    const DownloadClient* client = nullptr;
    void* context = nullptr;
    std::string url;
    std::string path;
    std::string validator;
    int64_t written = 0;
    // Where a resumed download goes on; negative for a new one.
    int64_t offset = -1;
    std::string target;
    // Whether the file at path is only what comes after offset.
    bool partial = false;
};

static bool append(const std::string& from, const std::string& to)
{
    const int in = open(from.c_str(), O_RDONLY | O_CLOEXEC);
    const int out = open(to.c_str(), O_WRONLY | O_APPEND | O_CLOEXEC);
    bool good = in >= 0 && out >= 0;
    char buffer[65536];
    while (good) {
        const ssize_t size = read(in, buffer, sizeof buffer);
        if (size <= 0) {
            good = size == 0;
            break;
        }
        for (ssize_t done = 0; good && done < size;) {
            const ssize_t count = write(out, buffer + done, size_t(size - done));
            good = count > 0;
            done += count;
        }
    }
    if (in >= 0)
        close(in);
    if (out >= 0)
        close(out);
    unlink(from.c_str());
    return good;
}

static void willRedirect(WKDownloadRef, WKURLResponseRef, WKURLRequestRef request, WKDownloadListenerRef listener, const void* info)
{
    const Download* download = static_cast<const Download*>(info);
    if (!download->client || !download->client->redirect) {
        WKDownloadListenerAllowRedirect(listener, true);
        return;
    }
    const std::string url = takeUrl(WKURLRequestCopyURL(request));
    download->client->redirect(download->context, url.c_str(), listener);
}

static void didReceiveDownloadChallenge(WKDownloadRef, WKAuthenticationChallengeRef challenge, const void* info)
{
    const Download* download = static_cast<const Download*>(info);
    if (!download->client || !download->client->challenge) {
        WKAuthenticationDecisionListenerPerformDefaultHandling(WKAuthenticationChallengeGetDecisionListener(challenge));
        return;
    }
    ChallengeText text;
    const Challenge value = makeChallenge(challenge, text);
    download->client->challenge(download->context, &value);
}

static void didWriteData(WKDownloadRef, long long bytes, long long total, long long expected, const void* info)
{
    Download* download = static_cast<Download*>(const_cast<void*>(info));
    download->written = total > 0 ? total : download->written + bytes;
    const int64_t before = download->partial ? download->offset : 0;
    if (download->client && download->client->progress)
        download->client->progress(download->context, before + download->written, expected > 0 ? before + expected : -1);
}

static void didFinish(WKDownloadRef, const void* info)
{
    const Download* download = static_cast<const Download*>(info);
    if (download->offset >= 0) {
        const bool moved = download->partial ? append(download->path, download->target)
                                             : rename(download->path.c_str(), download->target.c_str()) == 0;
        if (!moved) {
            if (download->client && download->client->failed) {
                const Error error = {-3000, "NSURLErrorDomain", "The download could not be written.", download->url.c_str()};
                download->client->failed(download->context, &error);
            }
            return;
        }
    }
    if (download->client && download->client->finished)
        download->client->finished(download->context);
}

static void didFail(WKDownloadRef, WKErrorRef error, WKDataRef resumeData, const void* info)
{
    const Download* download = static_cast<const Download*>(info);
    const bool resumable = resumeData && WKDataGetSize(resumeData);
    const std::string path = download->offset >= 0 ? download->target : download->path;
    bool kept = false;
    if (resumable && !download->path.empty()) {
        if (download->offset < 0)
            kept = rename(download->path.c_str(), path.c_str()) == 0;
        else {
            kept = download->partial ? append(download->path, path) : rename(download->path.c_str(), path.c_str()) == 0;
            if (!kept)
                unlink(download->path.c_str());
        }
    } else if (download->offset >= 0)
        unlink(download->path.c_str());
    if (kept && download->client && download->client->resumable) {
        struct stat status;
        if (stat(path.c_str(), &status) == 0 && status.st_size > 0) {
            download->client->resumable(download->context, download->url.c_str(), path.c_str(), status.st_size,
                download->validator.c_str());
        }
    }
    if (download->client && download->client->failed) {
        ErrorText text;
        Error value = {};
        if (error)
            value = makeError(error, text);
        download->client->failed(download->context, &value);
    }
}

struct Destination
{
    Download* download;
    WKDownloadListenerRef listener;
};

// What tells one version of a resource from another.
static std::string validatorOf(WKURLResponseRef response)
{
    for (const char* name : {"ETag", "Last-Modified"}) {
        std::string value = takeString(WKURLResponseCopyHTTPHeaderField(response, name));
        if (!value.empty())
            return value;
    }
    return {};
}

static void decideDestination(WKDownloadRef, WKURLResponseRef response, WKStringRef name, WKDownloadListenerRef listener, const void* info)
{
    Download* download = static_cast<Download*>(const_cast<void*>(info));
    if (download->offset >= 0) {
        download->partial = WKURLResponseHTTPStatusCode(response) == 206;
        download->path = download->target + ".reef-part";
        WKDownloadListenerChooseDestination(listener, download->path.c_str(), true);
        return;
    }
    download->validator = validatorOf(response);
    if (!download->client || !download->client->destination) {
        WKDownloadListenerChooseDestination(listener, nullptr, false);
        return;
    }
    const std::string url = takeUrl(WKURLResponseCopyURL(response));
    const std::string type = takeString(WKURLResponseCopyMIMEType(response));
    const std::string suggested = toString(name);
    download->client->destination(download->context, url.c_str(), type.c_str(), WKURLResponseGetExpectedContentLength(response),
        suggested.c_str(), new Destination{download, listener});
}

static Download* createDownload(WKDownloadRef ref)
{
    Download* download = new Download;
    WKRetain(ref);
    download->download = ref;
    if (WKURLRequestRef request = WKDownloadCopyRequest(ref)) {
        download->url = takeUrl(WKURLRequestCopyURL(request));
        WKRelease(request);
    }
    const WKDownloadDelegateV0 delegate = {
        {0, download}, willRedirect, didReceiveDownloadChallenge, decideDestination, didWriteData, didFinish, didFail,
    };
    WKDownloadSetDelegate(ref, &delegate.base);
    return download;
}

void actionBecameDownload(WKPageRef, WKNavigationActionRef, WKDownloadRef download, const void* info)
{
    View* view = viewOf(info);
    const auto function = view->client->at<void (*)(void*, int, Download*)>(0xa0);
    if (!function) {
        WKDownloadCancel(download, nullptr, nullptr);
        return;
    }
    function(view->context, 1, createDownload(download));
}

void responseBecameDownload(WKPageRef, WKNavigationResponseRef, WKDownloadRef download, const void* info)
{
    View* view = viewOf(info);
    const auto function = view->client->at<void (*)(void*, int, Download*)>(0xa0);
    if (!function) {
        WKDownloadCancel(download, nullptr, nullptr);
        return;
    }
    function(view->context, 2, createDownload(download));
}

using DownloadFunction = void (*)(void* context, Download* download);

static void downloadStarted(WKDownloadRef ref, void* context)
{
    Callback<DownloadFunction>* callback = static_cast<Callback<DownloadFunction>*>(context);
    callback->function(callback->context, ref ? createDownload(ref) : nullptr);
    delete callback;
}

struct Resuming
{
    DownloadFunction function;
    void* context;
    std::string path;
    int64_t offset;
    std::string validator;
};

static void downloadResumed(WKDownloadRef ref, void* context)
{
    Resuming* resuming = static_cast<Resuming*>(context);
    Download* download = nullptr;
    if (ref) {
        download = createDownload(ref);
        download->offset = resuming->offset;
        download->target = resuming->path;
        download->validator = resuming->validator;
    }
    resuming->function(resuming->context, download);
    delete resuming;
}

using KeptFunction = void (*)(void* context, const char* url, const char* path, int64_t size, const char* validator);

struct Kept
{
    std::string url;
    std::string path;
    std::string validator;
    bool partial;
    std::string written;
    KeptFunction function;
    void* context;
};

static void downloadKept(WKDataRef, const void* context)
{
    Kept* kept = static_cast<Kept*>(const_cast<void*>(context));
    const bool moved = !kept->partial ? rename(kept->written.c_str(), kept->path.c_str()) == 0 : append(kept->written, kept->path);
    struct stat status;
    if (moved && stat(kept->path.c_str(), &status) == 0 && status.st_size > 0) {
        kept->function(kept->context, kept->url.c_str(), kept->path.c_str(), status.st_size, kept->validator.c_str());
    } else {
        unlink(kept->written.c_str());
        kept->function(kept->context, nullptr, nullptr, -1, nullptr);
    }
    delete kept;
}

// 94: stops a download and keeps what it has written
void keepDownload(Download* download, KeptFunction function, void* context)
{
    const std::string path = download->path;
    static unsigned s_count;
    const std::string written = path.empty() ? std::string()
        : path + ".reef-kept-" + std::to_string(getpid()) + "-" + std::to_string(++s_count);
    if (written.empty() || link(path.c_str(), written.c_str()) != 0) {
        WKDownloadCancel(download->download, nullptr, nullptr);
        function(context, nullptr, nullptr, -1, nullptr);
        return;
    }
    WKDownloadCancel(download->download, new Kept{download->url, download->offset >= 0 ? download->target : path, download->validator,
        download->partial, written, function, context}, downloadKept);
}

// 65
void releaseDownload(Download* download)
{
    if (!download)
        return;
    WKDownloadSetDelegate(download->download, nullptr);
    WKRelease(download->download);
    delete download;
}

// 60
void startDownload(View* view, const char* text, DownloadFunction function, void* context)
{
    WKURLRef url = makeUrl(text);
    if (!url) {
        function(context, nullptr);
        return;
    }
    WKURLRequestRef request = WKURLRequestCreateWithWKURL(url);
    WKPageDownloadRequest(view->page, request, new Callback<DownloadFunction>{function, context}, downloadStarted);
    WKRelease(request);
    WKRelease(url);
}

// 95
void resumeDownload(View* view, const char* text, const char* path, int64_t offset, const char* validator, DownloadFunction function,
                    void* context)
{
    WKURLRef url = makeUrl(text);
    if (!url || !path || offset <= 0) {
        if (url)
            WKRelease(url);
        function(context, nullptr);
        return;
    }
    WKURLRequestRef request = WKURLRequestCreateWithWKURL(url);
    const std::string range = "bytes=" + std::to_string(offset) + "-";
    WKURLRequestRef ranged = WKURLRequestCopySettingHTTPHeaderField(request, "Range", range.c_str());
    if (validator && *validator) {
        WKURLRequestRef checked = WKURLRequestCopySettingHTTPHeaderField(ranged, "If-Range", validator);
        WKRelease(ranged);
        ranged = checked;
    }
    WKPageDownloadRequest(view->page, ranged, new Resuming{function, context, path, offset, validator ? validator : ""}, downloadResumed);
    WKRelease(ranged);
    WKRelease(request);
    WKRelease(url);
}

// 61
void setDownloadClient(Download* download, const DownloadClient* client, void* context)
{
    download->client = client;
    download->context = context;
}

// 62
const char* downloadUrl(Download* download) { return download->url.c_str(); }
// 63
void cancelDownload(Download* download) { WKDownloadCancel(download->download, nullptr, nullptr); }
// 66
void answerRedirect(WKDownloadListenerRef listener, int allow) { WKDownloadListenerAllowRedirect(listener, allow != 0); }

// 64
void chooseDestination(Destination* destination, const char* path, int overwrite)
{
    destination->download->path = path ? path : "";
    WKDownloadListenerChooseDestination(destination->listener, path, overwrite != 0);
    delete destination;
}

} // namespace cutefish::reef::web

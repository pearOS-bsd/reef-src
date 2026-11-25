// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the library exports: a version and 100 functions, in this order.

#include "web.h"

#include <WebKit/WKDownloadDelegate.h>

namespace cutefish::reef::web {

struct Cookie;
struct Destination;
struct Download;
struct DownloadClient;
struct DragInfo;
struct Frame;
struct KeyEvent;
struct MouseEvent;
struct Pixels;
struct Record;
struct Screen;
struct ScriptDialog;
struct ViewOptions;
struct WheelEvent;

using CookiesFunction = void (*)(void* context, const Cookie* cookies, int count);
using DownloadFunction = void (*)(void* context, Download* download);
using FilterFunction = void (*)(void* context, Filter* filter, uint32_t result);
using KeptFunction = void (*)(void* context, const char* url, const char* path, int64_t size, const char* validator);
using NamesFunction = void (*)(void* context, const char* const* names, int64_t count);
using RecordsFunction = void (*)(void* context, const Record* records, int64_t count);

int isAvailable();
int descriptor();
double iterate();
UserContent* createUserContent();
void releaseUserContent(UserContent* content);
void addUserScript(UserContent* content, const char* source, int atStart, int mainFrameOnly);
void removeUserScripts(UserContent* content);
void addMessageHandler(UserContent* content, const char* name, MessageFunction function, void* context);
void removeMessageHandlers(UserContent* content);
void answerMessage(WKCompletionListenerRef listener, const char* json, const char* error);
DataStore* defaultDataStore();
DataStore* createDataStore();
void releaseDataStore(DataStore* store);
void clearData(DataStore* store, uint32_t kinds, DoneFunction function, void* context);
void cookies(DataStore* store, CookiesFunction function, void* context);
void setCookie(DataStore* store, const Cookie* cookie, DoneFunction function, void* context);
void deleteCookie(DataStore* store, const Cookie* cookie, DoneFunction function, void* context);
View* createView(const ViewOptions* options);
void releaseView(View* view);
void setSize(View* view, WKSize size);
void setScale(View* view, double scale);
void setState(View* view, uint32_t state);
void setUserAgent(View* view, const char* agent);
void setZoom(View* view, double zoom);
void loadUrl(View* view, const char* text);
void loadHtml(View* view, const char* html, const char* base);
void loadData(View* view, const unsigned char* bytes, int64_t size, const char* type, const char* encoding, const char* base);
void loadFile(View* view, const char* file, const char* directory);
void reload(View* view, int fromOrigin);
void stopLoading(View* view);
void goBack(View* view);
void goForward(View* view);
const char* title(View* view);
const char* url(View* view);
int isLoading(View* view);
double progress(View* view);
int canGoBack(View* view);
int canGoForward(View* view);
void evaluate(View* view, const char* script, ScriptFunction function, void* context);
void execute(View* view, const char* command);
void sendMouse(View* view, const MouseEvent* event);
void sendWheel(View* view, const WheelEvent* event);
void sendKey(View* view, const KeyEvent* event);
void setComposition(View* view, const char* text, int cursor);
void confirmComposition(View* view, const char* text);
void cancelComposition(View* view);
void popupChosen(View* view, int index);
void menuChosen(View* view, int index);
int pixels(View* view, Pixels* result);
void frameDisplayed(View* view);
void taskResponse(const void* task, const char* type, int64_t length, const char* encoding, int status);
void taskData(const void* task, const void* bytes, int64_t size);
void taskFinish(const void* task);
void taskFail(const void* task, const char* domain, int code, const char* description);
void answerOpenPanel(WKOpenPanelResultListenerRef listener, const char* const* urls, int count);
void answerPolicy(WKFramePolicyListenerRef listener, int policy);
void answerScriptDialog(ScriptDialog* dialog, const char* text);
void setPasteboard(const Pasteboard* pasteboard);
void answerAuthentication(WKAuthenticationDecisionListenerRef listener, int disposition, const char* user, const char* password, int persistence);
void startDownload(View* view, const char* text, DownloadFunction function, void* context);
void setDownloadClient(Download* download, const DownloadClient* client, void* context);
const char* downloadUrl(Download* download);
void cancelDownload(Download* download);
void chooseDestination(Destination* destination, const char* path, int overwrite);
void releaseDownload(Download* download);
void answerRedirect(WKDownloadListenerRef listener, int allow);
void find(View* view, const char* text, uint32_t flags);
int history(View* view, const HistoryItem** items, int* back, int* forward);
void goToItem(View* view, int index);
FilterStore* createFilterStore(const char* path);
void releaseFilterStore(FilterStore* store);
void compileFilter(FilterStore* store, const char* identifier, const char* json, FilterFunction function, void* context);
void lookUpFilter(FilterStore* store, const char* identifier, FilterFunction function, void* context);
void removeFilter(FilterStore* store, const char* identifier, FilterFunction function, void* context);
void filterIdentifiers(FilterStore* store, NamesFunction function, void* context);
void releaseFilter(Filter* filter);
void setFilters(UserContent* content, Filter* const* filters, int count);
void setObscuredInsets(View* view, double top, double left, double bottom, double right);
void setDark(View* view, int dark);
void setDisplaySync(void (*startSync)(void*), void (*stopSync)(void*), void* context);
void displaySyncFired();
int currentFrame(View* view, Frame* frame);
void setSandbox(int enabled, const char* path);
void setApplication(const char* identifier);
void addUserScriptInWorld(UserContent* content, const char* source, int atStart, int mainFrameOnly, const char* world);
void addMessageHandlerInWorld(UserContent* content, const char* name, const char* world, MessageFunction function, void* context);
void evaluateInWorld(View* view, const char* script, const char* world, ScriptFunction function, void* context);
void callAsync(View* view, const char* body, const char* arguments, const char* world, ScriptFunction function, void* context);
void records(DataStore* store, uint32_t kinds, RecordsFunction function, void* context);
void removeRecords(DataStore* store, uint32_t kinds, const char* const* names, int count, DoneFunction function, void* context);
void drag(View* view, int kind, const DragInfo* info, double x, double y, uint32_t operations);
uint32_t dragOperation(View* view);
void dragSourceEnded(View* view, WKPoint point, uint32_t operation);
void keepDownload(Download* download, KeptFunction function, void* context);
void resumeDownload(View* view, const char* text, const char* path, int64_t offset, const char* validator, DownloadFunction function, void* context);
void setSpellChecker(const SpellChecker* checker);
void setContinuousSpellChecking(int enabled);
void setScreens(const Screen* screens, int count, uint32_t primary);
void drawPdf(View* view, int hasRect, double x, double y, double width, double height, DataFunction function, void* context);
void printPdf(View* view, double width, double height, double scale, DataFunction function, void* context);

struct Interface
{
    uintptr_t version;
    void* functions[100];
};

static const Interface s_interface = {
    1,
    {
        reinterpret_cast<void*>(isAvailable),
        reinterpret_cast<void*>(descriptor),
        reinterpret_cast<void*>(iterate),
        reinterpret_cast<void*>(createUserContent),
        reinterpret_cast<void*>(releaseUserContent),
        reinterpret_cast<void*>(addUserScript),
        reinterpret_cast<void*>(removeUserScripts),
        reinterpret_cast<void*>(addMessageHandler),
        reinterpret_cast<void*>(removeMessageHandlers),
        reinterpret_cast<void*>(answerMessage),
        reinterpret_cast<void*>(defaultDataStore),
        reinterpret_cast<void*>(createDataStore),
        reinterpret_cast<void*>(releaseDataStore),
        reinterpret_cast<void*>(clearData),
        reinterpret_cast<void*>(cookies),
        reinterpret_cast<void*>(setCookie),
        reinterpret_cast<void*>(deleteCookie),
        reinterpret_cast<void*>(createView),
        reinterpret_cast<void*>(releaseView),
        reinterpret_cast<void*>(setSize),
        reinterpret_cast<void*>(setScale),
        reinterpret_cast<void*>(setState),
        reinterpret_cast<void*>(setUserAgent),
        reinterpret_cast<void*>(setZoom),
        reinterpret_cast<void*>(loadUrl),
        reinterpret_cast<void*>(loadHtml),
        reinterpret_cast<void*>(loadData),
        reinterpret_cast<void*>(loadFile),
        reinterpret_cast<void*>(reload),
        reinterpret_cast<void*>(stopLoading),
        reinterpret_cast<void*>(goBack),
        reinterpret_cast<void*>(goForward),
        reinterpret_cast<void*>(title),
        reinterpret_cast<void*>(url),
        reinterpret_cast<void*>(isLoading),
        reinterpret_cast<void*>(progress),
        reinterpret_cast<void*>(canGoBack),
        reinterpret_cast<void*>(canGoForward),
        reinterpret_cast<void*>(evaluate),
        reinterpret_cast<void*>(execute),
        reinterpret_cast<void*>(sendMouse),
        reinterpret_cast<void*>(sendWheel),
        reinterpret_cast<void*>(sendKey),
        reinterpret_cast<void*>(setComposition),
        reinterpret_cast<void*>(confirmComposition),
        reinterpret_cast<void*>(cancelComposition),
        reinterpret_cast<void*>(popupChosen),
        reinterpret_cast<void*>(menuChosen),
        reinterpret_cast<void*>(pixels),
        reinterpret_cast<void*>(frameDisplayed),
        reinterpret_cast<void*>(taskResponse),
        reinterpret_cast<void*>(taskData),
        reinterpret_cast<void*>(taskFinish),
        reinterpret_cast<void*>(taskFail),
        reinterpret_cast<void*>(answerOpenPanel),
        reinterpret_cast<void*>(answerPolicy),
        reinterpret_cast<void*>(answerScriptDialog),
        reinterpret_cast<void*>(setPasteboard),
        reinterpret_cast<void*>(answerAuthentication),
        reinterpret_cast<void*>(startDownload),
        reinterpret_cast<void*>(setDownloadClient),
        reinterpret_cast<void*>(downloadUrl),
        reinterpret_cast<void*>(cancelDownload),
        reinterpret_cast<void*>(releaseDownload),
        reinterpret_cast<void*>(answerRedirect),
        reinterpret_cast<void*>(chooseDestination),
        reinterpret_cast<void*>(find),
        reinterpret_cast<void*>(history),
        reinterpret_cast<void*>(goToItem),
        reinterpret_cast<void*>(createFilterStore),
        reinterpret_cast<void*>(releaseFilterStore),
        reinterpret_cast<void*>(compileFilter),
        reinterpret_cast<void*>(lookUpFilter),
        reinterpret_cast<void*>(removeFilter),
        reinterpret_cast<void*>(filterIdentifiers),
        reinterpret_cast<void*>(releaseFilter),
        reinterpret_cast<void*>(setFilters),
        reinterpret_cast<void*>(setObscuredInsets),
        reinterpret_cast<void*>(setDark),
        reinterpret_cast<void*>(setDisplaySync),
        reinterpret_cast<void*>(displaySyncFired),
        reinterpret_cast<void*>(currentFrame),
        reinterpret_cast<void*>(setSandbox),
        reinterpret_cast<void*>(setApplication),
        reinterpret_cast<void*>(addUserScriptInWorld),
        reinterpret_cast<void*>(addMessageHandlerInWorld),
        reinterpret_cast<void*>(evaluateInWorld),
        reinterpret_cast<void*>(callAsync),
        reinterpret_cast<void*>(records),
        reinterpret_cast<void*>(removeRecords),
        reinterpret_cast<void*>(drag),
        reinterpret_cast<void*>(dragOperation),
        reinterpret_cast<void*>(dragSourceEnded),
        reinterpret_cast<void*>(keepDownload),
        reinterpret_cast<void*>(resumeDownload),
        reinterpret_cast<void*>(setSpellChecker),
        reinterpret_cast<void*>(setContinuousSpellChecking),
        reinterpret_cast<void*>(setScreens),
        reinterpret_cast<void*>(drawPdf),
        reinterpret_cast<void*>(printPdf),
    },
};

} // namespace cutefish::reef::web

extern "C" __attribute__((visibility("default"))) const void* reef_webkit_host_interface(int version)
{
    return version == 1 ? &cutefish::reef::web::s_interface : nullptr;
}

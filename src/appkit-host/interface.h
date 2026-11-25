// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The functions of the table, in its order.

#pragma once

#include <cstddef>
#include <cstdint>
#include <sys/types.h>

struct reef_ui_audio_input;
struct reef_ui_audio_output;
struct reef_ui_callbacks;
struct reef_ui_camera;
struct reef_ui_camera_info;
struct reef_ui_dma_buf;
struct reef_ui_drop_offer;
struct reef_ui_face;
struct reef_ui_face_axis;
struct reef_ui_face_metrics;
struct reef_ui_face_query;
struct reef_ui_feature;
struct reef_ui_file_chooser;
struct reef_ui_file_chooser_request;
struct reef_ui_frame_clock;
struct reef_ui_gc;
struct reef_ui_gl_context;
struct reef_ui_gl_surface;
struct reef_ui_glass;
struct reef_ui_glyph;
struct reef_ui_image;
struct reef_ui_menu;
struct reef_ui_menu_item;
struct reef_ui_mime_data;
struct reef_ui_now_playing;
struct reef_ui_overlay;
struct reef_ui_paragraph;
struct reef_ui_paragraph_line;
struct reef_ui_paragraph_style;
struct reef_ui_path_element;
struct reef_ui_pdf_document;
struct reef_ui_pdf_outline_item;
struct reef_ui_plane;
struct reef_ui_scroller;
struct reef_ui_scroller_callbacks;
struct reef_ui_sound;
struct reef_ui_status_item;
struct reef_ui_text;
struct reef_ui_text_run;
struct reef_ui_video_decoder;
struct reef_ui_video_frame;
struct reef_ui_window;

namespace cutefish::reef {

int hostInitialize(int argc, const char* const* argv, const char* identifier, const reef_ui_callbacks* given);
int hostLoopDescriptor();
void hostLoopDispatch();
bool hostIsHeadless();
void hostMainScreenSize(double* width, double* height);
double hostMainScale();
const char* systemFontFamily();
bool hostIsDark();
uint32_t hostHighlightColor();
uint32_t hostTextColor();
reef_ui_window* hostWindowCreate(void* context, double width, double height, int flags, reef_ui_window* parent, double x, double y);
void hostWindowDestroy(reef_ui_window* window);
void hostWindowSetTitle(reef_ui_window* window, const char* title);
void hostWindowResize(reef_ui_window* window, double width, double height);
void hostWindowSetMinimumSize(reef_ui_window* window, double width, double height);
void hostWindowSetMaximumSize(reef_ui_window* window, double width, double height);
void hostWindowShow(reef_ui_window* window);
void hostWindowRequestPaint(reef_ui_window* window);
void hostWindowSize(reef_ui_window* window, double* width, double* height);
double hostWindowScale(reef_ui_window* window);
int hostWindowIsActive(reef_ui_window* window);
void hostWindowActivate(reef_ui_window* window);
void hostWindowStartMove(reef_ui_window* window);
void hostWindowMinimize(reef_ui_window* window);
void hostWindowHideApplication(reef_ui_window* window);
void hostWindowSetMaximized(reef_ui_window* window, int maximized);
void hostWindowSetCursor(reef_ui_window* window, int cursor);
void hostWindowSetTextInput(reef_ui_window* window, int accepts, double x, double y, double width, double height);
int hostWindowSavePng(reef_ui_window* window, const char* path);
void hostTestPointer(reef_ui_window* window, int type, double x, double y, int button);
void hostTestKey(reef_ui_window* window, unsigned key, unsigned modifiers);
void hostTestText(reef_ui_window* window, const char* text);
void hostTestScale(reef_ui_window* window, double scale);
void gcSave(reef_ui_gc* gc);
void gcRestore(reef_ui_gc* gc);
void gcConcat(reef_ui_gc* gc, const double* matrix);
void gcSetTransform(reef_ui_gc* gc, const double* matrix);
void gcTransform(reef_ui_gc* gc, double* matrix);
double gcDeviceScale(reef_ui_gc* gc);
void gcClipToRect(reef_ui_gc* gc, double x, double y, double width, double height);
void gcClipToPath(reef_ui_gc* gc, const reef_ui_path_element* elements, unsigned count, int evenOdd);
void gcResetClip(reef_ui_gc* gc);
void gcClipBounds(reef_ui_gc* gc, double* rect);
void gcSetAlpha(reef_ui_gc* gc, double alpha);
void gcSetComposite(reef_ui_gc* gc, int composite);
void gcSetAntialias(reef_ui_gc* gc, int antialias);
void gcSetSmoothing(reef_ui_gc* gc, int smoothing);
void gcSetFillColor(reef_ui_gc* gc, double red, double green, double blue, double alpha);
void gcSetStrokeColor(reef_ui_gc* gc, double red, double green, double blue, double alpha);
void gcSetLineWidth(reef_ui_gc* gc, double width);
void gcSetLineCap(reef_ui_gc* gc, int cap);
void gcSetLineJoin(reef_ui_gc* gc, int join);
void gcSetMiterLimit(reef_ui_gc* gc, double limit);
void gcSetLineDash(reef_ui_gc* gc, const double* lengths, unsigned count, double offset);
void gcSetShadow(reef_ui_gc* gc, double dx, double dy, double blur, double red, double green, double blue, double alpha);
void gcFillPath(reef_ui_gc* gc, const reef_ui_path_element* elements, unsigned count, int evenOdd);
void gcStrokePath(reef_ui_gc* gc, const reef_ui_path_element* elements, unsigned count);
void gcFillRect(reef_ui_gc* gc, double x, double y, double width, double height);
void gcStrokeRect(reef_ui_gc* gc, double x, double y, double width, double height);
void gcClearRect(reef_ui_gc* gc, double x, double y, double width, double height);
void gcFillLinearGradient(
                          reef_ui_gc* gc, const double* offsets, const double* colors, unsigned count, double x0, double y0, double x1, double y1, int extend);
void gcFillRadialGradient(
                          reef_ui_gc* gc, const double* offsets, const double* colors, unsigned count, double x0, double y0, double r0, double x1, double y1, double r1, int extend);
void gcDrawPixels(
                  reef_ui_gc* gc, const void* pixels, int width, int height, size_t stride, int format, double x, double y, double drawnWidth, double drawnHeight, int mirrored);
reef_ui_image* imageCreate(const void* pixels, int width, int height, size_t stride);
void imageDestroy(reef_ui_image* image);
int gcDrawImage(reef_ui_gc* gc, reef_ui_image* image, double x, double y, double width, double height, int mirrored);
void gcBeginGroup(reef_ui_gc* gc);
void gcEndGroup(reef_ui_gc* gc);
void gcDrawText(reef_ui_gc* gc, reef_ui_text* text, double x, double y);
reef_ui_gc* gcCreateForPixels(void* pixels, int width, int height, size_t stride, int format);
void gcDestroy(reef_ui_gc* gc);
reef_ui_text* textCreate(
                         const char* text, const char* family, double size, int bold, int italic, double scale, double red, double green, double blue, double alpha, int kerned);
void textDestroy(reef_ui_text* text);
void textMetrics(reef_ui_text* text, double* width, double* ascent, double* descent);
double textOffsetToX(reef_ui_text* text, size_t offset);
size_t textXToOffset(reef_ui_text* text, double x);
void textFontMetrics(const char* family, double size, int bold, int italic, double scale, double* ascent, double* descent, double* leading);
reef_ui_menu* hostMenuCreate();
void hostMenuDestroy(reef_ui_menu* menu);
void hostMenuSetItems(reef_ui_menu* menu, const reef_ui_menu_item* items, unsigned count);
void hostMenuSetMain(reef_ui_menu* menu);
void* pixelsDecode(const char* path, const void* bytes, size_t size, int* width, int* height);
void* iconPixels(const char* name, int size, int* width, int* height);
void pixelsFree(void* pixels);
int hostMimeTypeForFile(const char* path, char* buffer, int size);
int hostMimeIconName(const char* type, int generic, char* buffer, int size);
int hostDefaultApplication(const char* type, char* buffer, int size);
int hostLaunchApplication(const char* path, const char* const* arguments, int count);
void hostClipboardSetText(const char* text);
void hostClipboardRequestText(void (*callback)(void* context, const char* text), void* context);
reef_ui_paragraph* paragraphCreate(const char* text, const reef_ui_text_run* runs, unsigned count, const reef_ui_paragraph_style* style);
void paragraphDestroy(reef_ui_paragraph* paragraph);
unsigned paragraphLineCount(reef_ui_paragraph* paragraph);
void paragraphLine(reef_ui_paragraph* paragraph, unsigned index, reef_ui_paragraph_line* line);
double paragraphOffsetToX(reef_ui_paragraph* paragraph, unsigned index, size_t offset);
size_t paragraphXToOffset(reef_ui_paragraph* paragraph, unsigned index, double x);
void gcDrawParagraph(reef_ui_gc* gc, reef_ui_paragraph* paragraph, double x, double y);
int textFontFamilies(char* buffer, int size);
void hostWindowSetToolTip(reef_ui_window* window, double x, double y, double width, double height, const char* text);
reef_ui_file_chooser* hostFileChooserOpen(reef_ui_window* window, const reef_ui_file_chooser_request* request,
                                          void (*callback)(void* context, const char* const* paths, unsigned count, const char* const* answers),
                                          void* context);
void hostFileChooserDestroy(reef_ui_file_chooser* chooser);
void hostTestFileChooserAnswer(const char* const* paths, unsigned count, const char* const* options, unsigned optionCount);
int hostClipboardSetData(const reef_ui_mime_data* items, unsigned count);
long hostClipboardChanges();
int hostClipboardTypes(char* buffer, int size);
int hostClipboardOwnsData();
void hostClipboardRequestData(const char* type, void (*callback)(void* context, const void* bytes, size_t size, int found), void* context);
int hostDropOfferTypes(reef_ui_drop_offer* offer, char* buffer, int size);
void hostDropOfferRead(reef_ui_drop_offer* offer, const char* type,
                       void (*callback)(void* context, const void* bytes, size_t size, int read), void* context);
int hostWindowStartDrag(reef_ui_window* window, const reef_ui_mime_data* items, unsigned count, unsigned actions, const void* pixels,
                        int width, int height, double scale, double hotX, double hotY, void (*finished)(void* context, unsigned action),
                        void* context);
ssize_t hostTestDragData(const char* type, void* buffer, size_t size);
unsigned hostTestDragActions();
unsigned hostWindowTestDrop(reef_ui_window* window, double x, double y, const reef_ui_mime_data* items, unsigned count, unsigned actions);
reef_ui_status_item* hostStatusItemCreate(void* context);
void hostStatusItemDestroy(reef_ui_status_item* item);
void hostStatusItemSetIcon(reef_ui_status_item* item, const uint32_t* pixels, int width, int height);
void hostStatusItemSetTitle(reef_ui_status_item* item, const char* title);
void hostStatusItemSetToolTip(reef_ui_status_item* item, const char* toolTip);
void hostStatusItemSetMenu(reef_ui_status_item* item, reef_ui_menu* menu);
void* hostWindowGrab(reef_ui_window* window, int* width, int* height);
void* pixelsEncodePng(const void* pixels, int width, int height, size_t stride, size_t* size);
reef_ui_face* faceMatch(const reef_ui_face_query* query, int* exact);
int faceList(const reef_ui_face_query* query, char* buffer, int size);
reef_ui_face* faceFromFile(const char* path, int index);
reef_ui_face* faceFromData(const void* data, size_t size, int index);
void faceRetain(reef_ui_face* face);
void faceRelease(reef_ui_face* face);
void faceMetrics(reef_ui_face* face, reef_ui_face_metrics* metrics);
int faceName(reef_ui_face* face, int id, char* buffer, int size);
void faceGlyphs(reef_ui_face* face, const uint32_t* characters, uint32_t count, uint32_t* glyphs);
void faceAdvances(reef_ui_face* face, const uint32_t* glyphs, uint32_t count, int vertical, double* advances);
void faceBounds(reef_ui_face* face, const uint32_t* glyphs, uint32_t count, double* bounds);
uint32_t faceOutline(reef_ui_face* face, uint32_t glyph, reef_ui_path_element* elements, uint32_t capacity);
int faceGlyphName(reef_ui_face* face, uint32_t glyph, char* buffer, int size);
uint32_t faceGlyphNamed(reef_ui_face* face, const char* name);
size_t faceTable(reef_ui_face* face, uint32_t tag, void* buffer, size_t size);
uint32_t faceTables(reef_ui_face* face, uint32_t* tags, uint32_t capacity);
uint32_t faceCharacterRanges(reef_ui_face* face, uint32_t* ranges, uint32_t capacity);
int faceLanguages(reef_ui_face* face, char* buffer, int size);
reef_ui_face* faceFallback(reef_ui_face* face, uint32_t character, const char* language);
uint32_t faceShape(
                   reef_ui_face* face, const uint16_t* text, uint32_t length, uint32_t start, uint32_t count, double size, int flags, uint32_t script, const char* language, const reef_ui_feature* features, uint32_t featureCount, reef_ui_glyph* glyphs, uint32_t capacity);
uint32_t faceAxes(reef_ui_face* face, reef_ui_face_axis* axes, uint32_t capacity);
uint32_t faceCoordinates(reef_ui_face* face, uint32_t* tags, double* values, uint32_t capacity);
reef_ui_face* faceWithVariations(reef_ui_face* face, const uint32_t* tags, const double* values, uint32_t count);
int fontRegister(const char* path);
int fontUnregister(const char* path);
const char* hostSystemFixedFontFamily();
void gcDrawGlyphs(
                  reef_ui_gc* gc, reef_ui_face* face, const double* matrix, const uint32_t* glyphs, const double* positions, uint32_t count, int flags);
void gcFillColor(reef_ui_gc* gc, double* color);
void gcBeginMaskedGroup(reef_ui_gc* gc);
void gcEndGroupMaskedByPixels(
                              reef_ui_gc* gc, const void* pixels, int width, int height, size_t stride, double x, double y, double drawnWidth, double drawnHeight, const double* matrix, int mirrored);
void hostWindowSetPosition(reef_ui_window* window, double x, double y);
int hostWindowPosition(reef_ui_window* window, double* x, double* y);
void hostMainVisibleFrame(double* frame);
reef_ui_sound* soundCreate(const char* path, const void* bytes, size_t size);
void soundDestroy(reef_ui_sound* sound);
int soundPlay(reef_ui_sound* sound, double volume, int loops, void (*finished)(void*, int), void* context);
void soundStop(reef_ui_sound* sound);
void soundPause(reef_ui_sound* sound);
void soundResume(reef_ui_sound* sound);
double soundDuration(reef_ui_sound* sound);
double soundPosition(reef_ui_sound* sound);
void soundSetPosition(reef_ui_sound* sound, double seconds);
void soundSetVolume(reef_ui_sound* sound, double volume);
int soundIsPlaying(reef_ui_sound* sound);
int soundPlayNamed(const char* name);
unsigned hostPathStroked(
                         const reef_ui_path_element* elements, unsigned count, double width, int cap, int join, double miterLimit, reef_ui_path_element* out, unsigned capacity);
unsigned hostPathCombine(
                         const reef_ui_path_element* first, unsigned firstCount, int firstEvenOdd, const reef_ui_path_element* second, unsigned secondCount, int secondEvenOdd, int operation, reef_ui_path_element* out, unsigned capacity);
reef_ui_gc* pdfCreate(const reef_ui_pdf_document* document);
void pdfBeginPage(reef_ui_gc* gc, const double* mediaBox, const double* boxes);
void pdfEndPage(reef_ui_gc* gc);
void pdfAddLink(reef_ui_gc* gc, double x, double y, double width, double height, const char* url, const char* destination);
void pdfAddDestination(reef_ui_gc* gc, const char* name, double x, double y);
void pdfBeginTag(
                 reef_ui_gc* gc, const char* type, const char* actualText, const char* alternateText, const char* title, const char* language);
void pdfEndTag(reef_ui_gc* gc);
void pdfSetOutline(reef_ui_gc* gc, const reef_ui_pdf_outline_item* items, unsigned count);
void pdfSetMetadata(reef_ui_gc* gc, const char* bytes, size_t size);
void pdfClose(reef_ui_gc* gc);
void pdfDestroy(reef_ui_gc* gc);
reef_ui_frame_clock* hostFrameClockCreate(reef_ui_window* window, void (*callback)(void*), void* context);
void hostFrameClockStart(reef_ui_frame_clock* clock);
void hostFrameClockStop(reef_ui_frame_clock* clock);
void hostFrameClockDestroy(reef_ui_frame_clock* clock);
void hostWindowSetPlacement(reef_ui_window* window, int placement);
int hostWindowPlacement(reef_ui_window* window);
int hostStatusItemsSupported();
void hostWindowSetStatusItemIdentifier(reef_ui_window* window, const char* identifier);
void hostWindowSetStatusItemHighlighted(reef_ui_window* window, int highlighted);
void hostWindowStatusItemState(reef_ui_window* window, double* rect, int* dark, uint32_t* color, int* highlighted);
void hostWindowSetUserResizable(reef_ui_window* window, int resizable);
void hostWindowSetShapeFromContent(reef_ui_window* window, int fromContent);
double hostPopupCornerRadius();
int hostWindowSetPopupGlass(reef_ui_window* window, int dark);
void hostWindowHide(reef_ui_window* window);
void hostWindowSetCastsShadow(reef_ui_window* window, int castsShadow);
reef_ui_scroller* hostScrollerCreate(reef_ui_window* window, const reef_ui_scroller_callbacks* callbacks);
void hostScrollerDestroy(reef_ui_scroller* scroller);
int hostScrollerScroll(
                       reef_ui_scroller* scroller, double deltaX, double deltaY, int value120X, int value120Y, int precise, int stop, unsigned time, unsigned modifiers);
void hostScrollerSetElastic(reef_ui_scroller* scroller, int elastic);
void hostScrollerViewMoved(reef_ui_scroller* scroller);
int hostScrollerIsActive(reef_ui_scroller* scroller);
int hostShowInFileManager(const char* const* paths, unsigned count, int select);
void hostWindowRequestPaintRect(reef_ui_window* window, double x, double y, double width, double height);
int hostScreenCount();
int hostScreenGeometry(uint32_t index, double* frame, double* visible, double* scale);
void hostWindowSetAlwaysOnTop(reef_ui_window* window, int onTop);
void hostWindowSetOpacity(reef_ui_window* window, double opacity);
void hostWindowSetFullScreen(reef_ui_window* window, int fullScreen);
void hostWindowSetContext(reef_ui_window* window, void* context);
void hostWindowScroll(reef_ui_window* window, double x, double y, double width, double height, double dx, double dy);
void* symbolPixels(
                   const char* name, int width, int height, const double* rect, double weight, int filled, int margin, int* pixelWidth, int* pixelHeight);
int symbolExists(const char* name);
int symbolBounds(const char* name, double* bounds);
int hostInstanceClaim(const char* identifier, int forward, int count, const char* const* items);
void hostWindowActivateWithToken(reef_ui_window* window, const char* token);
void hostInstanceSetShownInDock(int shown);
reef_ui_audio_output* audioOutputCreate(const char* name, double rate, int channels, int frames);
int audioOutputWrite(reef_ui_audio_output* output, const float* samples, int frames);
uint64_t audioOutputLatency(reef_ui_audio_output* output);
void audioOutputFlush(reef_ui_audio_output* output);
void audioOutputDestroy(reef_ui_audio_output* output);
reef_ui_gl_context* glContextCreate(int core, reef_ui_gl_context* shared);
void glContextDestroy(reef_ui_gl_context* context);
int glContextMakeCurrent(reef_ui_gl_context* context, reef_ui_gl_surface* surface);
void* glProcAddress(const char* name);
reef_ui_gl_surface* glSurfaceCreate(reef_ui_window* window);
void glSurfaceDestroy(reef_ui_gl_surface* surface);
void glSurfaceSetGeometry(reef_ui_gl_surface* surface, double x, double y, double width, double height);
int glSurfaceBeginFrame(reef_ui_gl_surface* surface, reef_ui_gl_context* context, int* width, int* height);
int glSurfaceEndFrame(reef_ui_gl_surface* surface);
int glSurfaceIsOffscreen(reef_ui_gl_surface* surface);
int glSurfaceTakePixels(reef_ui_gl_surface* surface, void* pixels, size_t stride, int width, int height);
void hostWindowSetBackgroundBlur(reef_ui_window* window, int blurred);
void glSurfaceSetOpaque(reef_ui_gl_surface* surface, int opaque);
void hostSetActivationToken(const char* token);
void hostWindowFrameStatistics(reef_ui_window* window, uint64_t* rendered, uint64_t* presented, uint64_t* discarded);
void glSurfaceSetResizeHandler(reef_ui_gl_surface* surface, void (*handler)(void*), void* context);
reef_ui_dma_buf* dmaBufCreate(
                              int width, int height, uint32_t fourcc, uint64_t modifier, int planeCount, const int* fds, const uint32_t* offsets, const uint32_t* strides);
void dmaBufDestroy(reef_ui_dma_buf* dmaBuf);
int gcDrawDmaBuf(reef_ui_gc* gc, reef_ui_dma_buf* dmaBuf, double x, double y, double width, double height, int mirrored);
reef_ui_video_decoder* videoDecoderCreate(uint32_t codec, const void* configuration, size_t length, int* width, int* height, int* depth);
int videoDecoderDecode(reef_ui_video_decoder* decoder, const void* data, size_t length, reef_ui_video_frame** frame);
void videoDecoderDestroy(reef_ui_video_decoder* decoder);
int videoFrameCopy(reef_ui_video_frame* frame, void* const* planes, const size_t* strides, int width, int height);
int videoFrameExport(
                     reef_ui_video_frame* frame, int* count, int* fds, uint32_t* formats, uint32_t* offsets, uint32_t* pitches, uint64_t* modifiers);
reef_ui_video_frame* videoFrameRetain(reef_ui_video_frame* frame);
void videoFrameRelease(reef_ui_video_frame* frame);
unsigned glTextureFromDmaBuf(int width, int height, unsigned fourcc, int descriptor, unsigned offset, unsigned stride, uint64_t modifier);
int videoDecoderSupports(uint32_t codec);
int hostWindowStartWindowDrag(reef_ui_window* window, reef_ui_window* dragged, double offsetX, double offsetY,
                              const reef_ui_mime_data* items, unsigned count, const void* pixels, int width, int height, double scale,
                              double hotX, double hotY, void (*finished)(void* context, unsigned action, int dropped), void* context);
void hostTestWindowDragEnd(unsigned action, int dropped);
int glSurfaceIsReady(reef_ui_gl_surface* surface);
void hostWindowSetFullScreenBars(reef_ui_window* window, double height, int push);
void hostSetDark(int dark);
void hostLanguageChanged();
int hostWindowCursor(reef_ui_window* window, int* changes);
double hostWindowCornerRadius();
void imageUpdate(reef_ui_image* image, const void* pixels, size_t stride, int x, int y, int width, int height);
int gcEndGroupMaskedByImage(
                            reef_ui_gc* gc, reef_ui_image* image, double x, double y, double width, double height, const double* matrix, int mirrored);
reef_ui_plane* hostPlaneCreate(reef_ui_window* window, void* context);
void hostPlaneDestroy(reef_ui_plane* plane);
void hostPlaneSetGeometry(
                          reef_ui_plane* plane, double x, double y, double width, double height, double clipX, double clipY, double clipWidth, double clipHeight, double radius, double opacity, int opaque);
void hostWindowSetPlaneOrder(reef_ui_window* window, reef_ui_plane* const* planes, unsigned count);
void hostPlaneRequestPaint(reef_ui_plane* plane, double x, double y, double width, double height);
void hostPlaneScroll(reef_ui_plane* plane, double x, double y, double width, double height, double dx, double dy);
void glSurfaceSetAbove(reef_ui_gl_surface* surface, int above);
void hostWindowSetSubsurfaceBlur(reef_ui_window* window, double x, double y, double width, double height, double radius);
void gcBlurBackdrop(reef_ui_gc* gc, const reef_ui_path_element* elements, unsigned count, double radius, double saturation);
void hostWindowSetBehindMaterials(reef_ui_window* window, const double* areas, unsigned count);
void hostWindowSetSubsurfaceBlurAreas(reef_ui_window* window, const double* areas, unsigned count);
void gcExcludeRect(reef_ui_gc* gc, double x, double y, double width, double height);
int gcClearBitmap(reef_ui_gc* gc);
void hostWindowSetSubsurfaceBlurFill(reef_ui_window* window, const double* tint, int blend, const double* color);
void hostWindowRequestLayout(reef_ui_window* window);
int gcClipToImage(reef_ui_gc* gc, reef_ui_image* image, double x, double y, double width, double height, int mirrored);
reef_ui_overlay* hostOverlayCreate(
                                   reef_ui_window* window, double x, double y, double width, double height, double duration, double elapsed, const void* pixels, int pixelWidth, int pixelHeight, size_t stride, const double* curve);
int hostOverlayNeedsPixels(reef_ui_overlay* overlay);
void hostOverlaySetPixels(reef_ui_overlay* overlay, const void* pixels, size_t stride);
void hostOverlayDestroy(reef_ui_overlay* overlay);
reef_ui_audio_input* audioInputCreate(const char* name, double rate, int channels, int frames);
int audioInputRead(reef_ui_audio_input* input, float* samples, int frames);
uint64_t audioInputLatency(reef_ui_audio_input* input);
void audioInputDestroy(reef_ui_audio_input* input);
void hostMenuSetDock(reef_ui_menu* menu);
int hostDisplayWaitVblank(const char* name, uint64_t* time);
uint64_t hostDisplayPeriod(const char* name);
int glTextureExportDmaBuf(unsigned texture, int* planes, int* descriptors, int* fourcc, int* offsets, int* strides, uint64_t* modifier);
unsigned glTextureFromDmaBufPlanes(
                                   int width, int height, unsigned fourcc, int planes, const int* descriptors, const unsigned* offsets, const unsigned* strides, uint64_t modifier);
int hostScreenName(uint32_t number, char* buffer, size_t size);
int hostWindowAcceptsTextInput(reef_ui_window* window);
void hostWindowSetCursorImage(
                              reef_ui_window* window, const void* pixels, int width, int height, int scale, double hotSpotX, double hotSpotY);
int hostCameraList(reef_ui_camera_info* cameras, int count);
reef_ui_camera* hostCameraOpen(const char* path, uint32_t pixelFormat, int width, int height, double rate);
int hostCameraRead(reef_ui_camera* camera, void* data, size_t capacity, size_t* size, uint64_t* time, int timeout);
void hostCameraClose(reef_ui_camera* camera);
void hostPlaneSetBackdrop(reef_ui_plane* plane, const double* areas, unsigned count, double blur, double saturation);
void hostWindowSetTransientParent(reef_ui_window* window, reef_ui_window* parent);
void hostPlaneSetGlass(reef_ui_plane* plane, const reef_ui_glass* panes, unsigned count);
void hostPlaneSetScale(reef_ui_plane* plane, double scale);
double hostWindowOutputScale(reef_ui_window* window);
unsigned hostWindowBehindMaterialCount(reef_ui_window* window);
void* iconPixelsScaled(const char* name, int size, int pixelSize, int* width, int* height);
void hostWindowSetLevel(reef_ui_window* window, int level);
int hostCanSendKeys();
int hostSendKey(unsigned key, int pressed);
void hostNowPlayingUpdate(const reef_ui_now_playing* state, void (*command)(int command, double value));
void videoFrameSize(reef_ui_video_frame* frame, int* width, int* height);
void glSurfaceSetBufferScale(reef_ui_gl_surface* surface, double scale);
void dmaBufSetFence(reef_ui_dma_buf* dmaBuf, int fd);
void hostWindowHoldPresentation(reef_ui_window* window, int hold);
int hostWindowIsTransparent(reef_ui_window* window);
int glSurfaceWaitForFrame(reef_ui_gl_surface* surface, int milliseconds);
void* hostImageEncodeJpeg2000(const void* pixels, int width, int height, size_t stride, double quality, size_t* size);
void hostWindowSetInputTransparent(reef_ui_window* window, int transparent);
void hostGpuWorkIssued(const char* name, const char* detail);
void hostGpuWorkMeasured(const char* name, const char* detail, int running, int thread, double duration, double time);
int hostThreadId();

} // namespace cutefish::reef

// LD_PRELOAD: in place of the toolkit's file chooser and of the start of drags: what they are given goes to stdout, and the
// driver gets the objects.
#include <bits/stdc++.h>
#define private public
#include "ui/file_chooser.h"
#include "ui/drag.h"
#include "ui/view.h"
#undef private

#include <cstdio>

using namespace cutefish::ui;

extern "C" {
bool hook_startDragResult = true;
View* hook_view;
DragSource* hook_source;
FileChooser* hook_chooser;
Window* hook_window;
Window* hook_window2;
}

static const char* windowName(Window* window)
{
    return !window ? "null" : window == hook_window ? "window" : window == hook_window2 ? "window2" : "other";
}

static void text(const std::string& s)
{
    putchar('"');
    for (unsigned char c : s) {
        if (c < 32 || c > 126 || c == '"') printf("\\%02x", c);
        else putchar(c);
    }
    putchar('"');
}

bool View::startDrag(DragSource source)
{
    hook_view = this;
    printf("  startDrag actions=%u iconScale=%.17g hot=%.17g,%.17g attached=%s at %.17g,%.17g context=%d data=%d finished=%d ended=%d types:",
           source.actions, source.iconScale, source.hotSpot.x, source.hotSpot.y, windowName(source.attachedWindow), source.attachedOffset.x,
           source.attachedOffset.y, source.context != nullptr, bool(source.data), bool(source.onFinished), bool(source.onEnded));
    for (const std::string& type : source.mimeTypes) { putchar(' '); text(type); }
    printf("\n  icon %dx%d stride=%zu format=%d:", source.icon.width(), source.icon.height(), source.icon.stride(), int(source.icon.format()));
    unsigned hash = 0;
    for (int y = 0; y < source.icon.height(); y++)
        for (int x = 0; x < source.icon.width(); x++)
            hash = hash * 31 + source.icon.pixel(x, y);
    printf(" %08x\n", hash);
    delete hook_source;
    hook_source = nullptr;
    if (hook_startDragResult)
        hook_source = new DragSource(std::move(source));
    return hook_startDragResult;
}

struct FileChooser::Request
{
};

FileChooser::FileChooser(Window* parent)
    : m_parent(parent)
{
    hook_chooser = this;
    printf("  FileChooser(%s)\n", windowName(parent));
}

FileChooser::~FileChooser()
{
    printf("  ~FileChooser\n");
    if (hook_chooser == this)
        hook_chooser = nullptr;
}

void FileChooser::setMode(Mode mode) { printf("  setMode %d\n", int(mode)); }
void FileChooser::setTitle(std::string title) { printf("  setTitle "); text(title); puts(""); }
void FileChooser::setAcceptLabel(std::string label) { printf("  setAcceptLabel "); text(label); puts(""); }
void FileChooser::setCurrentFolder(std::string path) { printf("  setCurrentFolder "); text(path); puts(""); }
void FileChooser::setCurrentFile(std::string path) { printf("  setCurrentFile "); text(path); puts(""); }
void FileChooser::setCurrentFilter(std::string name) { printf("  setCurrentFilter "); text(name); puts(""); }
void FileChooser::setModal(bool modal) { printf("  setModal %d\n", modal); }

void FileChooser::setFilters(std::vector<Filter> filters)
{
    printf("  setFilters %zu\n", filters.size());
    for (const Filter& filter : filters) {
        printf("    "); text(filter.name); printf(" patterns");
        for (const std::string& p : filter.patterns) { putchar(' '); text(p); }
        printf(" types");
        for (const std::string& p : filter.mimeTypes) { putchar(' '); text(p); }
        puts("");
    }
}

void FileChooser::setChoices(std::vector<Choice> choices)
{
    printf("  setChoices %zu\n", choices.size());
    for (const Choice& choice : choices) {
        printf("    "); text(choice.id); putchar(' '); text(choice.label); printf(" selected "); text(choice.selected); printf(" options");
        for (const auto& [id, label] : choice.options) { putchar(' '); text(id); putchar('='); text(label); }
        puts("");
    }
}

void FileChooser::open() { printf("  open accepted=%d rejected=%d\n", bool(onAccepted), bool(onRejected)); }
void FileChooser::close() { puts("  close"); }
bool FileChooser::isOpen() const { return false; }

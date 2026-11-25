// driver orig|ours: drives the drag and file chooser functions of the host table. The rest of the host (the application and the
// windows) is always the published library's; with "ours" the eleven functions are those of mine.so. Needs hooks.so in LD_PRELOAD.
// Everything that can be seen goes to stdout: run.sh compares the two logs.
#include <bits/stdc++.h>
#define private public
#include "dialogs.h"
#include "drag.h"
#include "ui/file_chooser.h"
#include "ui/test_input.h"
#include "ui/drag.h"
#include "ui/view.h"
#undef private

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <link.h>
#include <poll.h>
#include <random>

using namespace cutefish;

static void* table[317];
static bool* isHeadless;
template<typename F> static F* fn(int index) { return reinterpret_cast<F*>(table[index]); }

static bool* startDragResult;
static ui::View** hookView;
static ui::DragSource** hookSource;
static ui::FileChooser** hookChooser;

static std::mt19937 random_(7);
static unsigned pick(unsigned n) { return random_() % n; }

static void text(const void* data, size_t size)
{
    putchar('"');
    if (size > 200) {
        unsigned hash = 0;
        for (size_t i = 0; i < size; i++) hash = hash * 31 + static_cast<const unsigned char*>(data)[i];
        printf("#%zu:%08x", size, hash);
    } else {
        for (size_t i = 0; i < size; i++) {
            const unsigned char c = static_cast<const unsigned char*>(data)[i];
            if (c < 32 || c > 126 || c == '"') printf("\\%02x", c);
            else putchar(c);
        }
    }
    putchar('"');
}
static void text(const std::string& s) { text(s.data(), s.size()); }
static void text(const char* s) { if (s) text(s, strlen(s)); else printf("null"); }

using Types = int(reef_ui_drop_offer*, char*, int);
using Read = void(reef_ui_drop_offer*, const char*, void (*)(void*, const void*, size_t, int), void*);
using StartDrag = int(reef_ui_window*, const reef_ui_mime_data*, unsigned, unsigned, const void*, int, int, double, double, double,
                      void (*)(void*, unsigned), void*);
using StartWindowDrag = int(reef_ui_window*, reef_ui_window*, double, double, const reef_ui_mime_data*, unsigned, const void*, int, int,
                            double, double, double, void (*)(void*, unsigned, int), void*);
using Data = ssize_t(const char*, void*, size_t);
using Actions = unsigned();
using TestDrop = unsigned(reef_ui_window*, double, double, const reef_ui_mime_data*, unsigned, unsigned);
using DragEnd = void(unsigned, int);
using Open = reef_ui_file_chooser*(reef_ui_window*, const reef_ui_file_chooser_request*,
                                   void (*)(void*, const char* const*, unsigned, const char* const*), void*);
using Destroy = void(reef_ui_file_chooser*);
using SetAnswer = void(const char* const*, unsigned, const char* const*, unsigned);

static reef_ui_window* window;
static reef_ui_window* window2;

// ------------------------------------------------------------------ drops

struct Offer : ui::DropOffer
{
    const std::vector<std::string>& mimeTypes() const override { asked++; return types; }
    uint32_t sourceActions() const override { return 7; }
    void read(const std::string& mimeType, std::function<void(std::string data)> done) override
    {
        printf("  read "); text(mimeType); puts("");
        if (later) { pending = std::move(done); return; }
        if (mimeType == "throw") throw std::runtime_error("read");
        done(answer);
    }
    mutable int asked = 0;
    std::vector<std::string> types;
    std::string answer;
    bool later = false;
    std::function<void(std::string)> pending;
};

static void readDone(void* context, const void* bytes, size_t size, int read)
{
    printf("  done context=%ld read=%d size=%zu ", long(context), read, size);
    if (bytes) text(bytes, size); else printf("null");
    puts("");
}

static void offers()
{
    char buffer[4096];
    const auto types = [&](reef_ui_drop_offer* offer, bool null, int size) {
        memset(buffer, '#', sizeof buffer);
        const int result = fn<Types>(107)(offer, null ? nullptr : buffer + 8, size);
        int touched = 0;
        for (int i = 0; i < int(sizeof buffer); i++) if (buffer[i] != '#') touched = i + 1;
        printf("types size=%d null=%d: %d ", size, null, result); text(buffer, touched); puts("");
    };
    for (int size : {-1, 0, 1, 5}) { types(nullptr, true, size); types(nullptr, false, size); }
    std::vector<std::vector<std::string>> lists = {{}, {""}, {"text/plain"}, {"a", "b", "c"}, {"text/plain;charset=utf-8", "text/uri-list", "x"},
        {std::string(14, 'a')}, {std::string(15, 'b')}, {std::string(16, 'c')}, {std::string(29, 'd'), std::string(30, 'e'), std::string(31, 'f')},
        {"with\nline", "", "", "z"}, {std::string("nul\0in", 6), "q"}, {std::string(300, 'g'), std::string(1, 'h')}, {std::string(1000, 'i')}};
    for (int i = 0; i < 40; i++) {
        std::vector<std::string> list;
        for (unsigned n = pick(12); n; n--) list.push_back(std::string(pick(60), char('a' + pick(26))));
        lists.push_back(list);
    }
    for (const auto& list : lists) {
        Offer offer;
        offer.types = list;
        ui::DropOffer* base = &offer;
        int total = 0;
        for (const std::string& type : list) total += type.size() + 1;
        for (int size : {-5, 0, 1, 2, total - 1, total, total + 1, 2000}) { if (size < 4000) types(reinterpret_cast<reef_ui_drop_offer*>(base), false, size); }
        types(reinterpret_cast<reef_ui_drop_offer*>(base), true, 100);
        printf("asked %d\n", offer.asked);
    }
    puts("read");
    Offer offer;
    auto* drop = reinterpret_cast<reef_ui_drop_offer*>(static_cast<ui::DropOffer*>(&offer));
    fn<Read>(108)(nullptr, "text/plain", readDone, (void*)1);
    fn<Read>(108)(drop, nullptr, readDone, (void*)2);
    fn<Read>(108)(nullptr, nullptr, readDone, (void*)3);
    for (const std::string& answer : {std::string(), std::string("x"), std::string("fifteen chars!!"), std::string("sixteen chars!!!"), std::string("a\0b", 3), std::string(100000, 'y')}) {
        offer.answer = answer;
        offer.later = false;
        fn<Read>(108)(drop, "", readDone, (void*)4);
        fn<Read>(108)(drop, "text/plain", readDone, (void*)5);
        fn<Read>(108)(drop, std::string(40, 't').c_str(), readDone, nullptr);
        offer.later = true;
        fn<Read>(108)(drop, "later", readDone, (void*)6);
        puts("  pending");
        auto pending = std::move(offer.pending);
        auto copy = pending;
        pending(answer);
        copy(answer + "2");
        copy(std::string());
    }
    offer.later = false;
    try { fn<Read>(108)(drop, "throw", readDone, (void*)7); } catch (const std::exception& e) { printf("exception %s\n", e.what()); }
}

// ------------------------------------------------------------------ drags

static int provided = 0;
static const char* provideBytes = nullptr;
static size_t provideSize = 0;
static std::string provideStorage;

static const void* provide(void* context, size_t* size)
{
    printf("  provide context=%ld size=%zu\n", long(context), *size);
    provided++;
    switch (long(context) % 8) {
    case 0: *size = 99; return nullptr;
    case 1: *size = 0; return "";
    case 2: *size = 1; return "Q";
    case 3: *size = 15; return "123456789012345";
    case 4: *size = 16; return "1234567890123456";
    case 5: *size = 5; return "ab\0cd";
    case 6: provideStorage.assign(70000, 'p'); *size = provideStorage.size(); return provideStorage.data();
    default: return "never sized";
    }
}
static void release(void* context) { printf("  release context=%ld\n", long(context)); }
static void finished(void* context, unsigned action) { printf("  finished context=%ld action=%u\n", long(context), action); }
static void ended(void* context, unsigned action, int dropped) { printf("  ended context=%ld action=%u dropped=%d\n", long(context), action, dropped); }

static void showData(const char* type)
{
    char buffer[256];
    for (size_t size : {size_t(0), size_t(1), size_t(7), size_t(200)}) {
        memset(buffer, '#', sizeof buffer);
        const ssize_t result = fn<Data>(110)(type, buffer, size);
        printf("  data "); text(type); printf(" size=%zu: %zd ", size, result); text(buffer, 60); puts("");
    }
    printf("  data null buffer: %zd\n", fn<Data>(110)(type, nullptr, 1000));
}

static const char* const typeNames[] = {"text/plain", "text/uri-list", "", "a", "application/octet-stream-of-a-long-name", "text/plain", "b"};

static std::vector<unsigned> pixels(64 * 64);

// RootView::pressed.
static bool& pressed() { return reinterpret_cast<bool*>(window->root)[0x1ec]; }

static void startAndShow(const std::vector<reef_ui_mime_data>& items, unsigned actions, const void* picture, int width, int height, double scale,
                         double hotX, double hotY, bool callback, long context)
{
    pressed() = true;
    const int result = fn<StartDrag>(109)(window, items.data(), items.size(), actions, picture, width, height, scale, hotX, hotY,
                                          callback ? finished : nullptr, (void*)context);
    printf(" result=%d pressed=%d actions=%u view=%d\n", result, pressed(), fn<Actions>(111)(), *hookView == reinterpret_cast<ui::View*>(window->root));
    pressed() = false;
}

static void useSource()
{
    if (!*hookSource)
        return;
    ui::DragSource& source = **hookSource;
    if (source.data) {
        for (const char* type : {"text/plain", "text/uri-list", "", "a", "application/octet-stream-of-a-long-name", "b", "nothing"}) {
            for (int i = 0; i < 2; i++) { printf("  source.data "); text(type); putchar(' '); text(source.data(type)); puts(""); }
        }
    }
    if (source.onFinished) {
        for (unsigned action : {0u, 1u, 2u, 4u, 77u}) source.onFinished(ui::DropAction(action));
        auto copy = source.onFinished;
        copy(ui::CopyAction);
    }
    if (source.onEnded) {
        for (unsigned action : {0u, 1u, 2u, 4u}) { source.onEnded(ui::DropAction(action), true); source.onEnded(ui::DropAction(action), false); }
        auto copy = source.onEnded;
        copy(ui::MoveAction, true);
    }
}

static std::vector<reef_ui_mime_data> randomItems(std::vector<std::string>& keep)
{
    std::vector<reef_ui_mime_data> items(pick(5));
    keep.reserve(64);
    for (reef_ui_mime_data& item : items) {
        memset(&item, 0, sizeof item);
        item.type = pick(8) ? typeNames[pick(7)] : nullptr;
        switch (pick(4)) {
        case 0:
            item.provide = provide;
            item.release = pick(3) ? release : nullptr;
            item.context = (void*)(long)(pick(1000));
            if (pick(2)) { item.bytes = "ignored"; item.size = 7; }
            break;
        case 1:
            // Only with bytes, or ignored.
            item.release = release;
            item.context = (void*)(long)(pick(1000));
            [[fallthrough]];
        case 2: {
            const unsigned sizes[] = {0, 1, 2, 14, 15, 16, 29, 30, 31, 32, 500};
            keep.push_back(std::string(sizes[pick(11)], char('A' + pick(26))));
            item.bytes = keep.back().data();
            item.size = keep.back().size();
            break;
        }
        default:
            item.size = pick(20);
            break;
        }
    }
    return items;
}

static void drags()
{
    for (size_t i = 0; i < pixels.size(); i++) pixels[i] = unsigned(i * 2654435761u);
    printf("actions %u\n", fn<Actions>(111)());
    showData("text/plain");
    showData(nullptr);
    puts("empty drag");
    startAndShow({}, 0, nullptr, 0, 0, 0, 0, 0, true, 1);
    useSource();
    for (int round = 0; round < 250; round++) {
        std::vector<std::string> keep;
        const auto items = randomItems(keep);
        const unsigned actionList[] = {0, 1, 2, 3, 4, 5, 7, 0x12345};
        const unsigned actions = actionList[pick(8)];
        const int sizes[] = {-3, 0, 1, 2, 5, 17, 64};
        const double scales[] = {-1, -0.0, 0, 0.5, 1, 1.5, 2, 3.25, NAN, INFINITY};
        const double places[] = {-4, 0, 0.25, 3, 1e9};
        *startDragResult = pick(5);
        printf("drag %d items=%zu result=%d\n", round, items.size(), *startDragResult);
        startAndShow(items, actions, pick(5) ? pixels.data() : nullptr, sizes[pick(7)], sizes[pick(7)], scales[pick(10)], places[pick(5)],
                     places[pick(5)], pick(4), round);
        // The strings of the items are the caller's: gone by now.
        for (std::string& string : keep) memset(string.data(), '!', string.size());
        if (pick(2)) for (const char* type : {"text/plain", "a", "nothing", ""}) showData(type);
        if (pick(2)) showData(nullptr);
        if (pick(3)) useSource();
    }
    puts("null items");
    startAndShow({}, 1, pixels.data(), 4, 4, 1, 1, 1, false, 3);
    useSource();
    *startDragResult = true;
}

static void windowDrags()
{
    puts("window drags");
    fn<DragEnd>(250)(1, 1);
    for (int round = 0; round < 160; round++) {
        std::vector<std::string> keep;
        const auto items = randomItems(keep);
        const int sizes[] = {-3, 0, 1, 2, 5, 17, 64};
        const double scales[] = {-1, 0, 1, 1.5, 2};
        *startDragResult = pick(5);
        *isHeadless = round % 2;
        const bool callback = pick(4);
        reef_ui_window* dragged = pick(3) ? pick(2) ? window2 : window : nullptr;
        pressed() = true;
        printf("window drag %d headless=%d items=%zu result=%d\n", round, *isHeadless, items.size(), *startDragResult);
        const int result = fn<StartWindowDrag>(249)(window, dragged, pick(100), pick(100) / 8.0, items.data(), items.size(),
                                                    pick(4) ? pixels.data() : nullptr, sizes[pick(7)], sizes[pick(7)], scales[pick(5)],
                                                    pick(9), pick(9), callback ? ended : nullptr, (void*)(long)round);
        printf(" result=%d pressed=%d actions=%u\n", result, pressed(), fn<Actions>(111)());
        pressed() = false;
        if (pick(2)) for (const char* type : {"text/plain", "a", "nothing"}) showData(type);
        if (!*isHeadless && pick(2)) useSource();
        if (pick(3)) {
            fn<DragEnd>(250)(pick(6), int(pick(4)) - 1);
            fn<DragEnd>(250)(pick(6), 1);
        }
    }
    *isHeadless = true;
    *startDragResult = true;
    // Another drag begins while the last one ends.
    static int depth = 0;
    struct Again {
        static void ended(void* context, unsigned action, int dropped)
        {
            printf("  again %ld %u %d\n", long(context), action, dropped);
            if (depth++ < 3) {
                fn<StartWindowDrag>(249)(window, window2, 0, 0, nullptr, 0, nullptr, 0, 0, 0, 0, 0, ended, (void*)(long)(depth * 10));
                fn<DragEnd>(250)(depth, 0);
            }
        }
    };
    fn<StartWindowDrag>(249)(window, window2, 0, 0, nullptr, 0, nullptr, 0, 0, 0, 0, 0, Again::ended, (void*)5);
    fn<DragEnd>(250)(9, 2);
    fn<DragEnd>(250)(9, 2);
    struct Throws { static void ended(void*, unsigned, int) { throw std::runtime_error("ended"); } };
    fn<StartWindowDrag>(249)(window, window2, 0, 0, nullptr, 0, nullptr, 0, 0, 0, 0, 0, Throws::ended, (void*)5);
    try { fn<DragEnd>(250)(1, 1); } catch (const std::exception& e) { printf("exception %s\n", e.what()); }
    fn<DragEnd>(250)(1, 1);
}

// ------------------------------------------------------------------ test drops

static unsigned dragAnswer = 1;
static int dragReads = 0;

static unsigned dragged(void* context, int event, double x, double y, void* offer, unsigned actions, unsigned modifiers)
{
    printf("  drag window=%ld event=%d at %.3f,%.3f offer=%d actions=%u modifiers=%u\n", long(context), event, x, y, offer != nullptr, actions, modifiers);
    if (offer && event != 2) {
        char buffer[600];
        memset(buffer, 0, sizeof buffer);
        printf("  types %d ", fn<Types>(107)(static_cast<reef_ui_drop_offer*>(offer), buffer, sizeof buffer)); text(buffer); puts("");
        if (dragReads & (1 << event)) {
            for (const char* type : {"text/plain", "a", "", "nothing", "application/octet-stream-of-a-long-name"})
                fn<Read>(108)(static_cast<reef_ui_drop_offer*>(offer), type, readDone, (void*)(long)event);
        }
    }
    return dragAnswer;
}

static void testDrops()
{
    puts("test drops");
    for (int round = 0; round < 120; round++) {
        std::vector<std::string> keep;
        std::vector<reef_ui_mime_data> items(pick(5));
        for (reef_ui_mime_data& item : items) {
            memset(&item, 0, sizeof item);
            item.type = pick(6) ? typeNames[pick(7)] : nullptr;
            const unsigned sizes[] = {0, 1, 2, 14, 15, 16, 31, 500};
            keep.reserve(16);
            keep.push_back(std::string(sizes[pick(8)], char('A' + pick(26))));
            item.bytes = pick(5) ? keep.back().data() : nullptr;
            item.size = item.bytes ? keep.back().size() : 0;
        }
        const unsigned answers[] = {0, 1, 2, 4, 0x301, 0x702, 0x100};
        dragAnswer = answers[pick(7)];
        dragReads = pick(16);
        const unsigned actionList[] = {0, 1, 2, 3, 7};
        const unsigned actions = actionList[pick(5)];
        const double places[] = {-5, 0, 10, 50.5, 199, 1000};
        const double x = places[pick(6)], y = places[pick(6)];
        printf("drop %d items=%zu answer=%u actions=%u at %g,%g\n", round, items.size(), dragAnswer, actions, x, y);
        const unsigned result = fn<TestDrop>(112)(pick(2) ? window : window2, x, y, items.data(), items.size(), actions);
        printf(" result=%u\n", result);
    }
    reef_ui_mime_data bad = {};
    bad.type = "text/plain";
    bad.size = 4;
    try { fn<TestDrop>(112)(window, 1, 1, &bad, 1, 0); } catch (const std::exception& e) { printf("exception %s\n", e.what()); }
}

// ------------------------------------------------------------------ file choosers

static int optionCount = 0;

static void chosen(void* context, const char* const* paths, unsigned count, const char* const* options)
{
    printf("  answer context=%ld paths=%u", long(context), count);
    for (unsigned i = 0; i < count; i++) { putchar(' '); text(paths[i]); }
    printf(" options");
    for (int i = 0; i < optionCount; i++) { putchar(' '); text(options[i]); }
    puts("");
}

static void dispatch()
{
    const int fd = fn<int()>(1)();
    for (int i = 0; i < 20; i++) {
        pollfd p = {fd, POLLIN, 0};
        if (poll(&p, 1, 0) <= 0)
            break;
        fn<void()>(2)();
    }
}

static const char* const words[] = {"", "a", "b", "fifteen chars!!", "sixteen chars!!!", "a much longer text than what fits inside a string", "=", "x=y", nullptr, "a"};
static const char* word() { return words[pick(10)]; }
static const char* const patternList[] = {"", ";", ";;", "*.png", "*.png;*.jpg", ";*.png;;*.jpg;", "image/png;image/jpeg;application/x-something-quite-long", "a;b;c;d;e;f;g;h;i;j", nullptr,
                                          "no separators but more than sixteen characters", "fifteen chars!!;sixteen chars!!!;"};

struct Request
{
    reef_ui_file_chooser_request request = {};
    std::vector<reef_ui_file_chooser_filter> filters;
    std::vector<reef_ui_file_chooser_choice> choices;
    std::vector<std::vector<const char*>> arrays;

    Request()
    {
        arrays.reserve(64);
        const int modes[] = {-1, 0, 1, 2, 3, 4, 5, 1000, INT_MIN, INT_MAX};
        request.mode = modes[pick(10)];
        const int modals[] = {0, 1, 2, -1, 256};
        request.modal = modals[pick(5)];
        request.title = word();
        request.acceptLabel = word();
        request.folder = word();
        request.file = word();
        for (unsigned i = pick(4); i; i--) filters.push_back({word(), patternList[pick(11)], patternList[pick(11)]});
        for (unsigned i = pick(5); i; i--) {
            std::vector<const char*> a, b;
            for (unsigned n = pick(5); n; n--) { a.push_back(word()); b.push_back(word()); }
            arrays.push_back(a);
            arrays.push_back(b);
            reef_ui_file_chooser_choice choice = {word(), word(), arrays[arrays.size() - 2].data(), arrays.back().data(), unsigned(a.size()), word()};
            choices.push_back(choice);
        }
        request.filters = filters.data();
        request.filterCount = filters.size();
        request.choices = pick(6) ? choices.data() : nullptr;
        request.choiceCount = choices.size();
        optionCount = request.choices ? request.choiceCount : 0;
    }
};

static void choosers()
{
    puts("headless choosers");
    fn<Destroy>(100)(nullptr);
    for (int round = 0; round < 300; round++) {
        Request r;
        printf("chooser %d\n", round);
        if (pick(5)) {
            std::vector<const char*> paths, options;
            for (unsigned n = pick(4); n; n--) { const char* w = word(); paths.push_back(w ? w : "/tmp/file"); }
            for (unsigned n = pick(6); n; n--) {
                const char* list[] = {"a=1", "a=", "=1", "a", "", nullptr, "b=two=2", "fifteen chars!!=the option that was picked, long", "x=y=z", "sixteen chars!!!=s"};
                options.push_back(list[pick(10)]);
            }
            const bool noPaths = !pick(6), noOptions = !pick(6);
            fn<SetAnswer>(101)(noPaths ? nullptr : paths.data(), paths.size(), noOptions ? nullptr : options.data(), options.size());
            if (!pick(5)) fn<SetAnswer>(101)(paths.data(), paths.size(), options.data(), options.size());
        }
        reef_ui_file_chooser* chooser = fn<Open>(99)(pick(2) ? window : nullptr, &r.request, chosen, (void*)(long)round);
        printf("  opened %d hook=%d\n", chooser != nullptr, *hookChooser != nullptr);
        // What was given is the caller's again.
        r = Request();
        const int before = optionCount;
        (void)before;
        const unsigned what = pick(4);
        if (what == 0) { fn<Destroy>(100)(chooser); puts("  destroyed"); dispatch(); continue; }
        if (what == 1) {
            // A second one before the first answers: the test answer is for the first.
            Request r2;
            const int count2 = optionCount;
            reef_ui_file_chooser* second = fn<Open>(99)(window, &r2.request, chosen, (void*)(long)(round + 10000));
            optionCount = 0;
            dispatch();
            (void)count2;
            fn<Destroy>(100)(second);
        } else {
            optionCount = 0;
            dispatch();
        }
        fn<Destroy>(100)(chooser);
    }
    puts("headless choosers with options");
    for (int round = 0; round < 300; round++) {
        Request r;
        const int count = optionCount;
        if (pick(5)) {
            std::vector<const char*> paths, options;
            for (unsigned n = pick(4); n; n--) { const char* w = word(); paths.push_back(w ? w : "/tmp/file"); }
            for (unsigned n = pick(6); n; n--) {
                const char* list[] = {"a=1", "a=", "=1", "a", "", nullptr, "b=two=2", "fifteen chars!!=the option that was picked, long", "x=y=z", "sixteen chars!!!=s"};
                options.push_back(list[pick(10)]);
            }
            fn<SetAnswer>(101)(paths.data(), paths.size(), options.data(), options.size());
        }
        printf("chooser %d options=%d\n", round, count);
        reef_ui_file_chooser* chooser = fn<Open>(99)(window, &r.request, chosen, (void*)(long)round);
        optionCount = count;
        dispatch();
        fn<Destroy>(100)(chooser);
    }
    const char* bad[] = {"/a", nullptr};
    try { fn<SetAnswer>(101)(bad, 2, nullptr, 0); } catch (const std::exception& e) { printf("exception %s\n", e.what()); }
    {
        Request r;
        reef_ui_file_chooser* chooser = fn<Open>(99)(window, &r.request, chosen, (void*)77);
        dispatch();
        fn<Destroy>(100)(chooser);
    }
    puts("choosers");
    *isHeadless = false;
    for (int round = 0; round < 400; round++) {
        Request r;
        const int count = optionCount;
        printf("chooser %d\n", round);
        reef_ui_file_chooser* chooser = fn<Open>(99)(pick(4) ? pick(2) ? window : window2 : nullptr, &r.request, chosen, (void*)(long)round);
        ui::FileChooser* dialog = *hookChooser;
        printf("  opened %d %d\n", chooser != nullptr, dialog == *reinterpret_cast<ui::FileChooser**>(chooser));
        r = Request();
        optionCount = count;
        auto accepted = dialog->onAccepted;
        auto rejected = dialog->onRejected;
        for (int i = pick(4); i; i--) {
            std::vector<std::string> paths;
            for (unsigned n = pick(5); n; n--) paths.push_back(std::string(pick(40), char('k' + pick(10))));
            dialog->m_chosenOptions.clear();
            for (unsigned n = pick(9); n; n--) {
                const char* id = word();
                const char* option = word();
                dialog->m_chosenOptions.emplace_back(id ? id : "", option ? option : "");
            }
            if (pick(2)) dialog->onAccepted(paths, "filter");
            else if (pick(2)) accepted(paths, "");
            else dialog->onRejected();
        }
        fn<Destroy>(100)(chooser);
        puts("  destroyed");
        accepted({"/late"}, "");
        rejected();
    }
    *isHeadless = true;
}

int main(int argc, char** argv)
{
    setvbuf(stdout, nullptr, _IOFBF, 1 << 16);
    void* library = dlopen("/o/lib/cutefish-reef/host/libreef-appkit-host.so", RTLD_NOW | RTLD_GLOBAL);
    if (!library) { fprintf(stderr, "%s\n", dlerror()); return 1; }
    auto interface = reinterpret_cast<const uint64_t* (*)(unsigned)>(dlsym(library, "reef_appkit_host_interface"));
    memcpy(table, interface(0x18) + 1, sizeof table);
    link_map* map;
    dlinfo(library, RTLD_DI_LINKMAP, &map);
    isHeadless = reinterpret_cast<bool*>(map->l_addr + 0x1575c1);
    bool* original = isHeadless;

    startDragResult = static_cast<bool*>(dlsym(RTLD_DEFAULT, "hook_startDragResult"));
    hookView = static_cast<ui::View**>(dlsym(RTLD_DEFAULT, "hook_view"));
    hookSource = static_cast<ui::DragSource**>(dlsym(RTLD_DEFAULT, "hook_source"));
    hookChooser = static_cast<ui::FileChooser**>(dlsym(RTLD_DEFAULT, "hook_chooser"));
    if (!startDragResult) { fprintf(stderr, "no hooks\n"); return 1; }

    static reef_ui_callbacks callbacks = {};
    callbacks.drag = dragged;
    const char* arguments[] = {"driver", "-ReefHeadless", "YES"};
    printf("init %d\n", fn<int(int, const char* const*, const char*, const reef_ui_callbacks*)>(0)(3, arguments, "test.driver", &callbacks));
    printf("headless %d\n", *original);
    using Create = reef_ui_window*(void*, double, double, int, reef_ui_window*, double, double);
    window = fn<Create>(10)((void*)1, 200, 150, 0, nullptr, 0, 0);
    window2 = fn<Create>(10)((void*)2, 100, 80, 2, nullptr, 0, 0);
    fn<void(reef_ui_window*)>(16)(window);
    fn<void(reef_ui_window*)>(16)(window2);
    dispatch();
    *static_cast<ui::Window**>(dlsym(RTLD_DEFAULT, "hook_window")) = window->window;
    *static_cast<ui::Window**>(dlsym(RTLD_DEFAULT, "hook_window2")) = window2->window;

    if (argc > 1 && !strcmp(argv[1], "ours")) {
        void* mine = dlopen(getenv("MINE") ? getenv("MINE") : "/tmp/dialogs/t/mine.so", RTLD_LAZY | RTLD_LOCAL);
        if (!mine) { fprintf(stderr, "%s\n", dlerror()); return 1; }
        reinterpret_cast<void (*)(void**)>(dlsym(mine, "mine_table"))(table);
        isHeadless = reinterpret_cast<bool* (*)()>(dlsym(mine, "mine_headless"))();
    }
    offers();
    drags();
    windowDrags();
    testDrops();
    choosers();
    puts("end");
    fflush(stdout);
    _exit(0);
}

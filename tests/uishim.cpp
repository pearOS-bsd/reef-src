// LD_PRELOAD stand-in for the toolkit's user interface: no display, every text and setting is printed.
// Built against the toolkit's headers for the object sizes.
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include "base/event_loop.h"
#include "base/timer.h"
#include "ui/application.h"
#include "ui/cell_layout.h"
#include "ui/dialog_window.h"
#include "ui/icon.h"
#include "ui/label.h"
#include "ui/scroll_view.h"
#include "ui/text_area.h"
#include "ui/theme.h"
#include "ui/window.h"
using namespace cutefish::ui;
using S = std::string;
using P = std::unique_ptr<View>;
#define F(name, ret, args) extern "C" ret name args; ret name args
#define PRINT(...) (printf(__VA_ARGS__), putchar('\n'), fflush(stdout))
template<typename T> void zero(void* p) { memset(p, 0, sizeof(T)); }
static Window* window;
static DialogButton* primary;
F(_ZN8cutefish2ui11ApplicationC1EiPPcNS1_8PlatformE, void, (void* p)) { zero<Application>(p); new cutefish::base::EventLoop; PRINT("app"); }
F(_ZN8cutefish2ui11ApplicationD1Ev, void, (void*)) {}
F(_ZN8cutefish2ui11Application16setApplicationIdENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE, void, (void*, S& s)) { PRINT("id %s", s.c_str()); }
F(_ZN8cutefish2ui11Application4quitEi, void, (void*, int c)) { PRINT("quit %d", c); }
F(_ZN8cutefish2ui11Application3runEv, int, (void*))
{
    PRINT("run");
    // Long enough for the stand-in loader to end.
    cutefish::base::EventLoop* loop = cutefish::base::EventLoop::current();
    cutefish::base::Timer timer(loop);
    timer.setSingleShot(true);
    timer.onTimeout = [loop] { loop->quit(); };
    timer.start(std::chrono::milliseconds(120));
    loop->run();
    if (window->onCloseRequested) window->onCloseRequested();
    if (getenv("CLICK") && primary->onClicked) primary->onClicked();
    return 7;
}
F(_ZN8cutefish2ui6WindowC1Ev, void, (void* p)) { zero<Window>(p); window = static_cast<Window*>(p); }
F(_ZN8cutefish2ui6WindowD1Ev, void, (void*)) {}
F(_ZN8cutefish2ui6Window8setTitleERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE, void, (void*, const S& s)) { PRINT("title %s", s.c_str()); }
#define WBOOL(n, sym) F(sym, void, (void*, bool b)) { PRINT(n " %d", b); }
WBOOL("skipTaskbar", _ZN8cutefish2ui6Window14setSkipTaskbarEb)
WBOOL("alwaysOnTop", _ZN8cutefish2ui6Window14setAlwaysOnTopEb)
WBOOL("hinted", _ZN8cutefish2ui6Window19setHintedTextLayoutEb)
WBOOL("fits", _ZN8cutefish2ui6Window20setFitsContentHeightEb)
WBOOL("wrap", _ZN8cutefish2ui5Label7setWrapEb)
WBOOL("readOnly", _ZN8cutefish2ui8TextArea11setReadOnlyEb)
WBOOL("frame", _ZN8cutefish2ui8TextArea15setFrameVisibleEb)
F(_ZN8cutefish2ui6Window18setBackgroundColorESt8optionalINS0_11ColorRecipeEE, void, (void*, void*)) { PRINT("background"); }
F(_ZN8cutefish2ui6Window11addShortcutEjjSt8functionIFvvEE, int, (void*, unsigned m, unsigned k, std::function<void()>& f)) { PRINT("shortcut %u %x %d", m, k, bool(f)); return 1; }
F(_ZN8cutefish2ui6Window14setContentViewESt10unique_ptrINS0_4ViewESt14default_deleteIS3_EE, void, (void*, P& v)) { v.release(); PRINT("content"); }
F(_ZN8cutefish2ui6Window6resizeENS0_5SizeFE, void, (SizeF s, void*)) { PRINT("resize %g %g", s.width, s.height); }
F(_ZN8cutefish2ui6Window4showEv, void, (void*)) { PRINT("show"); }
F(_ZN8cutefish2ui6Window8activateEv, void, (void*)) { PRINT("activate"); }
F(_ZN8cutefish2ui6Window22requestActivationTokenESt8functionIFvRKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEE, void, (void*, std::function<void(const S&)>& f)) { PRINT("token"); f("TOKEN"); }
F(_ZN8cutefish2ui4ViewC1Ev, void, (void* p)) { zero<View>(p); }
F(_ZN8cutefish2ui4ViewC2Ev, void, (void* p)) { zero<View>(p); }
F(_ZN8cutefish2ui4ViewD2Ev, void, (void*)) {}
F(_ZN8cutefish2ui4View8addChildESt10unique_ptrIS1_St14default_deleteIS1_EE, void, (void*, P& v)) { v.release(); PRINT("child"); }
F(_ZN8cutefish2ui4View14setLayoutHintsERKNS0_11LayoutHintsE, void, (void*, const LayoutHints& h))
{ PRINT("hints %d %d %g a%d p%g,%g min%g,%g max%g,%g h%d v%d f%g,%g", h.fillWidth, h.fillHeight, h.stretch, int(h.alignment), h.preferredWidth, h.preferredHeight, h.minimumWidth, h.minimumHeight, h.maximumWidth, h.maximumHeight, int(h.horizontalAlignment), int(h.verticalAlignment), h.fallbackWidth, h.fallbackHeight); }
F(_ZN8cutefish2ui4View8setFocusENS0_11FocusReasonE, void, (void* p, int r)) { PRINT("focus %d primary %d", r, p == primary); }
F(_ZN8cutefish2ui10CellLayoutC2ENS1_4FlowEi, void, (void* p, int f, int c)) { zero<CellLayout>(p); PRINT("layout %d %d", f, c); }
F(_ZN8cutefish2ui10CellLayout10setPaddingERKNS0_7MarginsE, void, (void*, const Margins& m)) { PRINT("padding %g %g %g %g", m.left, m.top, m.right, m.bottom); }
F(_ZN8cutefish2ui10CellLayout10setSpacingEd, void, (void*, double d)) { PRINT("spacing %g", d); }
F(_ZN8cutefish2ui5LabelC1ENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE, void, (void* p, S& s)) { zero<Label>(p); PRINT("label %s", s.c_str()); }
F(_ZN8cutefish2ui5Label11setFontSizeEd, void, (void*, double d)) { PRINT("fontSize %g", d); }
F(_ZN8cutefish2ui5Label9setWeightEi, void, (void*, int w)) { PRINT("weight %d", w); }
F(_ZN8cutefish2ui5Label8setColorESt8optionalINS0_11ColorRecipeEE, void, (void*, void*)) { PRINT("color"); }
F(_ZN8cutefish2ui9ThemeIconC1ENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEd, void, (void* p, S& s, double d)) { zero<ThemeIcon>(p); PRINT("icon %s %g", s.c_str(), d); }
F(_ZN8cutefish2ui12DialogButtonC1ENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEb, void, (void* p, S& s, bool b)) { zero<DialogButton>(p); if (b) primary = static_cast<DialogButton*>(p); PRINT("button %s %d", s.c_str(), b); }
F(_ZN8cutefish2ui10ScrollViewC1Ev, void, (void* p)) { zero<ScrollView>(p); }
F(_ZN8cutefish2ui10ScrollView14setContentViewESt10unique_ptrINS0_4ViewESt14default_deleteIS3_EE, void, (void*, P& v)) { v.release(); }
F(_ZN8cutefish2ui10ScrollView19setScrollDirectionsEbb, void, (void*, bool a, bool b)) { PRINT("scroll %d %d", a, b); }
F(_ZN8cutefish2ui10ScrollView15setCornerRadiusEd, void, (void*, double d)) { PRINT("radius %g", d); }
F(_ZN8cutefish2ui8TextAreaC1ENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE, void, (void* p, S&)) { zero<TextArea>(p); }
F(_ZN8cutefish2ui8TextArea7setTextENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE, void, (void*, S& s)) { PRINT("text [%s]", s.c_str()); }
F(_ZN8cutefish2ui8TextArea10setPaddingERKNS0_7MarginsE, void, (void*, const Margins& m)) { PRINT("areaPadding %g %g", m.left, m.bottom); }
F(_ZN8cutefish2ui8TextArea11setWrapModeENS1_8WrapModeE, void, (void*, int m)) { PRINT("wrapMode %d", m); }
F(_ZN8cutefish2ui8TextArea7setFontERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEdi, void, (void*, const S& f, double d, int w)) { PRINT("font %s %g %d", f.c_str(), d, w); }

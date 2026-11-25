// A stand-in for libcutefish-webkit: the few types that the JSON code needs, and a log of every other call.
// Built by tests/webkit.py, which adds the generic functions (-DGENERIC='F(name) ...').
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>

struct Object { int type; std::string text; double number = 0; uint64_t integer = 0; std::vector<Object*> list, keys; };
enum { String = 1, Double, UInt64, Boolean, Array, Dictionary, Url };
static std::map<std::string, void*> callbacks;
static Object* made(int type) { return new Object{type, {}}; }
#define E extern "C"
#define LOG(...) (printf(__VA_ARGS__), fputs("\n", stdout))
static void dump(Object* o) {
    if (!o) { printf("nil"); return; }
    switch (o->type) {
    case String: printf("s%zu:", o->text.size()); for (unsigned char c : o->text) printf("%02x", c); break;
    case Double: printf("d%.17g", o->number); break;
    case Boolean: printf("b%d", int(o->integer)); break;
    case Array: printf("["); for (auto* x : o->list) dump(x), printf(","); printf("]"); break;
    case Dictionary: printf("{"); for (size_t i = 0; i < o->list.size(); i++) dump(o->keys[i]), printf(":"), dump(o->list[i]), printf(","); printf("}"); break;
    default: printf("?%d", o->type);
    }
}
E Object* WKStringCreateWithUTF8CString(const char* s) { Object* o = made(String); o->text = s; return o; }
E size_t WKStringGetMaximumUTF8CStringSize(Object* o) { return o->text.size() * 3 + 1; }
E size_t WKStringGetUTF8CString(Object* o, char* b, size_t n) { memcpy(b, o->text.c_str(), o->text.size() + 1); return o->text.size() + 1; }
E Object* WKURLCreateWithUTF8CString(const char* s) { Object* o = made(Url); o->text = s; return o; }
E Object* WKURLCopyString(Object* o) { Object* r = made(String); r->text = o->text; return r; }
E Object* WKDoubleCreate(double d) { Object* o = made(Double); o->number = d; return o; }
E double WKDoubleGetValue(Object* o) { return o->number; }
E Object* WKBooleanCreate(bool b) { Object* o = made(Boolean); o->integer = b; return o; }
E bool WKBooleanGetValue(Object* o) { return o->integer; }
E uint64_t WKUInt64GetValue(Object* o) { return o->integer; }
E Object* WKMutableArrayCreate() { return made(Array); }
E bool WKArrayAppendItem(Object* a, Object* x) { a->list.push_back(x); return true; }
E size_t WKArrayGetSize(Object* a) { return a->list.size(); }
E Object* WKArrayGetItemAtIndex(Object* a, size_t i) { return a->list[i]; }
E Object* WKMutableDictionaryCreate() { return made(Dictionary); }
E void WKDictionarySetItem(Object* d, Object* k, Object* v) { Object* c = made(String); c->text = k->text; d->keys.push_back(c); d->list.push_back(v); }
E Object* WKDictionaryCopyKeys(Object* d) { Object* a = made(Array); a->list = d->keys; return a; }
E Object* WKDictionaryGetItemForKey(Object* d, Object* k) { for (size_t i = 0; i < d->keys.size(); i++) if (d->keys[i]->text == k->text) return d->list[i]; return nullptr; }
E unsigned WKGetTypeID(Object* o) { return unsigned(o->type); }
E unsigned WKStringGetTypeID() { return String; }
E unsigned WKDoubleGetTypeID() { return Double; }
E unsigned WKUInt64GetTypeID() { return UInt64; }
E unsigned WKBooleanGetTypeID() { return Boolean; }
E unsigned WKArrayGetTypeID() { return Array; }
E unsigned WKDictionaryGetTypeID() { return Dictionary; }
E unsigned WKURLGetTypeID() { return Url; }
E void WKRelease(Object*) {}
E void* WKRetain(void* x) { return x; }
E void WKCompletionListenerComplete(void*, Object* o) { printf("complete "); dump(o); LOG(""); }
E Object* WKScriptMessageGetBody(Object* m) { return m; }
E void WKUserContentControllerAddScriptMessageHandlerInWorld(void*, Object* n, Object* w, void* f, void* c) {
    LOG("handler %s %s", n->text.c_str(), w ? w->text.c_str() : "-"); callbacks["message"] = f; callbacks["context"] = c; }
E void* WKContextConfigurationCreate() { return made(99); }
E void* WKContextCreateWithConfiguration(void*) { return made(99); }
E void* WKUserContentControllerCreate() { return made(99); }
E int WKMainRunLoopGetDescriptor() { return 7; }
E double WKMainRunLoopIterate() { return 0.5; }
E void WKViewSendMouseEvent(void*, const int* e) { const double* d = reinterpret_cast<const double*>(e);
    LOG("mouse %d %d %d %g %g %g %g %d %x", e[0], e[1], e[2] & 0xffff, d[2], d[3], d[4], d[5], e[12], e[13]); }
E void WKViewSendWheelEvent(void*, const double* d) { const unsigned char* c = reinterpret_cast<const unsigned char*>(d + 8);
    LOG("wheel %g %g %g %g %g %g %g %g %d %d %d %x %g", d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], c[0], c[1], c[2], *reinterpret_cast<const int*>(c + 4), d[9]); }
E void WKViewSendKeyboardEvent(void*, const int* e) { const char* t = *reinterpret_cast<const char* const*>(e + 4);
    LOG("key %d %x %x %s %d %x", e[0], e[1], e[2], t ? t : "-", e[6] & 0xffff, e[7]); }
E void WKSetScreens(const int* s, size_t n, unsigned p) { printf("screens %zu %u", n, p); for (size_t i = 0; i < n * 24; i++) printf(" %x", i % 24 == 1 ? 0 : s[i]); LOG(""); }
E void WKPageFindString(void*, Object* t, unsigned o, unsigned n) { LOG("find %s %x %u", t->text.c_str(), o, n); }
E void WKWebsiteDataStoreFetchDataRecords(void*, unsigned kinds, void* c, void (*f)(const void*, size_t, void*)) {
    struct { const char* n; unsigned t; } r[] = {{"a.example", kinds}, {"b.example", 0x200}, {"c.example", ~0u}};
    LOG("fetch %x", kinds); f(r, 3, c); }
E void WKWebsiteDataStoreRemoveDataRecords(void*, unsigned kinds, const char* const*, size_t n, void*, void*) { LOG("remove %x %zu", kinds, n); }
E void* WKFakeCallback(const char* n) { return callbacks[n]; }
E Object* WKFakeUInt64(uint64_t v) { Object* o = made(UInt64); o->integer = v; return o; }
#define F(name) E void* name() { puts(#name); return nullptr; }
#include "generic.inc"

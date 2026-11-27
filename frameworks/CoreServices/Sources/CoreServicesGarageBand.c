// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Carbon Core and Metadata functions GarageBand imports and Reef's CoreServices lacks.
#include <CoreFoundation/CoreFoundation.h>
#include <dispatch/dispatch.h>
#include <mach/mach_time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef char *Ptr;
typedef char **Handle;
typedef long Size;
typedef short OSErr;
typedef int32_t OSStatus;
extern Size GetHandleSize(Handle);
extern void SetHandleSize(Handle, Size);

static uint64_t nanoseconds(void) {
    static mach_timebase_info_data_t base;
    if (!base.denom) mach_timebase_info(&base);
    return mach_absolute_time() * base.numer / base.denom;
}

// Ticks are sixtieths of a second since startup
uint32_t TickCount(void) { return (uint32_t)(nanoseconds() / 16666667ull); }

void Delay(unsigned long ticks, unsigned long *finalTicks) {
    usleep((useconds_t)(ticks * 16667));
    if (finalTicks) *finalTicks = TickCount();
}

static short memoryError;
void LMSetMemErr(short value) { memoryError = value; }
short LMGetMemErr(void) { return memoryError; }

Ptr NewPtr(Size size) {
    Ptr p = malloc(size > 0 ? (size_t)size : 1);
    memoryError = p ? 0 : -108;   // memFullErr
    return p;
}
void DisposePtr(Ptr p) { free(p); memoryError = 0; }

OSErr PtrAndHand(const void *p, Handle h, long size) {
    if (!h) return -109;   // nilHandleErr
    Size old = GetHandleSize(h);
    SetHandleSize(h, old + size);
    if (GetHandleSize(h) != old + size) return -108;
    memcpy(*h + old, p, (size_t)size);
    return 0;
}

signed char HGetState(Handle h) { return 0; }
void HSetState(Handle h, signed char flags) {}

// The Time Manager: a task's routine runs once its time has passed (milliseconds, or microseconds if negative)
typedef struct TMTask {
    void *qLink;
    short qType;
    void (*tmAddr)(struct TMTask *);
    long tmCount;
    long tmWakeUp;
    long tmReserved;
} TMTask;

void InsTime(TMTask *task) {}
void InsXTime(TMTask *task) {}
void RmvTime(TMTask *task) { task->tmCount = 0; }
void PrimeTime(TMTask *task, long count) {
    int64_t delay = count >= 0 ? count * NSEC_PER_MSEC : -count * NSEC_PER_USEC;
    task->tmCount = count;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delay), dispatch_get_global_queue(QOS_CLASS_USER_INTERACTIVE, 0), ^{
        if (task->tmAddr && task->tmCount) {
            task->tmCount = 0;
            task->tmAddr(task);
        }
    });
}

static uint32_t processors(void) { long n = sysconf(_SC_NPROCESSORS_ONLN); return n > 0 ? (uint32_t)n : 1; }
uint32_t MPProcessors(void) { return processors(); }
uint32_t MPProcessorsScheduled(void) { return processors(); }

// The Component Manager has no components here
void *FindNextComponent(void *component, void *description) { return NULL; }

// Unicode collation through CFString comparison (kUCCollateCaseInsensitiveMask 1 << 1)
typedef struct { uint32_t options; } Collator;
OSStatus UCCreateCollator(void *locale, uint32_t variant, uint32_t options, Collator **collator) {
    *collator = malloc(sizeof **collator);
    (*collator)->options = options;
    return 0;
}
OSStatus UCDisposeCollator(Collator **collator) {
    if (collator && *collator) { free(*collator); *collator = NULL; }
    return 0;
}
OSStatus UCCompareText(Collator *collator, const UniChar *a, unsigned long lengthA, const UniChar *b, unsigned long lengthB,
                       Boolean *equivalent, int32_t *order) {
    CFStringRef sa = CFStringCreateWithCharactersNoCopy(NULL, a, (CFIndex)lengthA, kCFAllocatorNull);
    CFStringRef sb = CFStringCreateWithCharactersNoCopy(NULL, b, (CFIndex)lengthB, kCFAllocatorNull);
    CFStringCompareFlags flags = kCFCompareLocalized | kCFCompareNonliteral;
    if (collator && (collator->options & 2)) flags |= kCFCompareCaseInsensitive;
    CFComparisonResult r = CFStringCompare(sa, sb, flags);
    CFRelease(sa);
    CFRelease(sb);
    if (equivalent) *equivalent = r == kCFCompareEqualTo;
    if (order) *order = r == kCFCompareLessThan ? -1 : r == kCFCompareGreaterThan ? 1 : 0;
    return 0;
}

typedef struct { uint16_t length; UniChar unicode[255]; } HFSUniStr255;
CFStringRef FSCreateStringFromHFSUniStr(CFAllocatorRef allocator, const HFSUniStr255 *name) {
    return CFStringCreateWithCharacters(allocator, name->unicode, name->length);
}
OSStatus FSGetHFSUniStrFromString(CFStringRef string, HFSUniStr255 *name) {
    CFIndex n = CFStringGetLength(string);
    if (n > 255) return -1254;   // errFSNameTooLong
    CFStringGetCharacters(string, CFRangeMake(0, n), name->unicode);
    name->length = (uint16_t)n;
    return 0;
}
OSErr FSResolveNodeID(short volume, uint32_t node, void *ref) { return -43; }   // fnfErr

// Space a volume could free (purgeable files): none known
uint64_t CSDiskSpaceGetRecoveryEstimate(CFURLRef url) { return 0; }

// Spotlight queries: there is no index here, so a query finishes at once without results. The query is a
// CF object of its own (callers retain and release it); asynchronous queries post their notifications on
// the local center.
extern const CFStringRef kMDQueryDidFinishNotification;
extern const CFStringRef kMDQueryProgressNotification;

typedef CFMutableDictionaryRef MDQueryRef;

MDQueryRef MDQueryCreate(CFAllocatorRef allocator, CFStringRef query, CFArrayRef valueListAttributes, CFArrayRef sortingAttributes) {
    if (!query) return NULL;
    MDQueryRef q = CFDictionaryCreateMutable(allocator, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(q, CFSTR("query"), query);
    return q;
}

Boolean MDQueryExecute(MDQueryRef query, CFOptionFlags options) {
    if (!query) return false;
    if (!(options & 1)) {   // not kMDQuerySynchronous
        CFRetain(query);
        dispatch_async(dispatch_get_main_queue(), ^{
            CFNotificationCenterRef center = CFNotificationCenterGetLocalCenter();
            CFNotificationCenterPostNotification(center, kMDQueryProgressNotification, query, NULL, true);
            CFNotificationCenterPostNotification(center, kMDQueryDidFinishNotification, query, NULL, true);
            CFRelease(query);
        });
    }
    return true;
}
void MDQueryStop(MDQueryRef query) {}
void MDQueryDisableUpdates(MDQueryRef query) {}
void MDQueryEnableUpdates(MDQueryRef query) {}
CFIndex MDQueryGetResultCount(MDQueryRef query) { return 0; }
const void *MDQueryGetResultAtIndex(MDQueryRef query, CFIndex index) { return NULL; }
void *MDQueryGetAttributeValueOfResultAtIndex(MDQueryRef query, CFStringRef name, CFIndex index) { return NULL; }
void MDQuerySetSearchScope(MDQueryRef query, CFArrayRef scope, OptionBits options) {}
void MDQuerySetSortComparator(MDQueryRef query, void *comparator, void *context) {}
CFDictionaryRef _MDConfigCopyStoreAttributes(CFStringRef path) { return NULL; }

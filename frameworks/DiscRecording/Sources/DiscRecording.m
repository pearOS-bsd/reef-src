// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Disc burning: what a Mac without an optical drive answers (no devices, nothing to burn on).
#import <Foundation/Foundation.h>

const float kDRDeviceBurnSpeedCD1x = 176.4f;    // kilobytes per second
const float kDRDeviceBurnSpeedDVD1x = 1385.0f;

CFArrayRef DRCopyDeviceArray(void) { return CFArrayCreate(NULL, NULL, 0, &kCFTypeArrayCallBacks); }
Boolean DRDeviceIsValid(CFTypeRef device) { return false; }
CFDictionaryRef DRDeviceCopyInfo(CFTypeRef device) { return NULL; }
CFDictionaryRef DRDeviceCopyStatus(CFTypeRef device) { return NULL; }
OSStatus DRDeviceEjectMedia(CFTypeRef device) { return -50; }   // paramErr
void DRDeviceAcquireMediaReservation(CFTypeRef device) {}
void DRDeviceReleaseMediaReservation(CFTypeRef device) {}
CFTypeRef DRBurnCreate(CFTypeRef device) { return NULL; }
CFTypeRef DRBurnGetDevice(CFTypeRef burn) { return NULL; }
CFDictionaryRef DRBurnGetProperties(CFTypeRef burn) { return NULL; }
void DRBurnSetProperties(CFTypeRef burn, CFDictionaryRef properties) {}
OSStatus DRBurnWriteLayout(CFTypeRef burn, CFTypeRef layout) { return -50; }
CFTypeRef DRTrackCreate(CFDictionaryRef properties, void *callback) { return NULL; }

static NSMapTable *refCons;
void DRSetRefCon(CFTypeRef object, void *refCon, const void *callbacks) {
    @synchronized ([NSObject class]) {
        if (!refCons) refCons = [NSMapTable mapTableWithKeyOptions:NSPointerFunctionsOpaqueMemory | NSPointerFunctionsOpaquePersonality
                                                       valueOptions:NSPointerFunctionsOpaqueMemory | NSPointerFunctionsOpaquePersonality];
        [refCons setObject:(__bridge id)refCon forKey:(__bridge id)object];
    }
}
void *DRGetRefCon(CFTypeRef object) {
    @synchronized ([NSObject class]) { return (__bridge void *)[refCons objectForKey:(__bridge id)object]; }
}

// The notification center of the run loop: there are no devices, so it never posts.
CFTypeRef DRNotificationCenterCreate(void) { return CFDictionaryCreateMutable(NULL, 0, NULL, NULL); }
static void perform(void *info) {}
CFRunLoopSourceRef DRNotificationCenterCreateRunLoopSource(CFTypeRef center) {
    CFRunLoopSourceContext context = {0};
    context.perform = perform;
    return CFRunLoopSourceCreate(NULL, 0, &context);
}
void DRNotificationCenterAddObserver(CFTypeRef center, const void *observer, void *callback, CFStringRef name, CFTypeRef object) {}
void DRNotificationCenterRemoveObserver(CFTypeRef center, const void *observer, CFStringRef name, CFTypeRef object) {}

@interface DRNotificationCenter : NSObject
@end

@implementation DRNotificationCenter
+ (DRNotificationCenter *)currentRunLoopCenter {
    static DRNotificationCenter *center;
    static dispatch_once_t once;
    dispatch_once(&once, ^{ center = [DRNotificationCenter new]; });
    return center;
}
- (void)addObserver:(id)observer selector:(SEL)selector name:(NSString *)name object:(id)object {}
- (void)removeObserver:(id)observer name:(NSString *)name object:(id)object {}
@end

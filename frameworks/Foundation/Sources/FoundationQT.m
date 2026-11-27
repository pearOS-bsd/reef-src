// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

NSString *const NSMetadataItemPixelHeightKey = @"kMDItemPixelHeight";
NSString *const NSMetadataItemPixelWidthKey = @"kMDItemPixelWidth";

// "-[Class selector]" / "+[Class selector]" for the class and selector of a method.
NSString *_NSFullMethodName(Class cls, SEL sel) {
    return [NSString stringWithFormat:@"%c[%s %s]", class_isMetaClass(cls) ? '+' : '-', class_getName(cls), sel_getName(sel)];
}
// A size as the Finder writes it ("12.3 MB").
NSString *NSLocalizedFileSizeDescription(unsigned long long bytes) {
    return [NSByteCountFormatter stringFromByteCount:(long long)bytes countStyle:NSByteCountFormatterCountStyleFile];
}

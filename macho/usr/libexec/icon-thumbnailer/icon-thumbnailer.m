// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// icon-thumbnailer BUNDLE_PATH OUTPUT_PNG SIZE: writes the icon of an application bundle as a PNG of SIZE (1 to 1024) pixels.
// Exit status 0 when written, 1 when the icon or the file failed, 2 for bad arguments.

#import <Foundation/Foundation.h>
#include <stdlib.h>

@interface NSBundle : NSObject
+ (NSBundle *)bundleWithPath:(NSString *)path;
@end

@interface NSString (UTF8)
+ (id)stringWithUTF8String:(const char *)string;
@end

@interface NSData : NSObject
- (BOOL)writeToFile:(NSString *)path atomically:(BOOL)atomically;
@end

// The icon of a bundle as PNG data; implemented by the host AppKit.
extern NSData *__reefAppKitAssetIconPNG(NSBundle *bundle, long size);

int main(int argc, char **argv)
{
    int status = 2;
    @autoreleasepool {
        if (argc >= 4) {
            NSBundle *bundle = [NSBundle bundleWithPath:[NSString stringWithUTF8String:argv[1]]];
            long size = strtol(argv[3], NULL, 10);
            if (bundle && size >= 1 && size <= 1024) {
                NSData *png = __reefAppKitAssetIconPNG(bundle, size);
                if (png)
                    status = ![png writeToFile:[NSString stringWithUTF8String:argv[2]] atomically:NO];
                else
                    status = 1;
            }
        }
    }
    return status;
}

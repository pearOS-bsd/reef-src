// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

// Opens a document with a seamless transition from its Quick Look preview; without a preview window it does nothing.
@interface QLSeamlessDocumentOpener : NSObject
@end
@implementation QLSeamlessDocumentOpener
// Opening a document from a Quick Look preview without a seam: there is no preview to continue from.
+ (instancetype)seamlessDocumentOpenerForURL:(NSURL *)url { return nil; }
@end

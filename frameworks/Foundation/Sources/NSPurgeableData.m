// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Data that the system may discard while nobody accesses it: here it is never discarded.
#import <Foundation/Foundation.h>

@implementation NSPurgeableData {
    NSMutableData *_data;
    NSInteger _accessCount;
}
- (instancetype)init { return [self initWithCapacity:0]; }
- (instancetype)initWithCapacity:(NSUInteger)capacity {
    if ((self = [super init])) {
        _data = [[NSMutableData alloc] initWithCapacity:capacity];
        _accessCount = 1;   // created with its content accessed
    }
    return self;
}
- (instancetype)initWithLength:(NSUInteger)length {
    if ((self = [self initWithCapacity:length])) _data.length = length;
    return self;
}
- (instancetype)initWithBytes:(const void *)bytes length:(NSUInteger)length {
    if ((self = [self initWithCapacity:length])) [_data appendBytes:bytes length:length];
    return self;
}
- (instancetype)initWithData:(NSData *)data {
    if ((self = [self initWithCapacity:data.length])) [_data appendData:data];
    return self;
}
- (instancetype)initWithContentsOfURL:(NSURL *)url {
    NSData *d = [NSData dataWithContentsOfURL:url];
    return d ? [self initWithData:d] : nil;
}
- (instancetype)initWithContentsOfURL:(NSURL *)url options:(NSDataReadingOptions)options error:(NSError **)error {
    NSData *d = [NSData dataWithContentsOfURL:url options:options error:error];
    return d ? [self initWithData:d] : nil;
}
- (instancetype)initWithContentsOfFile:(NSString *)path {
    NSData *d = [NSData dataWithContentsOfFile:path];
    return d ? [self initWithData:d] : nil;
}
- (instancetype)initWithContentsOfFile:(NSString *)path options:(NSDataReadingOptions)options error:(NSError **)error {
    NSData *d = [NSData dataWithContentsOfFile:path options:options error:error];
    return d ? [self initWithData:d] : nil;
}
- (NSUInteger)length { return _data.length; }
- (void)setLength:(NSUInteger)length { _data.length = length; }
- (const void *)bytes { return _data.bytes; }
- (void *)mutableBytes { return _data.mutableBytes; }
- (BOOL)beginContentAccess { _accessCount++; return YES; }
- (void)endContentAccess { if (_accessCount > 0) _accessCount--; }
- (void)discardContentIfPossible {}
- (BOOL)isContentDiscarded { return NO; }
@end

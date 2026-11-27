// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

// The origin of web content (legacy WebView)
@interface WebSecurityOrigin : NSObject
@property (readonly, copy) NSString *protocol;
@property (readonly, copy) NSString *host;
@property (readonly) unsigned short port;
@property unsigned long long usage;
@property unsigned long long quota;
@end
@implementation WebSecurityOrigin
- (instancetype)initWithURL:(NSURL *)url {
    if ((self = [super init])) {
        _protocol = [url.scheme copy];
        _host = [url.host copy];
        _port = url.port.unsignedShortValue;
    }
    return self;
}
@end

NSString *const WebDatabaseDirectoryDefaultsKey __attribute__((weak)) = @"WebDatabaseDirectory";

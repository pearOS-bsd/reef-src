// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// The Music library: there is none here, so the library cannot be opened (ITLibErrorDomain, as without access).
#import <Foundation/Foundation.h>

@interface ITLibrary : NSObject
@end

@implementation ITLibrary
+ (instancetype)libraryWithAPIVersion:(NSString *)version error:(NSError **)error {
    return [self libraryWithAPIVersion:version options:0 error:error];
}
+ (instancetype)libraryWithAPIVersion:(NSString *)version options:(NSUInteger)options error:(NSError **)error {
    if (error) *error = [NSError errorWithDomain:@"ITLibErrorDomain" code:0 userInfo:nil];
    return nil;
}
- (instancetype)initWithAPIVersion:(NSString *)version error:(NSError **)error { return [ITLibrary libraryWithAPIVersion:version error:error]; }
- (instancetype)initWithAPIVersion:(NSString *)version options:(NSUInteger)options error:(NSError **)error { return [ITLibrary libraryWithAPIVersion:version options:options error:error]; }
- (NSArray *)allMediaItems { return @[]; }
- (NSArray *)allPlaylists { return @[]; }
- (NSURL *)musicFolderLocation { return nil; }
- (NSURL *)mediaFolderLocation { return nil; }
- (BOOL)reloadData { return NO; }
@end

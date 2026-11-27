// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

// QuartzFilter / QuartzFilterManager (Quartz.framework, QuartzFilters): PDF filters kept as .qfilter property lists in
// /System/Library/Filters, /Library/Filters and ~/Library/Filters.
@interface QuartzFilter : NSObject
+ (instancetype)quartzFilterWithURL:(NSURL *)url;
+ (instancetype)quartzFilterWithProperties:(NSDictionary *)properties;
- (NSURL *)url;
- (NSString *)localizedName;
- (NSDictionary *)properties;
@end

@implementation QuartzFilter {
    NSURL *_url;
    NSDictionary *_properties;
}
+ (instancetype)quartzFilterWithURL:(NSURL *)url {
    NSDictionary *d = url.isFileURL ? [NSDictionary dictionaryWithContentsOfURL:url] : nil;
    if (!d) return nil;
    QuartzFilter *f = [[self alloc] init];
    f->_url = url;
    f->_properties = d;
    return f;
}
+ (instancetype)quartzFilterWithProperties:(NSDictionary *)properties {
    QuartzFilter *f = [[self alloc] init];
    f->_properties = [properties copy];
    return f;
}
- (NSURL *)url { return _url; }
- (NSDictionary *)properties { return _properties; }
- (NSString *)localizedName {
    NSString *n = _properties[@"Name"];
    if (!n) n = [[_url lastPathComponent] stringByDeletingPathExtension];
    return n;
}
- (NSString *)description { return [NSString stringWithFormat:@"QuartzFilter:%p name:%@ URL:%@", self, [self localizedName], _url]; }
@end

@interface QuartzFilterManager : NSObject
+ (instancetype)filterManager;
+ (NSArray *)filtersInDomains:(NSArray *)domains;
@end

@implementation QuartzFilterManager
+ (instancetype)filterManager {
    static QuartzFilterManager *m;
    static dispatch_once_t once;
    dispatch_once(&once, ^{ m = [[QuartzFilterManager alloc] init]; });
    return m;
}
// The domains argument does not narrow the list on macOS 26 (observed): every filter of the three folders.
+ (NSArray *)filtersInDomains:(NSArray *)domains {
    NSMutableArray *out = [NSMutableArray array];
    NSArray *dirs = @[@"/System/Library/Filters", @"/Library/Filters", [NSHomeDirectory() stringByAppendingPathComponent:@"Library/Filters"]];
    for (NSString *dir in dirs) {
        for (NSString *n in [[[NSFileManager defaultManager] contentsOfDirectoryAtPath:dir error:NULL] sortedArrayUsingSelector:@selector(compare:)]) {
            if (![n.pathExtension isEqualToString:@"qfilter"]) continue;
            QuartzFilter *f = [QuartzFilter quartzFilterWithURL:[NSURL fileURLWithPath:[dir stringByAppendingPathComponent:n]]];
            if (f) [out addObject:f];
        }
    }
    return out;
}
@end

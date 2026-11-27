// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// QuickLookThumbnailing: requests (accessors, copy, defaults), the generator on files that do not exist (errors, cancellation, ordering of the handlers).
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <unistd.h>

struct Size { double width, height; };
@interface NSString (T)
- (const char *)UTF8String;
@end
@interface NSURL : NSObject
+ (NSURL *)fileURLWithPath:(NSString *)path;
- (NSString *)path;
@end
@interface NSError : NSObject
- (NSInteger)code;
- (NSString *)domain;
@end
@interface NSObject (T)
- (id)copy;
@end

@interface QLThumbnailGenerationRequest : NSObject
- (id)initWithFileAtURL:(NSURL *)url size:(struct Size)size scale:(double)scale representationTypes:(unsigned long)types;
- (double)minimumDimension;
- (void)setMinimumDimension:(double)value;
- (BOOL)iconMode;
- (void)setIconMode:(BOOL)value;
- (NSURL *)fileURL;
- (struct Size)size;
- (double)scale;
- (unsigned long)representationTypes;
@end
@interface QLThumbnailRepresentation : NSObject
- (long)type;
@end
@interface QLThumbnailGenerator : NSObject
+ (QLThumbnailGenerator *)sharedGenerator;
- (void)generateBestRepresentationForRequest:(QLThumbnailGenerationRequest *)request completionHandler:(void (^)(QLThumbnailRepresentation *, NSError *))handler;
- (void)generateRepresentationsForRequest:(QLThumbnailGenerationRequest *)request updateHandler:(void (^)(QLThumbnailRepresentation *, long, NSError *))handler;
- (void)cancelRequest:(id)request;
- (void)saveBestRepresentationForRequest:(QLThumbnailGenerationRequest *)request toFileAtURL:(NSURL *)url withContentType:(NSString *)type completionHandler:(void (^)(NSError *))handler;
@end
extern NSString *const QLThumbnailErrorDomain;

static void show(const char *what, NSError *error)
{
    printf("%s: %s %ld\n", what, error ? [[error domain] UTF8String] : "no error", (long)[error code]);
}

int main(void)
{
    NSURL *url = [NSURL fileURLWithPath:@"/nonexistent/file.txt"];
    struct Size size = { 128, 64 };
    QLThumbnailGenerationRequest *request = [[QLThumbnailGenerationRequest alloc] initWithFileAtURL:url size:size scale:2 representationTypes:7];
    printf("request %s %g %g %g %lu %g %d\n", [[[request fileURL] path] UTF8String], [request size].width, [request size].height, [request scale],
        [request representationTypes], [request minimumDimension], [request iconMode]);
    [request setMinimumDimension:32];
    [request setIconMode:YES];
    QLThumbnailGenerationRequest *copy = [request copy];
    printf("copy %d %g %d %g %g %g\n", copy != request, [copy minimumDimension], [copy iconMode], [copy size].width, [copy scale], (double)[copy representationTypes]);
    QLThumbnailGenerationRequest *zero = [[QLThumbnailGenerationRequest alloc] initWithFileAtURL:url size:size scale:0 representationTypes:1];
    QLThumbnailGenerationRequest *negative = [[QLThumbnailGenerationRequest alloc] initWithFileAtURL:url size:size scale:-3 representationTypes:1];
    printf("scales %g %g\n", [zero scale], [negative scale]);

    QLThumbnailGenerator *generator = [QLThumbnailGenerator sharedGenerator];
    printf("shared %d\n", generator == [QLThumbnailGenerator sharedGenerator]);
    [generator generateBestRepresentationForRequest:request completionHandler:^(QLThumbnailRepresentation *r, NSError *e) { printf("best %d\n", r != nil); show("best", e); }];
    usleep(200000);
    [generator generateRepresentationsForRequest:request updateHandler:^(QLThumbnailRepresentation *r, long type, NSError *e) { printf("update %d %ld\n", r != nil, type); show("update", e); }];
    usleep(200000);
    [generator generateRepresentationsForRequest:request updateHandler:nil];
    [generator cancelRequest:request];
    [generator generateBestRepresentationForRequest:request completionHandler:^(QLThumbnailRepresentation *r, NSError *e) { printf("cancelled %d\n", r != nil); show("cancelled", e); }];
    usleep(200000);
    [generator cancelRequest:request];
    [generator generateRepresentationsForRequest:request updateHandler:^(QLThumbnailRepresentation *r, long type, NSError *e) { printf("cancelled update %d %ld\n", r != nil, type); show("cancelled update", e); }];
    usleep(200000);
    [generator generateBestRepresentationForRequest:request completionHandler:^(QLThumbnailRepresentation *r, NSError *e) { printf("again %d\n", r != nil); show("again", e); }];
    usleep(200000);
    NSURL *target = [NSURL fileURLWithPath:@"/tmp/ql-thumbnail-test.png"];
    [generator saveBestRepresentationForRequest:request toFileAtURL:target withContentType:@"public.png" completionHandler:^(NSError *e) { show("save", e); }];
    usleep(200000);
    [generator saveBestRepresentationForRequest:request toFileAtURL:target withContentType:@"public.png" completionHandler:nil];
    usleep(200000);
    printf("domain %s\n", [QLThumbnailErrorDomain UTF8String]);
    return 0;
}

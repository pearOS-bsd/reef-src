// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Photos: fetch results of the empty library, options and their copies, resources, cloud identifiers, the photo library and the image manager answering with errors.
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <unistd.h>

static CGRect CGRectMake(double x, double y, double width, double height)
{
    CGRect rect = {{x, y}, {width, height}};
    return rect;
}
static CGSize CGSizeMake(double width, double height)
{
    CGSize size = {width, height};
    return size;
}
@interface NSDate : NSObject
+ (id)date;
@end

@interface NSString (T)
- (const char *)UTF8String;
@end
@interface NSError : NSObject
- (NSString *)domain;
- (NSInteger)code;
@end
@interface NSObject (T)
- (NSUInteger)count;
- (id)objectForKey:(id)key;
- (int)intValue;
@end
@interface NSException : NSObject
- (NSString *)name;
- (NSString *)reason;
@end

@interface PHObject : NSObject <NSCopying>
@property (readonly, copy) NSString *localIdentifier;
@end
@interface PHFetchOptions : NSObject <NSCopying>
@property (strong) id predicate;
@property (copy) NSArray *sortDescriptors;
@property BOOL includeHiddenAssets;
@property BOOL includeAllBurstAssets;
@property unsigned long includeAssetSourceTypes;
@property unsigned long fetchLimit;
@property BOOL wantsIncrementalChangeDetails;
@end
@interface PHFetchResult : NSObject <NSCopying>
@property (readonly) unsigned long count;
@property (readonly, nonatomic) id firstObject;
@property (readonly, nonatomic) id lastObject;
+ (id)emptyResult;
- (id)objectAtIndex:(unsigned long)index;
- (id)objectAtIndexedSubscript:(unsigned long)index;
- (BOOL)containsObject:(id)object;
- (unsigned long)indexOfObject:(id)object;
- (id)objectsAtIndexes:(id)indexes;
- (unsigned long)countOfAssetsWithMediaType:(long)mediaType;
@end
@interface NSIndexSet : NSObject
+ (id)indexSet;
+ (id)indexSetWithIndex:(unsigned long)index;
@end
@interface PHAsset : PHObject
@property (readonly, nonatomic) long mediaType;
@property (readonly, nonatomic) unsigned long pixelWidth;
@property (readonly, nonatomic) unsigned long pixelHeight;
@property (readonly, nonatomic) double duration;
@property (readonly, nonatomic, getter=isHidden) BOOL hidden;
@property (readonly, nonatomic, getter=isFavorite) BOOL favorite;
+ (id)fetchAssetsWithOptions:(PHFetchOptions *)options;
+ (id)fetchAssetsInAssetCollection:(id)collection options:(PHFetchOptions *)options;
+ (id)fetchAssetsWithMediaType:(long)mediaType options:(PHFetchOptions *)options;
+ (id)fetchAssetsWithLocalIdentifiers:(NSArray *)identifiers options:(PHFetchOptions *)options;
- (unsigned long)requestContentEditingInputWithOptions:(id)options completionHandler:(void (^)(id input, NSDictionary *info))handler;
- (void)cancelContentEditingInputRequest:(unsigned long)request;
@end
@interface PHContentEditingInputRequestOptions : NSObject
@property (copy) id canHandleAdjustmentData;
@property (getter=isNetworkAccessAllowed) BOOL networkAccessAllowed;
@property (copy) id progressHandler;
@end
@interface PHAssetCollection : PHObject
@property (readonly, nonatomic) long assetCollectionType;
@property (readonly, nonatomic) NSString *localizedTitle;
+ (id)fetchAssetCollectionsWithType:(long)type subtype:(long)subtype options:(PHFetchOptions *)options;
+ (id)fetchAssetCollectionsWithLocalIdentifiers:(NSArray *)identifiers options:(PHFetchOptions *)options;
@end
@interface PHAssetResource : NSObject
+ (id)assetResourcesForAsset:(PHAsset *)asset;
@end
@interface PHAssetResourceRequestOptions : NSObject <NSCopying>
@property (nonatomic, getter=isNetworkAccessAllowed) BOOL networkAccessAllowed;
@property (copy, nonatomic) void (^progressHandler)(double progress);
@end
@interface PHAssetResourceManager : NSObject
+ (id)defaultManager;
- (int)requestDataForAssetResource:(id)resource options:(id)options dataReceivedHandler:(void (^)(id data))dataHandler completionHandler:(void (^)(NSError *error))completionHandler;
- (void)writeDataForAssetResource:(id)resource toFile:(id)file options:(id)options completionHandler:(void (^)(NSError *error))completionHandler;
- (void)cancelDataRequest:(int)request;
@end
@interface PHCloudIdentifier : NSObject
@property (readonly, nonatomic) NSString *stringValue;
+ (BOOL)supportsSecureCoding;
- (id)initWithStringValue:(NSString *)stringValue;
@end
@interface PHImageRequestOptions : NSObject <NSCopying>
@property (nonatomic) long version;
@property (nonatomic) long deliveryMode;
@property (nonatomic) long resizeMode;
@property (nonatomic) CGRect normalizedCropRect;
@property (nonatomic, getter=isNetworkAccessAllowed) BOOL networkAccessAllowed;
@property (nonatomic, getter=isSynchronous) BOOL synchronous;
@property (copy, nonatomic) id progressHandler;
@end
@interface PHImageManager : NSObject
+ (id)defaultManager;
- (int)requestImageForAsset:(PHAsset *)asset targetSize:(CGSize)targetSize contentMode:(long)contentMode options:(PHImageRequestOptions *)options resultHandler:(void (^)(id image, NSDictionary *info))handler;
- (int)requestImageDataAndOrientationForAsset:(PHAsset *)asset options:(PHImageRequestOptions *)options resultHandler:(void (^)(id data, NSString *type, unsigned int orientation, NSDictionary *info))handler;
- (void)cancelImageRequest:(int)request;
@end
@interface PHObjectPlaceholder : PHObject
@end
@interface PHAssetChangeRequest : NSObject
@property (readonly, nonatomic) PHObjectPlaceholder *placeholderForCreatedAsset;
@property (strong, nonatomic) id creationDate;
@property (nonatomic, getter=isFavorite) BOOL favorite;
+ (id)creationRequest;
+ (id)creationRequestForAssetFromImage:(id)image;
+ (id)creationRequestForAssetFromImageAtFileURL:(id)url;
+ (id)creationRequestForAssetFromVideoAtFileURL:(id)url;
+ (void)deleteAssets:(id)assets;
@end
@interface PHChange : NSObject
- (id)changeDetailsForObject:(PHObject *)object;
- (id)changeDetailsForFetchResult:(PHFetchResult *)fetchResult;
@end
@interface PHObjectChangeDetails : NSObject
@property (readonly) id objectBeforeChanges;
@property (readonly) id objectAfterChanges;
@property (readonly) BOOL assetContentChanged;
@property (readonly) BOOL objectWasDeleted;
@end
@interface PHFetchResultChangeDetails : NSObject
@property (readonly) id fetchResultBeforeChanges;
@property (readonly) BOOL hasIncrementalChanges;
@property (readonly) id removedIndexes;
@property (readonly) id changedObjects;
@property (readonly) BOOL hasMoves;
@end
@interface PHPhotoLibrary : NSObject
+ (id)sharedPhotoLibrary;
+ (long)authorizationStatus;
+ (long)authorizationStatusForAccessLevel:(long)accessLevel;
+ (void)requestAuthorization:(void (^)(long status))handler;
+ (void)requestAuthorizationForAccessLevel:(long)accessLevel handler:(void (^)(long status))handler;
- (void)performChanges:(void (^)(void))changeBlock completionHandler:(void (^)(BOOL success, NSError *error))completionHandler;
- (BOOL)performChangesAndWait:(void (^)(void))changeBlock error:(NSError **)error;
- (void)registerChangeObserver:(id)observer;
- (void)unregisterChangeObserver:(id)observer;
@end
extern NSString *const PHPhotosErrorDomain;
extern NSString *const PHImageErrorKey;
extern NSString *const PHImageResultRequestIDKey;
extern NSString *const PHImageCancelledKey;

static volatile int arrived;

static void wait(int count)
{
    for (int i = 0; i < 100 && arrived < count; i++)
        usleep(20000);
    usleep(20000);
}

int main(void)
{
    PHObject *object = [PHObject new];
    PHObject *other = [PHObject new];
    printf("object %d %d %d\n", [object.localIdentifier length] == 36, ![object.localIdentifier isEqual:other.localIdentifier], [object copy] == object);

    PHFetchOptions *options = [PHFetchOptions new];
    options.sortDescriptors = @[@"a"]; options.includeHiddenAssets = YES; options.includeAllBurstAssets = YES; options.includeAssetSourceTypes = 5; options.fetchLimit = 7; options.wantsIncrementalChangeDetails = YES;
    PHFetchOptions *copy = [options copy];
    printf("options %d %lu %d %d %lu %lu %d %d\n", copy != options, (unsigned long)[copy.sortDescriptors count], copy.includeHiddenAssets, copy.includeAllBurstAssets, copy.includeAssetSourceTypes, copy.fetchLimit, copy.wantsIncrementalChangeDetails, copy.predicate == nil);

    PHFetchResult *result = [PHAsset fetchAssetsWithOptions:options];
    printf("result %lu %d %d %d %lu %lu %d\n", result.count, result.firstObject == nil, result.lastObject == nil, [result containsObject:@"x"], [result indexOfObject:@"x"], [result countOfAssetsWithMediaType:1], [result copy] == result);
    printf("others %lu %lu %lu %lu\n", [[PHAsset fetchAssetsInAssetCollection:nil options:nil] count], [[PHAsset fetchAssetsWithMediaType:1 options:nil] count], [[PHAsset fetchAssetsWithLocalIdentifiers:@[@"x"] options:nil] count],
           [[PHAssetCollection fetchAssetCollectionsWithType:1 subtype:2 options:nil] count] + [[PHAssetCollection fetchAssetCollectionsWithLocalIdentifiers:nil options:nil] count]);
    printf("indexes %lu %lu\n", [[result objectsAtIndexes:[NSIndexSet indexSet]] count], (unsigned long)(result == [PHFetchResult emptyResult]));
    @try { [result objectAtIndex:3]; } @catch (NSException *e) { printf("exception %s %s\n", [[e name] UTF8String], [[e reason] UTF8String]); }
    @try { [result objectAtIndexedSubscript:5]; } @catch (NSException *e) { printf("exception %s\n", [[e reason] UTF8String]); }
    @try { [result objectsAtIndexes:[NSIndexSet indexSetWithIndex:1]]; } @catch (NSException *e) { printf("exception %s\n", [[e reason] UTF8String]); }

    PHAsset *asset = [PHAsset new];
    printf("asset %ld %lu %lu %g %d %d\n", asset.mediaType, asset.pixelWidth, asset.pixelHeight, asset.duration, asset.hidden, asset.favorite);
    PHAssetCollection *collection = [PHAssetCollection new];
    printf("collection %ld %d\n", collection.assetCollectionType, collection.localizedTitle == nil);
    printf("resources %lu\n", [[PHAssetResource assetResourcesForAsset:asset] count]);
    PHContentEditingInputRequestOptions *edit = [PHContentEditingInputRequestOptions new];
    edit.networkAccessAllowed = YES; edit.progressHandler = ^(double p, BOOL *s) { }; edit.canHandleAdjustmentData = ^BOOL(id d) { return YES; };
    printf("edit %d %d %d\n", edit.networkAccessAllowed, edit.progressHandler != nil, edit.canHandleAdjustmentData != nil);
    [asset cancelContentEditingInputRequest:1];
    unsigned long request = [asset requestContentEditingInputWithOptions:edit completionHandler:^(id input, NSDictionary *info) {
        NSError *e = [info objectForKey:PHImageErrorKey];
        printf("editing %d %s %ld\n", input == nil, [e.domain UTF8String], (long)e.code);
        arrived++;
    }];
    printf("request %lu\n", request);
    usleep(100000);

    PHAssetResourceRequestOptions *ro = [PHAssetResourceRequestOptions new];
    ro.networkAccessAllowed = YES; ro.progressHandler = ^(double p) { };
    PHAssetResourceRequestOptions *roc = [ro copy];
    printf("resource options %d %d %d\n", roc != ro, roc.networkAccessAllowed, roc.progressHandler != nil);
    PHAssetResourceManager *manager = [PHAssetResourceManager defaultManager];
    printf("manager %d\n", manager == [PHAssetResourceManager defaultManager]);
    int r1 = [manager requestDataForAssetResource:nil options:ro dataReceivedHandler:nil completionHandler:^(NSError *e) {
        printf("resource data %s %ld\n", [e.domain UTF8String], (long)e.code);
        arrived++;
    }];
    wait(1);
    int r2 = [manager requestDataForAssetResource:nil options:nil dataReceivedHandler:nil completionHandler:^(NSError *e) { printf("resource data 2 %ld\n", (long)e.code); arrived++; }];
    wait(2);
    [manager writeDataForAssetResource:nil toFile:nil options:nil completionHandler:^(NSError *e) {
        printf("resource write %ld\n", (long)e.code);
        arrived++;
    }];
    wait(3);
    [manager cancelDataRequest:r1];
    printf("resource requests %d %d\n", r1, r2);

    PHCloudIdentifier *cloud = [[PHCloudIdentifier alloc] initWithStringValue:@"abc"];
    printf("cloud %s %d\n", [cloud.stringValue UTF8String], [PHCloudIdentifier supportsSecureCoding]);

    PHImageRequestOptions *io = [PHImageRequestOptions new];
    printf("image options %ld %ld %ld %d %d %g %g %g %g\n", io.version, io.deliveryMode, io.resizeMode, io.networkAccessAllowed, io.synchronous, io.normalizedCropRect.origin.x, io.normalizedCropRect.origin.y, io.normalizedCropRect.size.width, io.normalizedCropRect.size.height);
    io.version = 1; io.deliveryMode = 2; io.resizeMode = 3; io.networkAccessAllowed = YES; io.normalizedCropRect = CGRectMake(0.1, 0.2, 0.3, 0.4); io.progressHandler = ^(double p, NSError *e, BOOL *s, NSDictionary *i) { };
    PHImageRequestOptions *ioc = [io copy];
    printf("image options %d %ld %ld %ld %d %d %g %g %g %g %d\n", ioc != io, ioc.version, ioc.deliveryMode, ioc.resizeMode, ioc.networkAccessAllowed, ioc.synchronous, ioc.normalizedCropRect.origin.x, ioc.normalizedCropRect.origin.y, ioc.normalizedCropRect.size.width, ioc.normalizedCropRect.size.height, ioc.progressHandler != nil);
    PHImageManager *images = [PHImageManager defaultManager];
    printf("images %d\n", images == [PHImageManager defaultManager]);
    int i1 = [images requestImageForAsset:asset targetSize:CGSizeMake(10, 10) contentMode:0 options:nil resultHandler:^(id image, NSDictionary *info) {
        NSError *e = [info objectForKey:PHImageErrorKey];
        printf("image %d %s %ld %d\n", image == nil, [e.domain UTF8String], (long)e.code, [[info objectForKey:PHImageResultRequestIDKey] intValue]);
        arrived++;
    }];
    usleep(100000);
    io.synchronous = YES;
    int i2 = [images requestImageForAsset:asset targetSize:CGSizeMake(10, 10) contentMode:0 options:io resultHandler:^(id image, NSDictionary *info) {
        printf("image sync %d %d\n", image == nil, [[info objectForKey:PHImageResultRequestIDKey] intValue]);
    }];
    int i3 = [images requestImageForAsset:asset targetSize:CGSizeMake(10, 10) contentMode:0 options:nil resultHandler:nil];
    int i4 = [images requestImageDataAndOrientationForAsset:asset options:nil resultHandler:^(id data, NSString *type, unsigned int orientation, NSDictionary *info) {
        printf("data %d %d %u %ld\n", data == nil, type == nil, orientation, (long)[(NSError *)[info objectForKey:PHImageErrorKey] code]);
        arrived++;
    }];
    usleep(100000);
    int i5 = [images requestImageDataAndOrientationForAsset:asset options:nil resultHandler:nil];
    [images cancelImageRequest:i1];
    printf("image requests %d %d %d %d %d\n", i1, i2, i3, i4, i5);

    PHAssetChangeRequest *creation = [PHAssetChangeRequest creationRequest];
    printf("creation %d %d %d\n", creation.placeholderForCreatedAsset != nil, creation.creationDate == nil, creation.favorite);
    creation.creationDate = [NSDate date]; creation.favorite = YES;
    printf("creation %d %d\n", creation.creationDate != nil, creation.favorite);
    printf("requests %d %d %d\n", [PHAssetChangeRequest creationRequestForAssetFromImage:nil] != nil, [PHAssetChangeRequest creationRequestForAssetFromImageAtFileURL:nil] != nil, [PHAssetChangeRequest creationRequestForAssetFromVideoAtFileURL:nil] != nil);
    [PHAssetChangeRequest deleteAssets:nil];
    PHChange *change = [PHChange new];
    printf("change %d %d\n", [change changeDetailsForObject:object] == nil, [change changeDetailsForFetchResult:result] == nil);
    PHObjectChangeDetails *oc = [PHObjectChangeDetails new];
    printf("details %d %d %d %d\n", oc.objectBeforeChanges == nil, oc.objectAfterChanges == nil, oc.assetContentChanged, oc.objectWasDeleted);
    PHFetchResultChangeDetails *fc = [PHFetchResultChangeDetails new];
    printf("details %d %d %d %d %d\n", fc.fetchResultBeforeChanges == nil, fc.hasIncrementalChanges, fc.removedIndexes == nil, fc.changedObjects == nil, fc.hasMoves);

    PHPhotoLibrary *library = [PHPhotoLibrary sharedPhotoLibrary];
    printf("library %d %ld %ld\n", library == [PHPhotoLibrary sharedPhotoLibrary], [PHPhotoLibrary authorizationStatus], [PHPhotoLibrary authorizationStatusForAccessLevel:2]);
    [library registerChangeObserver:nil]; [library unregisterChangeObserver:nil];
    [PHPhotoLibrary requestAuthorization:^(long s) { printf("authorization %ld\n", s); arrived++; }];
    wait(4);
    [PHPhotoLibrary requestAuthorizationForAccessLevel:1 handler:^(long s) { printf("authorization2 %ld\n", s); arrived++; }];
    wait(5);
    [PHPhotoLibrary requestAuthorization:nil];
    [library performChanges:^{ printf("changes\n"); } completionHandler:^(BOOL ok, NSError *e) { printf("done %d %s %ld\n", ok, [e.domain UTF8String], (long)e.code); arrived++; }];
    wait(6);
    [library performChanges:nil completionHandler:nil];
    usleep(100000);
    NSError *error = nil;
    BOOL ok = [library performChangesAndWait:^{ printf("changes wait\n"); } error:&error];
    printf("wait %d %ld\n", ok, (long)error.code);
    printf("wait %d\n", [library performChangesAndWait:nil error:NULL]);
    printf("domain %s %s\n", [PHPhotosErrorDomain UTF8String], [PHImageCancelledKey UTF8String]);
    return 0;
}

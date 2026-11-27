// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Photos: the photo library model (assets, collections, resources, fetch results, options and requests) over an empty library;
// every request is answered with a "not authorized" error.

#import <Foundation/Foundation.h>
#include <dispatch/dispatch.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
@end

@interface NSError : NSObject
+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code userInfo:(NSDictionary *)userInfo;
@end
@interface NSUUID : NSObject
+ (id)UUID;
- (NSString *)UUIDString;
@end
@interface NSException : NSObject
+ (void)raise:(NSString *)name format:(NSString *)format, ...;
@end
@interface NSArray (Photos)
+ (id)array;
@end
@interface NSCoder : NSObject
- (id)decodeObjectOfClass:(Class)cls forKey:(NSString *)key;
- (void)encodeObject:(id)object forKey:(NSString *)key;
@end
@interface NSString (Photos)
+ (Class)class;
@end
@class NSPredicate, NSDate, NSIndexSet, NSURL;

extern NSString *const NSRangeException;
extern const CGRect CGRectZero;

typedef struct {
    unsigned long state;
    id *itemsPtr;
    unsigned long *mutationsPtr;
    unsigned long extra[5];
} PHFastEnumerationState;

@protocol NSFastEnumeration
- (unsigned long)countByEnumeratingWithState:(PHFastEnumerationState *)state objects:(id *)buffer count:(unsigned long)length;
@end

extern NSString *const PHPhotosErrorDomain;
extern NSString *const PHImageResultIsInCloudKey;
extern NSString *const PHImageResultIsDegradedKey;
extern NSString *const PHImageResultRequestIDKey;
extern NSString *const PHImageCancelledKey;
extern NSString *const PHImageErrorKey;

// Runs the block later, on a global queue, and gives it up afterwards.
static void runLater(void (^block)(void))
{
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        block();
        [block release];
    });
}

@interface PHObject : NSObject <NSCopying>
{
    NSString *_localIdentifier;
}
@property (readonly, copy) NSString *localIdentifier;
@end

@implementation PHObject

- (id)init
{
    self = [super init];
    if (self)
        _localIdentifier = [[[NSUUID UUID] UUIDString] copy];
    return self;
}

- (void)dealloc
{
    [_localIdentifier release];
    [super dealloc];
}

- (NSString *)localIdentifier
{
    return _localIdentifier;
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

@end

@interface PHObjectPlaceholder : PHObject
@end

@implementation PHObjectPlaceholder
@end

@interface PHFetchOptions : NSObject <NSCopying>
{
    BOOL _includeHiddenAssets;
    BOOL _includeAllBurstAssets;
    BOOL _wantsIncrementalChangeDetails;
    NSPredicate *_predicate;
    NSArray *_sortDescriptors;
    unsigned long _includeAssetSourceTypes;
    unsigned long _fetchLimit;
}
@property (strong) NSPredicate *predicate;
@property (copy) NSArray *sortDescriptors;
@property BOOL includeHiddenAssets;
@property BOOL includeAllBurstAssets;
@property unsigned long includeAssetSourceTypes;
@property unsigned long fetchLimit;
@property BOOL wantsIncrementalChangeDetails;
@end

@implementation PHFetchOptions

- (void)dealloc
{
    [_predicate release];
    [_sortDescriptors release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
    PHFetchOptions *copy = [[[self class] allocWithZone:zone] init];
    [copy setPredicate:_predicate];
    [copy setSortDescriptors:_sortDescriptors];
    [copy setIncludeHiddenAssets:_includeHiddenAssets];
    [copy setIncludeAllBurstAssets:_includeAllBurstAssets];
    [copy setIncludeAssetSourceTypes:_includeAssetSourceTypes];
    [copy setFetchLimit:_fetchLimit];
    [copy setWantsIncrementalChangeDetails:_wantsIncrementalChangeDetails];
    return copy;
}

@end

@interface PHFetchResult : NSObject <NSCopying, NSFastEnumeration>
@property (readonly) unsigned long count;
@property (readonly, nonatomic) id firstObject;
@property (readonly, nonatomic) id lastObject;
+ (id)emptyResult;
- (id)objectAtIndex:(unsigned long)index;
- (id)objectAtIndexedSubscript:(unsigned long)index;
- (BOOL)containsObject:(id)object;
- (unsigned long)indexOfObject:(id)object;
- (id)objectsAtIndexes:(NSIndexSet *)indexes;
- (void)enumerateObjectsUsingBlock:(void (^)(id object, unsigned long index, BOOL *stop))block;
- (unsigned long)countOfAssetsWithMediaType:(long)mediaType;
@end

@interface NSIndexSet : NSObject
- (unsigned long)count;
@end

@implementation PHFetchResult

+ (id)emptyResult
{
    return [[[self alloc] init] autorelease];
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

- (unsigned long)count
{
    return 0;
}

- (id)objectAtIndex:(unsigned long)index
{
    [NSException raise:NSRangeException format:@"index %lu beyond bounds of an empty fetch result", index];
    return nil;
}

- (id)objectAtIndexedSubscript:(unsigned long)index
{
    return [self objectAtIndex:index];
}

- (BOOL)containsObject:(id)object
{
    return NO;
}

- (unsigned long)indexOfObject:(id)object
{
    return 0x7fffffffffffffffUL;
}

- (id)objectsAtIndexes:(NSIndexSet *)indexes
{
    if ([indexes count])
        [NSException raise:NSRangeException format:@"indexes beyond bounds of an empty fetch result"];
    return [NSArray array];
}

- (id)firstObject
{
    return nil;
}

- (id)lastObject
{
    return nil;
}

- (void)enumerateObjectsUsingBlock:(void (^)(id object, unsigned long index, BOOL *stop))block
{
}

- (unsigned long)countOfAssetsWithMediaType:(long)mediaType
{
    return 0;
}

- (unsigned long)countByEnumeratingWithState:(PHFastEnumerationState *)state objects:(id *)buffer count:(unsigned long)length
{
    return 0;
}

@end

@interface PHAsset : PHObject
{
    BOOL _hidden;
    BOOL _favorite;
    long _mediaType;
    unsigned long _pixelWidth;
    unsigned long _pixelHeight;
    NSDate *_creationDate;
    NSDate *_modificationDate;
    double _duration;
}
@property (readonly, nonatomic) long mediaType;
@property (readonly, nonatomic) unsigned long pixelWidth;
@property (readonly, nonatomic) unsigned long pixelHeight;
@property (readonly, nonatomic) NSDate *creationDate;
@property (readonly, nonatomic) NSDate *modificationDate;
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

@implementation PHAsset

+ (id)fetchAssetsWithOptions:(PHFetchOptions *)options
{
    return [PHFetchResult emptyResult];
}

+ (id)fetchAssetsInAssetCollection:(id)collection options:(PHFetchOptions *)options
{
    return [PHFetchResult emptyResult];
}

+ (id)fetchAssetsWithMediaType:(long)mediaType options:(PHFetchOptions *)options
{
    return [PHFetchResult emptyResult];
}

+ (id)fetchAssetsWithLocalIdentifiers:(NSArray *)identifiers options:(PHFetchOptions *)options
{
    return [PHFetchResult emptyResult];
}

- (unsigned long)requestContentEditingInputWithOptions:(id)options completionHandler:(void (^)(id input, NSDictionary *info))handler
{
    if (handler) {
        void (^completion)(id input, NSDictionary *info) = [handler copy];
        dispatch_async(dispatch_get_main_queue(), ^{
            completion(nil, @{PHImageErrorKey: [NSError errorWithDomain:@"PHPhotosErrorDomain" code:3303 userInfo:nil]});
            [completion release];
        });
    }
    return 1;
}

- (void)cancelContentEditingInputRequest:(unsigned long)request
{
}

@end

@interface PHContentEditingInputRequestOptions : NSObject
{
    BOOL _networkAccessAllowed;
    BOOL (^_canHandleAdjustmentData)(id adjustmentData);
    void (^_progressHandler)(double progress, BOOL *stop);
}
@property (copy) BOOL (^canHandleAdjustmentData)(id adjustmentData);
@property (getter=isNetworkAccessAllowed) BOOL networkAccessAllowed;
@property (copy) void (^progressHandler)(double progress, BOOL *stop);
@end

@implementation PHContentEditingInputRequestOptions

- (void)dealloc
{
    [_canHandleAdjustmentData release];
    [_progressHandler release];
    [super dealloc];
}

@end

@interface PHContentEditingInput : NSObject
{
    NSURL *_fullSizeImageURL;
    long _mediaType;
}
@property (readonly, copy) NSURL *fullSizeImageURL;
@property (readonly) long mediaType;
@end

@implementation PHContentEditingInput
@end

@interface PHCollection : PHObject
{
    NSString *_localizedTitle;
}
@property (readonly, nonatomic) NSString *localizedTitle;
@end

@implementation PHCollection
@end

@interface PHAssetCollection : PHCollection
{
    long _assetCollectionType;
    long _assetCollectionSubtype;
    unsigned long _estimatedAssetCount;
}
@property (readonly, nonatomic) long assetCollectionType;
@property (readonly, nonatomic) long assetCollectionSubtype;
@property (readonly, nonatomic) unsigned long estimatedAssetCount;
+ (id)fetchAssetCollectionsWithType:(long)type subtype:(long)subtype options:(PHFetchOptions *)options;
+ (id)fetchAssetCollectionsWithLocalIdentifiers:(NSArray *)identifiers options:(PHFetchOptions *)options;
@end

@implementation PHAssetCollection

+ (id)fetchAssetCollectionsWithType:(long)type subtype:(long)subtype options:(PHFetchOptions *)options
{
    return [PHFetchResult emptyResult];
}

+ (id)fetchAssetCollectionsWithLocalIdentifiers:(NSArray *)identifiers options:(PHFetchOptions *)options
{
    return [PHFetchResult emptyResult];
}

@end

@interface PHAssetResource : NSObject
{
    long _type;
    NSString *_assetLocalIdentifier;
    NSString *_uniformTypeIdentifier;
    NSString *_originalFilename;
}
@property (readonly, nonatomic) long type;
@property (readonly, copy, nonatomic) NSString *assetLocalIdentifier;
@property (readonly, copy, nonatomic) NSString *uniformTypeIdentifier;
@property (readonly, copy, nonatomic) NSString *originalFilename;
+ (id)assetResourcesForAsset:(PHAsset *)asset;
@end

@implementation PHAssetResource

+ (id)assetResourcesForAsset:(PHAsset *)asset
{
    return [NSArray array];
}

@end

@interface PHAssetResourceRequestOptions : NSObject <NSCopying>
{
    BOOL _networkAccessAllowed;
    void (^_progressHandler)(double progress);
}
@property (nonatomic, getter=isNetworkAccessAllowed) BOOL networkAccessAllowed;
@property (copy, nonatomic) void (^progressHandler)(double progress);
@end

@implementation PHAssetResourceRequestOptions

- (void)dealloc
{
    [_progressHandler release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
    PHAssetResourceRequestOptions *copy = [[[self class] allocWithZone:zone] init];
    [copy setNetworkAccessAllowed:_networkAccessAllowed];
    [copy setProgressHandler:_progressHandler];
    return copy;
}

@end

@interface PHAssetResourceManager : NSObject
{
    int _lastRequest;
}
+ (id)defaultManager;
- (int)requestDataForAssetResource:(PHAssetResource *)resource options:(PHAssetResourceRequestOptions *)options dataReceivedHandler:(void (^)(NSData *data))dataHandler completionHandler:(void (^)(NSError *error))completionHandler;
- (void)writeDataForAssetResource:(PHAssetResource *)resource toFile:(NSURL *)file options:(PHAssetResourceRequestOptions *)options completionHandler:(void (^)(NSError *error))completionHandler;
- (void)cancelDataRequest:(int)request;
@end

typedef long dispatch_once_t;
extern void dispatch_once(dispatch_once_t *predicate, void (^block)(void));
// The SDK's inline fast path in front of the library function.
static inline void _dispatch_once(dispatch_once_t *predicate, void (^block)(void))
{
    if (__builtin_expect(*predicate, ~0l) != ~0l)
        dispatch_once(predicate, block);
}
#define dispatch_once _dispatch_once

@implementation PHAssetResourceManager

+ (id)defaultManager
{
    static PHAssetResourceManager *manager;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        manager = [[PHAssetResourceManager alloc] init];
    });
    return manager;
}

- (int)requestDataForAssetResource:(PHAssetResource *)resource options:(PHAssetResourceRequestOptions *)options dataReceivedHandler:(void (^)(NSData *data))dataHandler completionHandler:(void (^)(NSError *error))completionHandler
{
    int request;
    @synchronized(self) {
        request = ++_lastRequest;
    }
    [self writeDataForAssetResource:resource toFile:nil options:options completionHandler:completionHandler];
    return request;
}

- (void)writeDataForAssetResource:(PHAssetResource *)resource toFile:(NSURL *)file options:(PHAssetResourceRequestOptions *)options completionHandler:(void (^)(NSError *error))completionHandler
{
    void (^completion)(NSError *error) = [completionHandler copy];
    dispatch_async(dispatch_get_global_queue(0x15, 0), ^{
        completion([NSError errorWithDomain:@"PHPhotosErrorDomain" code:3303 userInfo:nil]);
        [completion release];
    });
}

- (void)cancelDataRequest:(int)request
{
}

@end

@interface PHCloudIdentifier : NSObject <NSSecureCoding>
{
    NSString *_stringValue;
}
@property (readonly, nonatomic) NSString *stringValue;
- (id)initWithStringValue:(NSString *)stringValue;
@end

@implementation PHCloudIdentifier

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (id)initWithStringValue:(NSString *)stringValue
{
    self = [super init];
    if (self)
        _stringValue = [stringValue copy];
    return self;
}

- (id)initWithCoder:(NSCoder *)coder
{
    NSString *value = [coder decodeObjectOfClass:[NSString class] forKey:@"stringValue"];
    return [self initWithStringValue:value ?: @""];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:_stringValue forKey:@"stringValue"];
}

- (void)dealloc
{
    [_stringValue release];
    [super dealloc];
}

- (NSString *)stringValue
{
    return _stringValue;
}

@end

@interface PHImageRequestOptions : NSObject <NSCopying>
{
    BOOL _networkAccessAllowed;
    BOOL _synchronous;
    long _version;
    long _deliveryMode;
    long _resizeMode;
    void (^_progressHandler)(double progress, NSError *error, BOOL *stop, NSDictionary *info);
    CGRect _normalizedCropRect;
}
@property (nonatomic) long version;
@property (nonatomic) long deliveryMode;
@property (nonatomic) long resizeMode;
@property (nonatomic) CGRect normalizedCropRect;
@property (nonatomic, getter=isNetworkAccessAllowed) BOOL networkAccessAllowed;
@property (nonatomic, getter=isSynchronous) BOOL synchronous;
@property (copy, nonatomic) void (^progressHandler)(double progress, NSError *error, BOOL *stop, NSDictionary *info);
@end

@implementation PHImageRequestOptions

- (id)init
{
    self = [super init];
    if (self)
        _normalizedCropRect = CGRectZero;
    return self;
}

- (void)dealloc
{
    [_progressHandler release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
    PHImageRequestOptions *copy = [[[self class] allocWithZone:zone] init];
    [copy setVersion:_version];
    [copy setDeliveryMode:_deliveryMode];
    [copy setResizeMode:_resizeMode];
    [copy setNormalizedCropRect:_normalizedCropRect];
    [copy setNetworkAccessAllowed:_networkAccessAllowed];
    [copy setSynchronous:_synchronous];
    [copy setProgressHandler:_progressHandler];
    return copy;
}

@end

@interface PHImageManager : NSObject
{
    int _lastRequest;
}
+ (id)defaultManager;
- (int)nextRequest;
- (int)requestImageForAsset:(PHAsset *)asset targetSize:(CGSize)targetSize contentMode:(long)contentMode options:(PHImageRequestOptions *)options resultHandler:(void (^)(id image, NSDictionary *info))handler;
- (int)requestImageDataAndOrientationForAsset:(PHAsset *)asset options:(PHImageRequestOptions *)options resultHandler:(void (^)(NSData *data, NSString *type, unsigned int orientation, NSDictionary *info))handler;
- (void)cancelImageRequest:(int)request;
@end

@implementation PHImageManager

+ (id)defaultManager
{
    static PHImageManager *manager;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        manager = [[PHImageManager alloc] init];
    });
    return manager;
}

- (int)nextRequest
{
    int request;
    @synchronized(self) {
        request = ++_lastRequest;
    }
    return request;
}

- (int)requestImageForAsset:(PHAsset *)asset targetSize:(CGSize)targetSize contentMode:(long)contentMode options:(PHImageRequestOptions *)options resultHandler:(void (^)(id image, NSDictionary *info))handler
{
    int request = [self nextRequest];
    if (handler) {
        NSDictionary *info = @{PHImageErrorKey: [NSError errorWithDomain:@"PHPhotosErrorDomain" code:3303 userInfo:nil], PHImageResultRequestIDKey: [NSNumber numberWithInt:request]};
        if ([options isSynchronous]) {
            handler(nil, info);
        } else {
            void (^completion)(id image, NSDictionary *info) = [handler copy];
            [info retain];
            dispatch_async(dispatch_get_main_queue(), ^{
                completion(nil, info);
                [info release];
                [completion release];
            });
        }
    }
    return request;
}

- (int)requestImageDataAndOrientationForAsset:(PHAsset *)asset options:(PHImageRequestOptions *)options resultHandler:(void (^)(NSData *data, NSString *type, unsigned int orientation, NSDictionary *info))handler
{
    int request = [self nextRequest];
    if (handler) {
        void (^completion)(NSData *data, NSString *type, unsigned int orientation, NSDictionary *info) = [handler copy];
        dispatch_async(dispatch_get_main_queue(), ^{
            completion(nil, nil, 1, @{PHImageErrorKey: [NSError errorWithDomain:@"PHPhotosErrorDomain" code:3303 userInfo:nil]});
            [completion release];
        });
    }
    return request;
}

- (void)cancelImageRequest:(int)request
{
}

@end

@interface PHChangeRequest : NSObject
@end

@implementation PHChangeRequest
@end

@interface PHAssetChangeRequest : PHChangeRequest
{
    BOOL _favorite;
    PHObjectPlaceholder *_placeholderForCreatedAsset;
    NSDate *_creationDate;
}
@property (readonly, nonatomic) PHObjectPlaceholder *placeholderForCreatedAsset;
@property (strong, nonatomic) NSDate *creationDate;
@property (nonatomic, getter=isFavorite) BOOL favorite;
+ (id)creationRequest;
+ (id)creationRequestForAssetFromImage:(id)image;
+ (id)creationRequestForAssetFromImageAtFileURL:(NSURL *)url;
+ (id)creationRequestForAssetFromVideoAtFileURL:(NSURL *)url;
+ (void)deleteAssets:(id)assets;
@end

@implementation PHAssetChangeRequest

- (void)dealloc
{
    [_placeholderForCreatedAsset release];
    [_creationDate release];
    [super dealloc];
}

+ (id)creationRequest
{
    PHAssetChangeRequest *request = [[[self alloc] init] autorelease];
    request->_placeholderForCreatedAsset = [[PHObjectPlaceholder alloc] init];
    return request;
}

+ (id)creationRequestForAssetFromImage:(id)image
{
    return [self creationRequest];
}

+ (id)creationRequestForAssetFromImageAtFileURL:(NSURL *)url
{
    return [self creationRequest];
}

+ (id)creationRequestForAssetFromVideoAtFileURL:(NSURL *)url
{
    return [self creationRequest];
}

+ (void)deleteAssets:(id)assets
{
}

@end

@interface PHObjectChangeDetails : NSObject
{
    BOOL _assetContentChanged;
    BOOL _objectWasDeleted;
    PHObject *_objectBeforeChanges;
    PHObject *_objectAfterChanges;
}
@property (readonly, strong) PHObject *objectBeforeChanges;
@property (readonly, strong) PHObject *objectAfterChanges;
@property (readonly) BOOL assetContentChanged;
@property (readonly) BOOL objectWasDeleted;
@end

@implementation PHObjectChangeDetails
@end

@interface PHFetchResultChangeDetails : NSObject
{
    BOOL _hasIncrementalChanges;
    BOOL _hasMoves;
    PHFetchResult *_fetchResultBeforeChanges;
    PHFetchResult *_fetchResultAfterChanges;
    NSIndexSet *_removedIndexes;
    NSArray *_removedObjects;
    NSIndexSet *_insertedIndexes;
    NSArray *_insertedObjects;
    NSIndexSet *_changedIndexes;
    NSArray *_changedObjects;
}
@property (readonly, strong) PHFetchResult *fetchResultBeforeChanges;
@property (readonly, strong) PHFetchResult *fetchResultAfterChanges;
@property (readonly) BOOL hasIncrementalChanges;
@property (readonly, strong) NSIndexSet *removedIndexes;
@property (readonly, strong) NSArray *removedObjects;
@property (readonly, strong) NSIndexSet *insertedIndexes;
@property (readonly, strong) NSArray *insertedObjects;
@property (readonly, strong) NSIndexSet *changedIndexes;
@property (readonly, strong) NSArray *changedObjects;
@property (readonly) BOOL hasMoves;
@end

@implementation PHFetchResultChangeDetails
@end

@interface PHChange : NSObject
- (id)changeDetailsForObject:(PHObject *)object;
- (id)changeDetailsForFetchResult:(PHFetchResult *)fetchResult;
@end

@implementation PHChange

- (id)changeDetailsForObject:(PHObject *)object
{
    return nil;
}

- (id)changeDetailsForFetchResult:(PHFetchResult *)fetchResult
{
    return nil;
}

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

@implementation PHPhotoLibrary

+ (id)sharedPhotoLibrary
{
    static PHPhotoLibrary *library;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        library = [[PHPhotoLibrary alloc] init];
    });
    return library;
}

+ (long)authorizationStatus
{
    return 3;
}

+ (long)authorizationStatusForAccessLevel:(long)accessLevel
{
    return 3;
}

+ (void)requestAuthorization:(void (^)(long status))handler
{
    if (handler)
        runLater([^{ handler(3); } copy]);
}

+ (void)requestAuthorizationForAccessLevel:(long)accessLevel handler:(void (^)(long status))handler
{
    [self requestAuthorization:handler];
}

- (void)performChanges:(void (^)(void))changeBlock completionHandler:(void (^)(BOOL success, NSError *error))completionHandler
{
    void (^changes)(void) = [changeBlock copy];
    void (^completion)(BOOL success, NSError *error) = [completionHandler copy];
    runLater([^{
        @autoreleasepool {
            if (changes)
                changes();
        }
        if (completion)
            completion(NO, [NSError errorWithDomain:@"PHPhotosErrorDomain" code:3300 userInfo:nil]);
        [changes release];
        [completion release];
    } copy]);
}

- (BOOL)performChangesAndWait:(void (^)(void))changeBlock error:(NSError **)error
{
    @autoreleasepool {
        if (changeBlock)
            changeBlock();
    }
    if (error)
        *error = [NSError errorWithDomain:@"PHPhotosErrorDomain" code:3300 userInfo:nil];
    return NO;
}

- (void)registerChangeObserver:(id)observer
{
}

- (void)unregisterChangeObserver:(id)observer
{
}

@end

NSString *const PHPhotosErrorDomain = @"PHPhotosErrorDomain";
NSString *const PHImageResultIsInCloudKey = @"PHImageResultIsInCloudKey";
NSString *const PHImageResultIsDegradedKey = @"PHImageResultIsDegradedKey";
NSString *const PHImageResultRequestIDKey = @"PHImageResultRequestIDKey";
NSString *const PHImageCancelledKey = @"PHImageCancelledKey";
NSString *const PHImageErrorKey = @"PHImageErrorKey";

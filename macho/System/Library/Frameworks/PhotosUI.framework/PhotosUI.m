// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PhotosUI: the photo picker (filter, configuration, result and view controller); it shows an empty view and reports that nothing was picked.

#import <Foundation/Foundation.h>
#include <dispatch/dispatch.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
- (id)autorelease;
@end

@interface NSString (Compare)
- (BOOL)isEqualToString:(NSString *)string;
@end

@class NSItemProvider;

@interface NSItemProvider : NSObject
@end

@interface NSView : NSObject
- (id)initWithFrame:(CGRect)frame;
@end

@interface NSViewController : NSObject
{
    char _padding[16];
}
- (id)initWithNibName:(NSString *)name bundle:(id)bundle;
- (void)setView:(NSView *)view;
- (void)viewDidAppear;
@end

@interface PHPickerFilter : NSObject <NSCopying>
{
    NSString *_kind;
}
+ (id)filterOfKind:(NSString *)kind;
+ (id)imagesFilter;
+ (id)videosFilter;
+ (id)livePhotosFilter;
+ (id)anyFilterMatchingSubfilters:(NSArray *)subfilters;
@end

@implementation PHPickerFilter

+ (id)filterOfKind:(NSString *)kind
{
    PHPickerFilter *filter = [[[self alloc] init] autorelease];
    filter->_kind = [kind copy];
    return filter;
}

+ (id)imagesFilter { return [self filterOfKind:@"images"]; }
+ (id)videosFilter { return [self filterOfKind:@"videos"]; }
+ (id)livePhotosFilter { return [self filterOfKind:@"livePhotos"]; }
+ (id)anyFilterMatchingSubfilters:(NSArray *)subfilters { return [self filterOfKind:@"any"]; }

- (void)dealloc
{
    [_kind release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

- (BOOL)isEqual:(id)object
{
    if ([object isKindOfClass:[PHPickerFilter class]])
        return [_kind isEqualToString:((PHPickerFilter *)object)->_kind] != 0;
    return NO;
}

- (NSUInteger)hash
{
    return [_kind hash];
}

@end

@interface PHPickerConfiguration : NSObject <NSCopying>
{
    NSInteger _preferredAssetRepresentationMode;
    NSInteger _selection;
    NSInteger _selectionLimit;
    PHPickerFilter *_filter;
    NSArray *_preselectedAssetIdentifiers;
}
@property (nonatomic) NSInteger preferredAssetRepresentationMode;
@property (nonatomic) NSInteger selection;
@property (nonatomic) NSInteger selectionLimit;
@property (copy, nonatomic) PHPickerFilter *filter;
@property (copy, nonatomic) NSArray *preselectedAssetIdentifiers;
- (id)initWithPhotoLibrary:(id)library;
@end

@implementation PHPickerConfiguration

- (id)init
{
    self = [super init];
    if (self) {
        _selectionLimit = 1;
        _preselectedAssetIdentifiers = [[NSArray alloc] init];
    }
    return self;
}

- (id)initWithPhotoLibrary:(id)library
{
    return [self init];
}

- (void)dealloc
{
    [_filter release];
    [_preselectedAssetIdentifiers release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
    PHPickerConfiguration *copy = [[[self class] allocWithZone:zone] init];
    [copy setPreferredAssetRepresentationMode:_preferredAssetRepresentationMode];
    [copy setSelection:_selection];
    [copy setSelectionLimit:_selectionLimit];
    [copy setFilter:_filter];
    [copy setPreselectedAssetIdentifiers:_preselectedAssetIdentifiers];
    return copy;
}

@end

@interface PHPickerResult : NSObject
{
    NSItemProvider *_itemProvider;
    NSString *_assetIdentifier;
}
@property (readonly, nonatomic) NSItemProvider *itemProvider;
@property (readonly, nonatomic) NSString *assetIdentifier;
@end

@implementation PHPickerResult

- (id)init
{
    self = [super init];
    if (self)
        _itemProvider = [[NSItemProvider alloc] init];
    return self;
}

- (void)dealloc
{
    [_itemProvider release];
    [_assetIdentifier release];
    [super dealloc];
}

@end

@interface PHPickerViewController : NSViewController
{
    BOOL _finished;
    PHPickerConfiguration *_configuration;
    __weak id _delegate;
}
@property (readonly, copy, nonatomic) PHPickerConfiguration *configuration;
@property (weak, nonatomic) id delegate;
- (id)initWithConfiguration:(PHPickerConfiguration *)configuration;
@end

@implementation PHPickerViewController

- (id)initWithConfiguration:(PHPickerConfiguration *)configuration
{
    self = [super initWithNibName:nil bundle:nil];
    if (self)
        _configuration = [configuration copy];
    return self;
}

- (void)dealloc
{
    [_configuration release];
    [super dealloc];
}

- (void)loadView
{
    [self setView:[[[NSView alloc] initWithFrame:(CGRect){{0, 0}, {480, 320}}] autorelease]];
}

- (void)viewDidAppear
{
    [super viewDidAppear];
    if (!_finished) {
        _finished = YES;
        [self retain];
        dispatch_async(dispatch_get_main_queue(), ^{
            [[self delegate] picker:self didFinishPicking:[NSArray array]];
            [self release];
        });
    }
}

@end

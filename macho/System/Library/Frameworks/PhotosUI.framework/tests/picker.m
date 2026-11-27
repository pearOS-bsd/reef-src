// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// PhotosUI: picker filters compare by kind, a configuration copies all its settings, and a picker view controller keeps a copy of its configuration.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface PHPickerFilter : NSObject <NSCopying>
+ (id)filterOfKind:(NSString *)kind;
+ (id)imagesFilter;
+ (id)videosFilter;
+ (id)livePhotosFilter;
+ (id)anyFilterMatchingSubfilters:(NSArray *)subfilters;
@end
@interface PHPickerConfiguration : NSObject <NSCopying>
@property (nonatomic) NSInteger preferredAssetRepresentationMode, selection, selectionLimit;
@property (copy, nonatomic) PHPickerFilter *filter;
@property (copy, nonatomic) NSArray *preselectedAssetIdentifiers;
- (id)initWithPhotoLibrary:(id)library;
@end
@interface PHPickerResult : NSObject
@property (readonly, nonatomic) id itemProvider;
@property (readonly, nonatomic) NSString *assetIdentifier;
@end
@interface PHPickerViewController : NSObject
@property (readonly, copy, nonatomic) PHPickerConfiguration *configuration;
@property (weak, nonatomic) id delegate;
- (id)initWithConfiguration:(PHPickerConfiguration *)configuration;
@end

int main(void)
{
    PHPickerFilter *images = [PHPickerFilter imagesFilter];
    printf("filters %d %d %d %d\n", [images isEqual:[PHPickerFilter imagesFilter]], [images isEqual:[PHPickerFilter videosFilter]], [[PHPickerFilter livePhotosFilter] isEqual:[PHPickerFilter filterOfKind:@"livePhotos"]],
           [[PHPickerFilter anyFilterMatchingSubfilters:@[images]] isEqual:[PHPickerFilter anyFilterMatchingSubfilters:nil]]);
    printf("hash %d copy %d notfilter %d\n", [images hash] == [[PHPickerFilter imagesFilter] hash], [images copy] == images, [images isEqual:@"images"]);
    PHPickerConfiguration *c = [PHPickerConfiguration new];
    printf("default %ld %ld %ld %d %lu\n", c.preferredAssetRepresentationMode, c.selection, c.selectionLimit, c.filter == nil, (unsigned long)[c.preselectedAssetIdentifiers count]);
    c.preferredAssetRepresentationMode = 2; c.selection = 1; c.selectionLimit = 5; c.filter = images; c.preselectedAssetIdentifiers = @[@"a", @"b"];
    PHPickerConfiguration *copy = [c copy];
    printf("copy %d %ld %ld %ld %d %lu\n", copy != c, copy.preferredAssetRepresentationMode, copy.selection, copy.selectionLimit, [copy.filter isEqual:images], (unsigned long)[copy.preselectedAssetIdentifiers count]);
    printf("library %d\n", [[PHPickerConfiguration alloc] initWithPhotoLibrary:nil].selectionLimit);
    PHPickerResult *result = [PHPickerResult new];
    printf("result %d %d\n", result.itemProvider != nil, result.assetIdentifier == nil);
    PHPickerViewController *vc = [[PHPickerViewController alloc] initWithConfiguration:c];
    printf("controller %d %d %ld\n", vc != nil, vc.configuration != c, vc.configuration.selectionLimit);
    return 0;
}

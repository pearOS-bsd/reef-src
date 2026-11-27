// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>
#import <ImageIO/ImageIO.h>

// CGImageMetadata: tags (XMP properties) addressed by "prefix:name" paths. Tags are kept flat by full path;
// structure and array element paths ("prefix:name[1]", "a/b") are stored under their whole path.
@interface REEFImageMetadataTag : NSObject {
@public
    NSString *ns, *prefix, *name, *path;
    CGImageMetadataType type;
    id value;
}
@end
@implementation REEFImageMetadataTag
@end

@interface REEFImageMetadata : NSObject {
@public
    NSMutableDictionary<NSString *, REEFImageMetadataTag *> *tags;
    NSMutableArray<NSString *> *order;
}
@end
@implementation REEFImageMetadata
- (instancetype)init { if ((self = [super init])) { tags = [NSMutableDictionary dictionary]; order = [NSMutableArray array]; } return self; }
@end

static NSString *NamespaceForPrefix(NSString *p) {
    static NSDictionary *m;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        m = @{@"exif": @"http://ns.adobe.com/exif/1.0/", @"tiff": @"http://ns.adobe.com/tiff/1.0/", @"dc": @"http://purl.org/dc/elements/1.1/",
              @"xmp": @"http://ns.adobe.com/xap/1.0/", @"photoshop": @"http://ns.adobe.com/photoshop/1.0/", @"exifEX": @"http://cipa.jp/exif/1.0/",
              @"aux": @"http://ns.adobe.com/exif/1.0/aux/", @"Iptc4xmpCore": @"http://iptc.org/std/Iptc4xmpCore/1.0/xmlns/",
              @"xmpRights": @"http://ns.adobe.com/xap/1.0/rights/"};
    });
    return m[p];
}

static CGImageMetadataType TypeOf(id v) {
    if ([v isKindOfClass:[NSArray class]]) return kCGImageMetadataTypeArrayOrdered;
    if ([v isKindOfClass:[NSDictionary class]]) return kCGImageMetadataTypeStructure;
    return kCGImageMetadataTypeString;
}

static REEFImageMetadataTag *MakeTag(NSString *ns, NSString *prefix, NSString *name, CGImageMetadataType type, id value) {
    REEFImageMetadataTag *t = [REEFImageMetadataTag new];
    t->ns = [ns copy]; t->prefix = [prefix copy]; t->name = [name copy]; t->type = type; t->value = value;
    t->path = [NSString stringWithFormat:@"%@:%@", prefix, name];
    return t;
}

CGImageMetadataTagRef CGImageMetadataTagCreate(CFStringRef xmlns, CFStringRef prefix, CFStringRef name, CGImageMetadataType type, CFTypeRef value) {
    if (!xmlns || !name || type < kCGImageMetadataTypeDefault || type > kCGImageMetadataTypeStructure) return NULL;
    if (!prefix) return NULL;
    NSString *pfx = (__bridge NSString *)prefix;
    CGImageMetadataType t = type == kCGImageMetadataTypeDefault ? TypeOf((__bridge id)value) : type;
    return (CGImageMetadataTagRef)CFBridgingRetain(MakeTag((__bridge NSString *)xmlns, pfx, (__bridge NSString *)name, t, (__bridge id)value));
}

CGMutableImageMetadataRef CGImageMetadataCreateMutableCopy(CGImageMetadataRef metadata) {
    REEFImageMetadata *n = [REEFImageMetadata new];
    REEFImageMetadata *o = (__bridge REEFImageMetadata *)metadata;
    if (o) { [n->tags setDictionary:o->tags]; [n->order addObjectsFromArray:o->order]; }
    return (CGMutableImageMetadataRef)CFBridgingRetain(n);
}

static void Put(REEFImageMetadata *m, NSString *key, REEFImageMetadataTag *t) {
    if (!m->tags[key]) [m->order addObject:key];
    m->tags[key] = t;
}

static NSString *FullPath(CGImageMetadataTagRef parent, CFStringRef path) {
    NSString *p = (__bridge NSString *)path;
    return p;
}

bool CGImageMetadataSetTagWithPath(CGMutableImageMetadataRef metadata, CGImageMetadataTagRef parent, CFStringRef path, CGImageMetadataTagRef tag) {
    if (!metadata || !path || !tag) return false;
    Put((__bridge REEFImageMetadata *)metadata, FullPath(parent, path), (__bridge REEFImageMetadataTag *)tag);
    return true;
}

bool CGImageMetadataSetValueWithPath(CGMutableImageMetadataRef metadata, CGImageMetadataTagRef parent, CFStringRef path, CFTypeRef value) {
    if (!metadata || !path) return false;
    NSString *p = FullPath(parent, path);
    NSRange c = [p rangeOfString:@":"];
    if (c.location == NSNotFound) return false;
    NSString *prefix = [p substringToIndex:c.location];
    NSString *ns = NamespaceForPrefix(prefix);
    if (!ns) return false;
    NSString *name = [p substringFromIndex:c.location + 1];
    Put((__bridge REEFImageMetadata *)metadata, p, MakeTag(ns, prefix, name, TypeOf((__bridge id)value), (__bridge id)value));
    return true;
}

bool CGImageMetadataRemoveTagWithPath(CGMutableImageMetadataRef metadata, CGImageMetadataTagRef parent, CFStringRef path) {
    REEFImageMetadata *m = (__bridge REEFImageMetadata *)metadata;
    NSString *p = FullPath(parent, path);
    if (!m || !m->tags[p]) return false;
    [m->tags removeObjectForKey:p];
    [m->order removeObject:p];
    return true;
}

void CGImageMetadataEnumerateTagsUsingBlock(CGImageMetadataRef metadata, CFStringRef rootPath, CFDictionaryRef options, bool (^block)(CFStringRef path, CGImageMetadataTagRef tag)) {
    REEFImageMetadata *m = (__bridge REEFImageMetadata *)metadata;
    NSString *root = (__bridge NSString *)rootPath;
    for (NSString *k in [m->order copy]) {
        if (root && ![k hasPrefix:root]) continue;
        if (!block((__bridge CFStringRef)k, (__bridge CGImageMetadataTagRef)m->tags[k])) break;
    }
}

// Metadata of an image: the TIFF and Exif property dictionaries become tiff: and exif: tags.
CGImageMetadataRef CGImageSourceCopyMetadataAtIndex(CGImageSourceRef isrc, size_t index, CFDictionaryRef options) {
    NSDictionary *props = CFBridgingRelease(CGImageSourceCopyPropertiesAtIndex(isrc, index, NULL));
    if (!props) return NULL;
    REEFImageMetadata *m = [REEFImageMetadata new];
    NSDictionary *groups = @{(__bridge NSString *)kCGImagePropertyTIFFDictionary: @"tiff", (__bridge NSString *)kCGImagePropertyExifDictionary: @"exif"};
    for (NSString *g in groups) {
        NSDictionary *d = props[g];
        NSString *prefix = groups[g];
        for (NSString *k in [d.allKeys sortedArrayUsingSelector:@selector(compare:)]) {
            id v = d[k];
            Put(m, [NSString stringWithFormat:@"%@:%@", prefix, k], MakeTag(NamespaceForPrefix(prefix), prefix, k, TypeOf(v), v));
        }
    }
    if (!m->order.count) return NULL;
    return (CGImageMetadataRef)CFBridgingRetain(m);
}

// Adds the image with the exif and tiff tags of the metadata as its properties.
void CGImageDestinationAddImageAndMetadata(CGImageDestinationRef idst, CGImageRef image, CGImageMetadataRef metadata, CFDictionaryRef options) {
    NSMutableDictionary *props = [(__bridge NSDictionary *)options mutableCopy] ?: [NSMutableDictionary dictionary];
    REEFImageMetadata *m = (__bridge REEFImageMetadata *)metadata;
    NSMutableDictionary *tiff = [NSMutableDictionary dictionary], *exif = [NSMutableDictionary dictionary];
    for (NSString *k in m->order) {
        REEFImageMetadataTag *t = m->tags[k];
        if ([t->prefix isEqualToString:@"tiff"]) tiff[t->name] = t->value;
        else if ([t->prefix isEqualToString:@"exif"]) exif[t->name] = t->value;
    }
    if (tiff.count) props[(__bridge NSString *)kCGImagePropertyTIFFDictionary] = tiff;
    if (exif.count) props[(__bridge NSString *)kCGImagePropertyExifDictionary] = exif;
    CGImageDestinationAddImage(idst, image, (__bridge CFDictionaryRef)props);
}

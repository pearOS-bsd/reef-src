// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

// Places and cities of the geographic database: no database here, so the classes only carry what they are given.
@interface GeoKitPlace : NSObject
@end
@implementation GeoKitPlace
@end
@interface GEOCity : NSObject
@end
@implementation GEOCity
@end
// The context of the geo database: none.
id GEOManagedObjectContext(void) { return nil; }

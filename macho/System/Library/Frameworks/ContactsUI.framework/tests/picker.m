// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// ContactsUI: the picker keeps a copy of its displayed keys, a weak delegate, and tells the delegate when its popover closes.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface CNContactPicker : NSObject
@property (copy) NSArray *displayedKeys;
@property (weak) id delegate;
- (void)close;
- (void)popoverWillClose:(id)notification;
- (void)popoverDidClose:(id)notification;
@end

@interface Delegate : NSObject
@end
@implementation Delegate
- (void)contactPickerWillClose:(id)picker { printf("will close %d\n", [picker isKindOfClass:[CNContactPicker class]]); }
- (void)contactPickerDidClose:(id)picker { printf("did close\n"); }
@end

@interface Silent : NSObject
@end
@implementation Silent
@end

int main(void)
{
    CNContactPicker *picker = [CNContactPicker new];
    printf("keys %d\n", picker.displayedKeys == nil);
    NSArray *keys = [NSArray arrayWithObjects:(id[]){@"a", @"b"} count:2];
    picker.displayedKeys = keys;
    printf("keys %lu\n", (unsigned long)[picker.displayedKeys count]);
    [picker close];
    [picker popoverWillClose:nil];
    @autoreleasepool {
        Delegate *delegate = [Delegate new];
        picker.delegate = delegate;
        printf("delegate %d\n", picker.delegate == delegate);
        [picker popoverWillClose:nil];
        [picker popoverDidClose:nil];
    }
    printf("delegate gone %d\n", picker.delegate == nil);
    Silent *silent = [Silent new];
    picker.delegate = silent;
    [picker popoverWillClose:nil];
    [picker popoverDidClose:nil];
    printf("done\n");
    return 0;
}

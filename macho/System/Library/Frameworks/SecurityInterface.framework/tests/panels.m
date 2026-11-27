// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// SecurityInterface: the panels are shared objects that keep their settings, never run, and finish a sheet by calling the delegate on the main queue (which this test does not drain).
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <unistd.h>


@interface NSString (T)
- (const char *)UTF8String;
@end

@interface SFCertificatePanel : NSObject
+ (id)sharedCertificatePanel;
- (long)runModalForTrust:(void *)trust showGroup:(BOOL)showGroup;
- (long)runModalForCertificates:(NSArray *)certificates showGroup:(BOOL)showGroup;
- (void)beginSheetForWindow:(id)window modalDelegate:(id)delegate didEndSelector:(SEL)didEndSelector contextInfo:(void *)contextInfo trust:(void *)trust showGroup:(BOOL)showGroup;
- (void)beginSheetForWindow:(id)window modalDelegate:(id)delegate didEndSelector:(SEL)didEndSelector contextInfo:(void *)contextInfo certificates:(NSArray *)certificates showGroup:(BOOL)showGroup;
- (void)setPolicies:(id)policies;
- (id)policies;
- (void)setDefaultButtonTitle:(NSString *)title;
- (void)setAlternateButtonTitle:(NSString *)title;
- (void)setShowsHelp:(BOOL)showsHelp;
- (BOOL)showsHelp;
- (void)setHelpAnchor:(NSString *)anchor;
- (NSString *)helpAnchor;
@end
@interface SFCertificateTrustPanel : SFCertificatePanel
+ (id)sharedCertificateTrustPanel;
- (long)runModalForTrust:(void *)trust message:(NSString *)message;
- (void)beginSheetForWindow:(id)window modalDelegate:(id)delegate didEndSelector:(SEL)didEndSelector contextInfo:(void *)contextInfo trust:(void *)trust message:(NSString *)message;
- (void)setInformativeText:(NSString *)text;
- (NSString *)informativeText;
@end
@interface SFChooseIdentityPanel : NSObject
+ (id)sharedChooseIdentityPanel;
- (long)runModalForIdentities:(NSArray *)identities message:(NSString *)message;
- (void)beginSheetForWindow:(id)window modalDelegate:(id)delegate didEndSelector:(SEL)didEndSelector contextInfo:(void *)contextInfo identities:(NSArray *)identities message:(NSString *)message;
- (void *)identity;
- (void)setPolicies:(id)policies;
- (id)policies;
- (void)setDefaultButtonTitle:(NSString *)title;
- (void)setAlternateButtonTitle:(NSString *)title;
- (void)setShowsHelp:(BOOL)showsHelp;
- (BOOL)showsHelp;
- (void)setHelpAnchor:(NSString *)anchor;
- (NSString *)helpAnchor;
- (void)setInformativeText:(NSString *)text;
- (NSString *)informativeText;
- (void)setDomain:(NSString *)domain;
- (NSString *)domain;
@end

@interface Delegate : NSObject
@end
@implementation Delegate
- (void)sheetDidEnd:(id)panel returnCode:(long)code contextInfo:(void *)context { printf("sheet ended %ld %d\n", code, panel != nil); }
@end

int main(void)
{
    // The shared panels would run NSPanel's initializer, which needs the host's AppKit library: the panels are allocated only.
    SFCertificatePanel *p = [SFCertificatePanel alloc];
    printf("policies %lu\n", (unsigned long)[[p policies] count]);
    [p setPolicies:@"policy"];
    printf("one %lu\n", (unsigned long)[[p policies] count]);
    [p setPolicies:@[@"a", @"b"]];
    printf("two %lu\n", (unsigned long)[[p policies] count]);
    [p setDefaultButtonTitle:@"OK"]; [p setAlternateButtonTitle:@"Cancel"]; [p setHelpAnchor:@"anchor"]; [p setShowsHelp:YES];
    printf("settings %d %s\n", [p showsHelp], [[p helpAnchor] UTF8String]);
    printf("run %ld %ld\n", [p runModalForTrust:NULL showGroup:NO], [p runModalForCertificates:nil showGroup:YES]);
    SFCertificateTrustPanel *t = [SFCertificateTrustPanel alloc];
    [t setInformativeText:@"info"];
    printf("trust %s %ld\n", [[t informativeText] UTF8String], [t runModalForTrust:NULL message:@"m"]);
    SFChooseIdentityPanel *c = [SFChooseIdentityPanel alloc];
    [c setDomain:@"example.com"]; [c setInformativeText:@"pick"]; [c setHelpAnchor:@"h"]; [c setShowsHelp:YES]; [c setPolicies:@[@"x"]];
    printf("identity %s %s %s %d %lu %ld %d\n", [[c domain] UTF8String], [[c informativeText] UTF8String], [[c helpAnchor] UTF8String], [c showsHelp], (unsigned long)[[c policies] count],
           [c runModalForIdentities:nil message:nil], [c identity] == NULL);
    Delegate *d = [Delegate new];
    [p beginSheetForWindow:nil modalDelegate:d didEndSelector:@selector(sheetDidEnd:returnCode:contextInfo:) contextInfo:NULL trust:NULL showGroup:NO];
    [t beginSheetForWindow:nil modalDelegate:d didEndSelector:@selector(sheetDidEnd:returnCode:contextInfo:) contextInfo:NULL trust:NULL message:nil];
    [c beginSheetForWindow:nil modalDelegate:d didEndSelector:@selector(sheetDidEnd:returnCode:contextInfo:) contextInfo:NULL identities:nil message:nil];
    [p beginSheetForWindow:nil modalDelegate:nil didEndSelector:NULL contextInfo:NULL certificates:nil showGroup:NO];
    usleep(300000);
    printf("done\n");
    return 0;
}

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SecurityInterface: the certificate and identity panels; none of them shows anything and a sheet ends at once (the delegate is told later on the main queue).

#import <Foundation/Foundation.h>

typedef long dispatch_once_t;
typedef void (^dispatch_block_t)(void);
extern void dispatch_once(dispatch_once_t *predicate, dispatch_block_t block);
extern void dispatch_async(void *queue, dispatch_block_t block);
extern struct dispatch_queue_s _dispatch_main_q;
// The SDK's inline fast path in front of the library function.
static inline void _dispatch_once(dispatch_once_t *predicate, dispatch_block_t block)
{
    if (__builtin_expect(*predicate, ~0l) != ~0l)
        dispatch_once(predicate, block);
}
#define dispatch_once _dispatch_once
extern id objc_msgSend(id, SEL, ...);

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
@end

struct __SecTrust;
struct __SecIdentity;

@interface NSPanel : NSObject
{
    char _padding[16];
}
@end

// Ends a sheet: the delegate gets its didEnd selector (panel, return code 0, context) a moment later, on the main queue.
static void endSheetLater(id panel, id delegate, SEL didEndSelector, void *contextInfo)
{
    if (!delegate || !didEndSelector)
        return;
    [panel retain];
    [delegate retain];
    dispatch_async(&_dispatch_main_q, ^{
        ((void (*)(id, SEL, id, long, void *))objc_msgSend)(delegate, didEndSelector, panel, 0, contextInfo);
        [delegate release];
        [panel release];
    });
}

@interface SFCertificatePanel : NSPanel
{
    id _policies;
    NSString *_defaultButtonTitle;
    NSString *_alternateButtonTitle;
    NSString *_helpAnchor;
    BOOL _showsHelp;
}
+ (id)sharedCertificatePanel;
- (long)runModalForTrust:(struct __SecTrust *)trust showGroup:(BOOL)showGroup;
- (long)runModalForCertificates:(NSArray *)certificates showGroup:(BOOL)showGroup;
- (void)beginSheetForWindow:(id)window modalDelegate:(id)delegate didEndSelector:(SEL)didEndSelector contextInfo:(void *)contextInfo trust:(struct __SecTrust *)trust showGroup:(BOOL)showGroup;
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

@implementation SFCertificatePanel

+ (id)sharedCertificatePanel
{
    static SFCertificatePanel *panel;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        panel = [[SFCertificatePanel alloc] init];
    });
    return panel;
}

- (void)dealloc
{
    [_policies release];
    [_defaultButtonTitle release];
    [_alternateButtonTitle release];
    [_helpAnchor release];
    [super dealloc];
}

- (long)runModalForTrust:(struct __SecTrust *)trust showGroup:(BOOL)showGroup { return 0; }
- (long)runModalForCertificates:(NSArray *)certificates showGroup:(BOOL)showGroup { return 0; }

- (void)beginSheetForWindow:(id)window modalDelegate:(id)delegate didEndSelector:(SEL)didEndSelector contextInfo:(void *)contextInfo trust:(struct __SecTrust *)trust showGroup:(BOOL)showGroup
{
    endSheetLater(self, delegate, didEndSelector, contextInfo);
}

- (void)beginSheetForWindow:(id)window modalDelegate:(id)delegate didEndSelector:(SEL)didEndSelector contextInfo:(void *)contextInfo certificates:(NSArray *)certificates showGroup:(BOOL)showGroup
{
    endSheetLater(self, delegate, didEndSelector, contextInfo);
}

- (void)setPolicies:(id)policies
{
    [_policies release];
    _policies = [policies retain];
}

- (id)policies
{
    if (_policies) {
        if ([_policies isKindOfClass:[NSArray class]])
            return _policies;
        return [NSArray arrayWithObject:_policies];
    }
    return [NSArray array];
}

- (void)setDefaultButtonTitle:(NSString *)title
{
    [_defaultButtonTitle release];
    _defaultButtonTitle = [title copy];
}

- (void)setAlternateButtonTitle:(NSString *)title
{
    [_alternateButtonTitle release];
    _alternateButtonTitle = [title copy];
}

- (void)setShowsHelp:(BOOL)showsHelp { _showsHelp = showsHelp; }
- (BOOL)showsHelp { return _showsHelp; }

- (void)setHelpAnchor:(NSString *)anchor
{
    [_helpAnchor release];
    _helpAnchor = [anchor copy];
}

- (NSString *)helpAnchor { return _helpAnchor; }

@end

@interface SFCertificateTrustPanel : SFCertificatePanel
{
    NSString *_informativeText;
}
+ (id)sharedCertificateTrustPanel;
- (long)runModalForTrust:(struct __SecTrust *)trust message:(NSString *)message;
- (void)beginSheetForWindow:(id)window modalDelegate:(id)delegate didEndSelector:(SEL)didEndSelector contextInfo:(void *)contextInfo trust:(struct __SecTrust *)trust message:(NSString *)message;
- (void)setInformativeText:(NSString *)text;
- (NSString *)informativeText;
@end

@implementation SFCertificateTrustPanel

+ (id)sharedCertificateTrustPanel
{
    static SFCertificateTrustPanel *panel;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        panel = [[SFCertificateTrustPanel alloc] init];
    });
    return panel;
}

- (void)dealloc
{
    [_informativeText release];
    [super dealloc];
}

- (long)runModalForTrust:(struct __SecTrust *)trust message:(NSString *)message { return 0; }

- (void)beginSheetForWindow:(id)window modalDelegate:(id)delegate didEndSelector:(SEL)didEndSelector contextInfo:(void *)contextInfo trust:(struct __SecTrust *)trust message:(NSString *)message
{
    endSheetLater(self, delegate, didEndSelector, contextInfo);
}

- (void)setInformativeText:(NSString *)text
{
    [_informativeText release];
    _informativeText = [text copy];
}

- (NSString *)informativeText { return _informativeText; }

@end

@interface SFChooseIdentityPanel : NSPanel
{
    id _policies;
    NSString *_defaultButtonTitle;
    NSString *_alternateButtonTitle;
    NSString *_helpAnchor;
    NSString *_informativeText;
    NSString *_domain;
    BOOL _showsHelp;
}
+ (id)sharedChooseIdentityPanel;
- (long)runModalForIdentities:(NSArray *)identities message:(NSString *)message;
- (void)beginSheetForWindow:(id)window modalDelegate:(id)delegate didEndSelector:(SEL)didEndSelector contextInfo:(void *)contextInfo identities:(NSArray *)identities message:(NSString *)message;
- (struct __SecIdentity *)identity;
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

@implementation SFChooseIdentityPanel

+ (id)sharedChooseIdentityPanel
{
    static SFChooseIdentityPanel *panel;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        panel = [[SFChooseIdentityPanel alloc] init];
    });
    return panel;
}

- (void)dealloc
{
    [_policies release];
    [_defaultButtonTitle release];
    [_alternateButtonTitle release];
    [_helpAnchor release];
    [_informativeText release];
    [_domain release];
    [super dealloc];
}

- (long)runModalForIdentities:(NSArray *)identities message:(NSString *)message { return 0; }

- (void)beginSheetForWindow:(id)window modalDelegate:(id)delegate didEndSelector:(SEL)didEndSelector contextInfo:(void *)contextInfo identities:(NSArray *)identities message:(NSString *)message
{
    endSheetLater(self, delegate, didEndSelector, contextInfo);
}

- (struct __SecIdentity *)identity { return 0; }

- (void)setPolicies:(id)policies
{
    [_policies release];
    _policies = [policies retain];
}

- (id)policies
{
    if (_policies) {
        if ([_policies isKindOfClass:[NSArray class]])
            return _policies;
        return [NSArray arrayWithObject:_policies];
    }
    return [NSArray array];
}

- (void)setDefaultButtonTitle:(NSString *)title
{
    [_defaultButtonTitle release];
    _defaultButtonTitle = [title copy];
}

- (void)setAlternateButtonTitle:(NSString *)title
{
    [_alternateButtonTitle release];
    _alternateButtonTitle = [title copy];
}

- (void)setShowsHelp:(BOOL)showsHelp { _showsHelp = showsHelp; }
- (BOOL)showsHelp { return _showsHelp; }

- (void)setHelpAnchor:(NSString *)anchor
{
    [_helpAnchor release];
    _helpAnchor = [anchor copy];
}

- (NSString *)helpAnchor { return _helpAnchor; }

- (void)setInformativeText:(NSString *)text
{
    [_informativeText release];
    _informativeText = [text copy];
}

- (NSString *)informativeText { return _informativeText; }

- (void)setDomain:(NSString *)domain
{
    [_domain release];
    _domain = [domain copy];
}

- (NSString *)domain { return _domain; }

@end

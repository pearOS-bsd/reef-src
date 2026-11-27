// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// CoreAudioKit: the classes Mac audio apps import (AUGenericView, AUPannerView, the Audio Unit view controllers and configuration, the Bluetooth
// MIDI, inter-device audio and network browser controllers). The Audio Unit views draw a titled panel (there is no parameter UI to build from
// a unit that Reef does not host yet); the controllers are the AppKit classes with Apple's interface.
#import <Cocoa/Cocoa.h>
#import <CoreAudioKit/CoreAudioKit.h>

@implementation AUGenericView {
    AudioUnit _unit;
    AUGenericViewDisplayFlags _flags;
    BOOL _expert;
}
- (AUGenericView *)initWithAudioUnit:(AudioUnit)au { return [self initWithAudioUnit:au displayFlags:AUViewTitleDisplayFlag | AUViewPropertiesDisplayFlag | AUViewParametersDisplayFlag]; }
- (AUGenericView *)initWithAudioUnit:(AudioUnit)au displayFlags:(AUGenericViewDisplayFlags)flags
{
    self = [super initWithFrame:NSMakeRect(0, 0, 400, 200)];
    if (self) { _unit = au; _flags = flags; }
    return self;
}
- (AudioUnit)audioUnit { return _unit; }
- (BOOL)showsExpertParameters { return _expert; }
- (void)setShowsExpertParameters:(BOOL)v { _expert = v; [self setNeedsDisplay:YES]; }
- (BOOL)isFlipped { return YES; }
- (void)drawRect:(NSRect)dirty
{
    [[NSColor windowBackgroundColor] setFill];
    NSRectFill(self.bounds);
    if (_flags & AUViewTitleDisplayFlag) {
        NSDictionary *a = @{ NSFontAttributeName: [NSFont boldSystemFontOfSize:13], NSForegroundColorAttributeName: [NSColor labelColor] };
        [@"Audio Unit" drawAtPoint:NSMakePoint(12, 10) withAttributes:a];
    }
}
- (void)setAudioUnitPersistentData:(NSDictionary *)d {}
- (NSDictionary *)audioUnitPersistentData { return @{}; }
@end

@implementation AUPannerView {
    AudioUnit _unit;
}
+ (AUPannerView *)AUPannerViewWithAudioUnit:(AudioUnit)au
{
    AUPannerView *v = [[AUPannerView alloc] initWithFrame:NSMakeRect(0, 0, 200, 200)];
    v->_unit = au;
    return v;
}
- (AudioUnit)audioUnit { return _unit; }
- (BOOL)isFlipped { return YES; }
- (void)drawRect:(NSRect)dirty
{
    [[NSColor windowBackgroundColor] setFill];
    NSRectFill(self.bounds);
    [[NSColor tertiaryLabelColor] setStroke];
    NSBezierPath *p = [NSBezierPath bezierPathWithOvalInRect:NSInsetRect(self.bounds, 8, 8)];
    [p stroke];
}
@end

@implementation AUAudioUnitViewConfiguration
@synthesize width = _width, height = _height, hostHasController = _hostHasController;
- (instancetype)initWithWidth:(CGFloat)w height:(CGFloat)h hostHasController:(BOOL)c
{
    if ((self = [super init])) { _width = w; _height = h; _hostHasController = c; }
    return self;
}
+ (BOOL)supportsSecureCoding { return YES; }
- (instancetype)initWithCoder:(NSCoder *)c
{
    return [self initWithWidth:[c decodeDoubleForKey:@"width"] height:[c decodeDoubleForKey:@"height"] hostHasController:[c decodeBoolForKey:@"hostHasController"]];
}
- (void)encodeWithCoder:(NSCoder *)c
{
    [c encodeDouble:_width forKey:@"width"]; [c encodeDouble:_height forKey:@"height"]; [c encodeBool:_hostHasController forKey:@"hostHasController"];
}
- (BOOL)isEqual:(id)o
{
    if (![o isKindOfClass:AUAudioUnitViewConfiguration.class]) return NO;
    AUAudioUnitViewConfiguration *c = o;
    return c.width == _width && c.height == _height && c.hostHasController == _hostHasController;
}
- (NSUInteger)hash { return (NSUInteger)(_width * 31 + _height) ^ (_hostHasController ? 1 : 0); }
@end

@implementation AUViewController
- (void)beginRequestWithExtensionContext:(NSExtensionContext *)context {}
@end

@implementation AUGenericViewController
@synthesize auAudioUnit = _auAudioUnit;
@end

@implementation CABTLEMIDIWindowController
@end

@implementation CAInterDeviceAudioViewController
@end

@implementation CANetworkBrowserWindowController
+ (BOOL)isAVBSupported { return NO; }
- (instancetype)init { return [super initWithWindow:nil]; }
@end

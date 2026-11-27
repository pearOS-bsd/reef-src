# CoreAudioKit

Classes and symbols the Mac apps import (from `reef --list-missing`): `AUGenericView` (Amadeus Pro, Audio Hijack, GarageBand, Ecamm, QLab) and
`AUAudioUnitViewConfiguration` (GarageBand). The interfaces are those of the public headers of macOS 26 (observed with the CLT SDK on the Mac VM):
AUGenericView, AUPannerView, AUViewController, AUGenericViewController, AUAudioUnitViewConfiguration (secure coding), CABTLEMIDIWindowController,
CAInterDeviceAudioViewController, CANetworkBrowserWindowController (`+isAVBSupported` is NO: no AVB hardware).
Not done (hackathon): the generic view draws a titled panel, it builds no parameter controls from the Audio Unit.

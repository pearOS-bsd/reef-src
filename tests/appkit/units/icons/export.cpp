// The functions of our unit by the indices of the table of the host (for the driver).
#include "images.h"
using namespace cutefish::reef;
extern "C" __attribute__((visibility("default"))) void* oursFunction(int index)
{
    switch (index) {
    case 63: return (void*)imageCreate;
    case 64: return (void*)imageDestroy;
    case 65: return (void*)gcDrawImage;
    case 81: return (void*)pixelsDecode;
    case 82: return (void*)iconPixels;
    case 83: return (void*)pixelsFree;
    case 120: return (void*)pixelsEncodePng;
    case 210: return (void*)symbolPixels;
    case 211: return (void*)symbolExists;
    case 212: return (void*)symbolBounds;
    case 237: return (void*)dmaBufCreate;
    case 238: return (void*)dmaBufDestroy;
    case 239: return (void*)gcDrawDmaBuf;
    case 257: return (void*)imageUpdate;
    case 301: return (void*)iconPixelsScaled;
    case 308: return (void*)dmaBufSetFence;
    }
    return nullptr;
}

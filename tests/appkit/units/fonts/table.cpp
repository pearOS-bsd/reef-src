// The table of the fonts unit alone, for the test driver.
#include "fonts.h"
using namespace cutefish::reef;
struct Table { uint64_t version; void* functions[317]; };
extern "C" __attribute__((visibility("default"))) void* reef_appkit_host_interface(int version)
{
    static Table table;
    table.version = 0x18;
#define F(i, f) table.functions[i] = reinterpret_cast<void*>(&f)
    F(121, faceMatch); F(122, faceList); F(123, faceFromFile); F(124, faceFromData); F(125, faceRetain); F(126, faceRelease);
    F(127, faceMetrics); F(128, faceName); F(129, faceGlyphs); F(130, faceAdvances); F(131, faceBounds); F(132, faceOutline);
    F(133, faceGlyphName); F(134, faceGlyphNamed); F(135, faceTable); F(136, faceTables); F(137, faceCharacterRanges);
    F(138, faceLanguages); F(139, faceFallback); F(140, faceShape); F(141, faceAxes); F(142, faceCoordinates);
    F(143, faceWithVariations); F(144, fontRegister); F(145, fontUnregister);
    return version == 0x18 ? &table : nullptr;
}
extern "C" __attribute__((visibility("default"))) void* fonts_extra(int index)
{
    void* const extra[] = {
        (void*)&madeObliqueSlant, (void*)&faceStretch, (void*)&madeBoldStrength, (void*)&faceOutlineFormat, (void*)&faceCharacters,
        (void*)&parseFeatures, (void*)&faceSubset, (void*)&drawGlyphs,
    };
    return extra[index];
}

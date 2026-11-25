// Test only: the functions of our objects behind the same table as the published library's, at the same indices.
#include "desktop.h"
#include "host.h"

namespace cutefish::reef {

int hostInitialize(int argc, const char* const* argv, const char* identifier, const reef_ui_callbacks* given);
int hostLoopDescriptor();
void hostLoopDispatch();
void setSoundSilent(bool) {}
void setGlHeadless(bool) {}
void logDrawnFrames() {}

} // namespace cutefish::reef

using namespace cutefish::reef;

struct Table
{
    uint64_t version;
    void* functions[317];
};
static Table table;

extern "C" __attribute__((visibility("default"))) const void* reef_appkit_host_interface(unsigned)
{
    table.version = 0x18;
    table.functions[0] = (void*)hostInitialize;
    table.functions[1] = (void*)hostLoopDescriptor;
    table.functions[2] = (void*)hostLoopDispatch;
    table.functions[84] = (void*)hostMimeTypeForFile;
    table.functions[85] = (void*)hostMimeIconName;
    table.functions[86] = (void*)hostDefaultApplication;
    table.functions[87] = (void*)hostLaunchApplication;
    table.functions[201] = (void*)hostShowInFileManager;
    table.functions[213] = (void*)hostInstanceClaim;
    table.functions[215] = (void*)hostInstanceSetShownInDock;
    table.functions[234] = (void*)hostSetActivationToken;
    table.functions[305] = (void*)hostNowPlayingUpdate;
    return &table;
}

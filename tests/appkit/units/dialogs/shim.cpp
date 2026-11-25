// Test only: our objects' functions at the indices of the host table, and what they take from other units.
#include "dialogs.h"
#include "drag.h"

#include <cstring>

namespace cutefish::reef {

ui::Application* application = nullptr;
reef_ui_callbacks callbacks;
bool headless = true;

int copyString(const std::string& text, char* buffer, int size)
{
    if (buffer && size > 0) {
        const size_t length = std::min(size_t(size - 1), text.size());
        memcpy(buffer, text.data(), length);
        buffer[length] = 0;
    }
    return int(text.size());
}

} // namespace cutefish::reef

using namespace cutefish::reef;

extern "C" __attribute__((visibility("default"))) bool* mine_headless() { return &headless; }

extern "C" __attribute__((visibility("default"))) void mine_table(void** table)
{
    table[107] = (void*)hostDropOfferTypes;
    table[108] = (void*)hostDropOfferRead;
    table[109] = (void*)hostWindowStartDrag;
    table[110] = (void*)hostTestDragData;
    table[111] = (void*)hostTestDragActions;
    table[112] = (void*)hostWindowTestDrop;
    table[249] = (void*)hostWindowStartWindowDrag;
    table[250] = (void*)hostTestWindowDragEnd;
    table[99] = (void*)hostFileChooserOpen;
    table[100] = (void*)hostFileChooserDestroy;
    table[101] = (void*)hostTestFileChooserAnswer;
}

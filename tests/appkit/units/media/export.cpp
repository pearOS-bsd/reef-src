// Test only: the functions of the five units in a table, at the indices of the host's table; after it what is not in the table.
#include "camera.h"
#include "displays.h"
#include "gpu_activity.h"
#include "host.h"
#include "jpeg2000.h"
#include "sound.h"

using namespace cutefish::reef;
#define F(x) reinterpret_cast<void*>(x)

extern "C" __attribute__((visibility("default"))) void* const* reef_test_table()
{
    static void* table[330];
    table[154] = F(soundCreate);
    table[155] = F(soundDestroy);
    table[156] = F(soundPlay);
    table[157] = F(soundStop);
    table[158] = F(soundPause);
    table[159] = F(soundResume);
    table[160] = F(soundDuration);
    table[161] = F(soundPosition);
    table[162] = F(soundSetPosition);
    table[163] = F(soundSetVolume);
    table[164] = F(soundIsPlaying);
    table[165] = F(soundPlayNamed);
    table[216] = F(audioOutputCreate);
    table[217] = F(audioOutputWrite);
    table[218] = F(audioOutputLatency);
    table[219] = F(audioOutputFlush);
    table[220] = F(audioOutputDestroy);
    table[279] = F(audioInputCreate);
    table[280] = F(audioInputRead);
    table[281] = F(audioInputLatency);
    table[282] = F(audioInputDestroy);
    table[284] = F(hostDisplayWaitVblank);
    table[285] = F(hostDisplayPeriod);
    table[291] = F(hostCameraList);
    table[292] = F(hostCameraOpen);
    table[293] = F(hostCameraRead);
    table[294] = F(hostCameraClose);
    table[312] = F(hostImageEncodeJpeg2000);
    table[314] = F(hostGpuWorkIssued);
    table[315] = F(hostGpuWorkMeasured);
    table[316] = F(hostThreadId);
    table[317] = F(setSoundSilent);
    table[318] = F(logDrawnFrames);
    table[319] = F(isJpeg2000);
    table[320] = F(decodeJpeg2000);
    return table;
}

// Cameras, with made up devices. test_camera LIBRARY
#include "load.h"
#include "stub_log.h"
#include <string>
#include <vector>

struct Camera;
struct Format { uint32_t format; int width, height; double minimum, maximum; };
struct Info { char path[64]; char name[128]; char bus[96]; int count; Format formats[96]; };

int main(int argc, char** argv)
{
    load(argv[1]);
    auto fake = reinterpret_cast<void (*)(int, uint64_t, int, const char* const*, int)>(dlsym(RTLD_DEFAULT, "fake_dev"));
    auto clock = reinterpret_cast<void (*)(int, int64_t, int64_t)>(dlsym(RTLD_DEFAULT, "fake_clock"));
    auto list = fn<int(Info*, int)>(291);
    auto open = fn<Camera*(const char*, uint32_t, int, int, double)>(292);
    auto read = fn<int(Camera*, void*, size_t, size_t*, uint64_t*, int)>(293);
    auto close = fn<void(Camera*)>(294);
    const std::vector<std::vector<const char*>> folders = {
        {"null", "video0", "tty", "video10", "video2", "video1", "videoX", "vide", "video"},
        {"video0"},
        {},
        {"sda", "zero"},
        {"video9", "video8", "video7", "video6", "video5", "video4", "video3", "video2", "video1", "video0", "video11", "video10", "video100", "video20", "video19", "video02", "video99999999999999999999999999",
         "video30", "video31", "video32", "video33", "video34", "video35", "video36", "video37", "video38", "video39", "video40"},
        {"video-a-name-that-is-much-longer-than-what-fits-into-the-sixty-four-bytes-of-the-path", "videoa", "videob"},
    };
    std::vector<Info> infos(40);
    const int seeds = argc > 2 ? atoi(argv[2]) : 60;
    for (int mode : {0, 1, 2, 3}) {
        for (int seed = 0; seed < (mode == 2 ? 2 : seeds); seed++) {
            for (size_t f = 0; f < folders.size(); f++) {
                note("== mode %d seed %d folder %zu", mode, seed, f);
                fake(1, seed, mode, folders[f].data(), int(folders[f].size()));
                memset(infos.data(), 0xaa, infos.size() * sizeof(Info));
                const int counts[] = {40, 40, 3, 1, 0, -1};
                const int count = counts[seed % 6];
                const int n = list(infos.data(), count);
                fake(0, 0, 0, nullptr, 0);
                note("list %d -> %d", count, n);
                for (int i = 0; i < n; i++) {
                    note("path %.64s name %.128s bus %.96s count %d hash %016llx", infos[i].path, infos[i].name, infos[i].bus, infos[i].count,
                         (unsigned long long)hashOf(&infos[i], sizeof(Info)));
                    for (int k = 0; k < infos[i].count; k++)
                        note("  %x %dx%d %.9g %.9g", infos[i].formats[k].format, infos[i].formats[k].width, infos[i].formats[k].height, infos[i].formats[k].minimum,
                             infos[i].formats[k].maximum);
                }
                note("rest %016llx", (unsigned long long)hashOf(infos.data() + n, (infos.size() - n) * sizeof(Info)));
            }
        }
    }
    std::vector<unsigned char> data(10000);
    for (int mode : {0, 1}) {
        for (int seed = 0; seed < seeds * 5; seed++) {
            note("== open mode %d seed %d", mode, seed);
            fake(1, seed + 1000, mode, nullptr, 0);
            clock(1, 1000000000000ll, 777);
            const double rates[] = {30, 0, -1, 29.97, 0.0004, 0.0005, 60, 1e7, 59.9999};
            Camera* c = open("/dev/video0", 0x56595559, 640 + seed % 3, 480, rates[seed % 9]);
            note("open -> %d", c != nullptr);
            for (int i = 0; c && i < 12; i++) {
                std::fill(data.begin(), data.end(), 0x55);
                size_t size = 12345;
                uint64_t time = 54321;
                const size_t capacities[] = {10000, 500, 0, 3000, 10000};
                const int r = read(c, data.data(), capacities[i % 5], &size, &time, i * 10 - 10);
                note("read -> %d size %zu time %llu data %016llx", r, size, (unsigned long long)time, (unsigned long long)hashOf(data.data(), data.size()));
            }
            close(c);
            clock(0, 0, 0);
            fake(0, 0, 0, nullptr, 0);
        }
    }
    fflush(stdout);
    _exit(0);
}

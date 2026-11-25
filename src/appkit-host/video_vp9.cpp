// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// VP9: the uncompressed headers and the reference frames. Profiles 0 and 2, 4:2:0.

#include "video_private.h"

#include <algorithm>
#include <cstring>

namespace cutefish::reef::video {

// The quantizers of the indices.
static const int16_t dc8[256] = {
    4, 8, 8, 9, 10, 11, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 23, 24, 25, 26, 26, 27, 28, 29, 30, 31, 32, 32,
    33, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 53, 53, 54, 55, 56, 57, 57,
    58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 74, 75, 76, 77, 78, 78, 79, 80, 81, 81,
    82, 83, 84, 85, 85, 87, 88, 90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 108, 110, 111, 113, 114, 116, 117, 118,
    120, 121, 123, 125, 127, 129, 131, 134, 136, 138, 140, 142, 144, 146, 148, 150, 152, 154, 156, 158, 161, 164, 166, 169,
    172, 174, 177, 180, 182, 185, 187, 190, 192, 195, 199, 202, 205, 208, 211, 214, 217, 220, 223, 226, 230, 233, 237, 240,
    243, 247, 250, 253, 257, 261, 265, 269, 272, 276, 280, 284, 288, 292, 296, 300, 304, 309, 313, 317, 322, 326, 330, 335,
    340, 344, 349, 354, 359, 364, 369, 374, 379, 384, 389, 395, 400, 406, 411, 417, 423, 429, 435, 441, 447, 454, 461, 467,
    475, 482, 489, 497, 505, 513, 522, 530, 539, 549, 559, 569, 579, 590, 602, 614, 626, 640, 654, 668, 684, 700, 717, 736,
    755, 775, 796, 819, 843, 869, 896, 925, 955, 988, 1022, 1058, 1098, 1139, 1184, 1232, 1282, 1336,
};
static const int16_t dc10[256] = {
    4, 9, 10, 13, 15, 17, 20, 22, 25, 28, 31, 34, 37, 40, 43, 47, 50, 53, 57, 60, 64, 68, 71, 75, 78, 82, 86, 90, 93, 97, 101,
    105, 109, 113, 116, 120, 124, 128, 132, 136, 140, 143, 147, 151, 155, 159, 163, 166, 170, 174, 178, 182, 185, 189, 193,
    197, 200, 204, 208, 212, 215, 219, 223, 226, 230, 233, 237, 241, 244, 248, 251, 255, 259, 262, 266, 269, 273, 276, 280,
    283, 287, 290, 293, 297, 300, 304, 307, 310, 314, 317, 321, 324, 327, 331, 334, 337, 343, 350, 356, 362, 369, 375, 381,
    387, 394, 400, 406, 412, 418, 424, 430, 436, 442, 448, 454, 460, 466, 472, 478, 484, 490, 499, 507, 516, 525, 533, 542,
    550, 559, 567, 576, 584, 592, 601, 609, 617, 625, 634, 644, 655, 666, 676, 687, 698, 708, 718, 729, 739, 749, 759, 770,
    782, 795, 807, 819, 831, 844, 856, 868, 880, 891, 906, 920, 933, 947, 961, 975, 988, 1001, 1015, 1030, 1045, 1061, 1076,
    1090, 1105, 1120, 1137, 1153, 1170, 1186, 1202, 1218, 1236, 1253, 1271, 1288, 1306, 1323, 1342, 1361, 1379, 1398, 1416,
    1436, 1456, 1476, 1496, 1516, 1537, 1559, 1580, 1601, 1624, 1647, 1670, 1692, 1717, 1741, 1766, 1791, 1817, 1844, 1871,
    1900, 1929, 1958, 1990, 2021, 2054, 2088, 2123, 2159, 2197, 2236, 2276, 2319, 2363, 2410, 2458, 2508, 2561, 2616, 2675,
    2737, 2802, 2871, 2944, 3020, 3102, 3188, 3280, 3375, 3478, 3586, 3702, 3823, 3953, 4089, 4236, 4394, 4559, 4737, 4929,
    5130, 5347,
};
static const int16_t ac8[256] = {
    4, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
    38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
    68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97,
    98, 99, 100, 101, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 142,
    144, 146, 148, 150, 152, 155, 158, 161, 164, 167, 170, 173, 176, 179, 182, 185, 188, 191, 194, 197, 200, 203, 207, 211,
    215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255, 260, 265, 270, 275, 280, 285, 290, 295, 300, 305, 311, 317, 323,
    329, 335, 341, 347, 353, 359, 366, 373, 380, 387, 394, 401, 408, 416, 424, 432, 440, 448, 456, 465, 474, 483, 492, 501,
    510, 520, 530, 540, 550, 560, 571, 582, 593, 604, 615, 627, 639, 651, 663, 676, 689, 702, 715, 729, 743, 757, 771, 786,
    801, 816, 832, 848, 864, 881, 898, 915, 933, 951, 969, 988, 1007, 1026, 1046, 1066, 1087, 1108, 1129, 1151, 1173, 1196,
    1219, 1243, 1267, 1292, 1317, 1343, 1369, 1396, 1423, 1451, 1479, 1508, 1537, 1567, 1597, 1628, 1660, 1692, 1725, 1759,
    1793, 1828,
};
static const int16_t ac10[256] = {
    4, 9, 11, 13, 16, 18, 21, 24, 27, 30, 33, 37, 40, 44, 48, 51, 55, 59, 63, 67, 71, 75, 79, 83, 88, 92, 96, 100, 105, 109,
    114, 118, 122, 127, 131, 136, 140, 145, 149, 154, 158, 163, 168, 172, 177, 181, 186, 190, 195, 199, 204, 208, 213, 217,
    222, 226, 231, 235, 240, 244, 249, 253, 258, 262, 267, 271, 275, 280, 284, 289, 293, 297, 302, 306, 311, 315, 319, 324,
    328, 332, 337, 341, 345, 349, 354, 358, 362, 367, 371, 375, 379, 384, 388, 392, 396, 401, 409, 417, 425, 433, 441, 449,
    458, 466, 474, 482, 490, 498, 506, 514, 523, 531, 539, 547, 555, 563, 571, 579, 588, 596, 604, 616, 628, 640, 652, 664,
    676, 688, 700, 713, 725, 737, 749, 761, 773, 785, 797, 809, 825, 841, 857, 873, 889, 905, 922, 938, 954, 970, 986, 1002,
    1018, 1038, 1058, 1078, 1098, 1118, 1138, 1158, 1178, 1198, 1218, 1242, 1266, 1290, 1314, 1338, 1362, 1386, 1411, 1435,
    1463, 1491, 1519, 1547, 1575, 1603, 1631, 1663, 1695, 1727, 1759, 1791, 1823, 1859, 1895, 1931, 1967, 2003, 2039, 2079,
    2119, 2159, 2199, 2239, 2283, 2327, 2371, 2415, 2459, 2507, 2555, 2603, 2651, 2703, 2755, 2807, 2859, 2915, 2971, 3027,
    3083, 3143, 3203, 3263, 3327, 3391, 3455, 3523, 3591, 3659, 3731, 3803, 3876, 3952, 4028, 4104, 4184, 4264, 4348, 4432,
    4516, 4604, 4692, 4784, 4876, 4972, 5068, 5168, 5268, 5372, 5476, 5584, 5692, 5804, 5916, 6032, 6148, 6268, 6388, 6512,
    6640, 6768, 6900, 7036, 7172, 7312,
};

static const int literalToFilter[4] = {1, 0, 2, 3};
static const int featureBits[4] = {8, 6, 2, 0};
static const bool featureSigned[4] = {true, true, false, false};

namespace {

enum Feature {
    QuantizerFeature = 0,
    FilterFeature = 1,
    ReferenceFeature = 2,
    SkipFeature = 3,
};

struct Header
{
    int profile = 0;
    bool showExisting = false;
    bool keyFrame = false;
    bool showFrame = false;
    bool errorResilient = false;
    bool intraOnly = false;
    int resetContext = 0;
    int bitDepth = 8;
    bool subsamplingX = true;
    bool subsamplingY = true;
    int refreshFlags = 0;
    int references[3] = {};
    bool signBias[3] = {};
    int width = 0;
    int height = 0;
    bool highPrecision = false;
    int filter = 0;
    bool refreshContext = false;
    bool parallel = false;
    int contextIndex = 0;
    int filterLevel = 0;
    int sharpness = 0;
    int baseQ = 0;
    int deltaYDc = 0;
    int deltaUvDc = 0;
    int deltaUvAc = 0;
    bool lossless = false;
    bool segmentation = false;
    bool updateMap = false;
    bool temporalUpdate = false;
    uint8_t treeProbabilities[7] = {255, 255, 255, 255, 255, 255, 255};
    uint8_t predictionProbabilities[3] = {255, 255, 255};
    int log2TileColumns = 0;
    int log2TileRows = 0;
    size_t headerSize = 0;
    int compressedSize = 0;

    bool isIntra() const { return keyFrame || intraOnly; }
};

struct Reference
{
    std::shared_ptr<Surface> surface;
    int width = 0;
    int height = 0;
};

// What lasts until a frame changes it.
struct State
{
    int bitDepth = 8;
    bool subsamplingX = true;
    bool subsamplingY = true;
    bool deltaEnabled = false;
    int refDeltas[4] = {1, 0, -1, -1};
    int modeDeltas[2] = {};
    bool absolute = false;
    bool enabled[8][4] = {};
    int data[8][4] = {};
};

class Vp9Decoder : public Decoder
{
public:
    bool configure(const uint8_t* configuration, size_t size, int width, int height);
    bool decode(const uint8_t* data, size_t size, std::shared_ptr<Surface>& surface) override;

private:
    bool parseHeader(const uint8_t* data, size_t size, Header& header);
    void fillSegments(const Header& header, VASliceParameterBufferVP9& parameters) const;

    std::shared_ptr<VaDisplay> m_display;
    std::unique_ptr<Context> m_context;
    VAProfile m_vaProfile = VAProfileNone;
    int m_profile = 0;
    Reference m_references[8];
    State m_state;
};

} // namespace

static void readColorConfig(BitReader& reader, Header& header)
{
    header.bitDepth = 8;
    if (header.profile >= 2)
        header.bitDepth = reader.bit() ? 12 : 10;
    const int space = reader.bits(3);
    // The range.
    if (space != 7) {
        reader.bit();
        if (header.profile == 1 || header.profile == 3) {
            header.subsamplingX = reader.bit();
            header.subsamplingY = reader.bit();
            reader.bit();
        } else {
            header.subsamplingX = true;
            header.subsamplingY = true;
        }
    } else if (header.profile == 1 || header.profile == 3) {
        // RGB
        header.subsamplingX = false;
        header.subsamplingY = false;
        reader.bit();
    }
}

static bool readSyncCode(BitReader& reader)
{
    return reader.bits(24) == 0x498342;
}

static void readFrameSize(BitReader& reader, Header& header)
{
    header.width = reader.bits(16) + 1;
    header.height = reader.bits(16) + 1;
}

// The size to be shown.
static void skipRenderSize(BitReader& reader)
{
    if (reader.bit())
        reader.skip(32);
}

static int readSigned(BitReader& reader, int count)
{
    const int value = reader.bits(count);
    return reader.bit() ? -value : value;
}

static int readDelta(BitReader& reader)
{
    return reader.bit() ? readSigned(reader, 4) : 0;
}

static uint8_t readProbability(BitReader& reader)
{
    return reader.bit() ? reader.bits(8) : 255;
}

bool Vp9Decoder::parseHeader(const uint8_t* data, size_t size, Header& header)
{
    BitReader reader(data, size);
    if (reader.bits(2) != 2)
        return false;
    header.profile = reader.bits(1);
    header.profile |= reader.bits(1) << 1;
    if (header.profile == 3)
        reader.bit();
    header.showExisting = reader.bit();
    if (header.showExisting)
        return !reader.failed();
    header.keyFrame = !reader.bit();
    header.showFrame = reader.bit();
    header.errorResilient = reader.bit();
    header.bitDepth = m_state.bitDepth;
    header.subsamplingX = m_state.subsamplingX;
    header.subsamplingY = m_state.subsamplingY;
    if (header.keyFrame) {
        if (!readSyncCode(reader))
            return false;
        readColorConfig(reader, header);
        readFrameSize(reader, header);
        skipRenderSize(reader);
        header.refreshFlags = 0xff;
    } else {
        if (!header.showFrame)
            header.intraOnly = reader.bit();
        if (!header.errorResilient)
            header.resetContext = reader.bits(2);
        if (header.intraOnly) {
            if (!readSyncCode(reader))
                return false;
            if (header.profile > 0) {
                readColorConfig(reader, header);
            } else {
                header.bitDepth = 8;
                header.subsamplingX = true;
                header.subsamplingY = true;
            }
            header.refreshFlags = reader.bits(8);
            readFrameSize(reader, header);
            skipRenderSize(reader);
        } else {
            header.refreshFlags = reader.bits(8);
            for (int i = 0; i < 3; i++) {
                header.references[i] = reader.bits(3);
                header.signBias[i] = reader.bit();
            }
            bool found = false;
            for (int i = 0; i < 3; i++) {
                if (reader.bit()) {
                    const Reference& reference = m_references[header.references[i]];
                    header.width = reference.width;
                    header.height = reference.height;
                    found = true;
                    break;
                }
            }
            if (!found)
                readFrameSize(reader, header);
            skipRenderSize(reader);
            header.highPrecision = reader.bit();
            header.filter = reader.bit() ? 4 : literalToFilter[reader.bits(2)];
        }
    }
    m_state.bitDepth = header.bitDepth;
    m_state.subsamplingX = header.subsamplingX;
    m_state.subsamplingY = header.subsamplingY;
    if (!header.errorResilient) {
        header.refreshContext = reader.bit();
        header.parallel = reader.bit();
    } else {
        header.parallel = true;
    }
    header.contextIndex = reader.bits(2);
    if (header.isIntra() || header.errorResilient) {
        m_state.deltaEnabled = true;
        m_state.refDeltas[0] = 1;
        m_state.refDeltas[1] = 0;
        m_state.refDeltas[2] = -1;
        m_state.refDeltas[3] = -1;
        memset(m_state.modeDeltas, 0, sizeof(m_state.modeDeltas));
        m_state.absolute = false;
        memset(m_state.enabled, 0, sizeof(m_state.enabled));
        memset(m_state.data, 0, sizeof(m_state.data));
    }

    header.filterLevel = reader.bits(6);
    header.sharpness = reader.bits(3);
    m_state.deltaEnabled = reader.bit();
    if (m_state.deltaEnabled && reader.bit()) {
        for (int& delta : m_state.refDeltas) {
            if (reader.bit())
                delta = readSigned(reader, 6);
        }
        for (int& delta : m_state.modeDeltas) {
            if (reader.bit())
                delta = readSigned(reader, 6);
        }
    }

    header.baseQ = reader.bits(8);
    header.deltaYDc = readDelta(reader);
    header.deltaUvDc = readDelta(reader);
    header.deltaUvAc = readDelta(reader);
    header.lossless = header.baseQ == 0 && header.deltaYDc == 0 && header.deltaUvDc == 0 && header.deltaUvAc == 0;

    header.segmentation = reader.bit();
    if (header.segmentation) {
        header.updateMap = reader.bit();
        if (header.updateMap) {
            for (uint8_t& probability : header.treeProbabilities)
                probability = readProbability(reader);
            header.temporalUpdate = reader.bit();
            for (uint8_t& probability : header.predictionProbabilities)
                probability = header.temporalUpdate ? readProbability(reader) : 255;
        }
        if (reader.bit()) {
            m_state.absolute = reader.bit();
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 4; j++) {
                    const bool enabled = reader.bit();
                    int value = 0;
                    if (enabled) {
                        value = reader.bits(featureBits[j]);
                        if (featureSigned[j] && reader.bit())
                            value = -value;
                    }
                    m_state.enabled[i][j] = enabled;
                    m_state.data[i][j] = value;
                }
            }
        }
    }

    // Tiles are between 4 and 64 blocks of 64 pixels wide.
    const int blocks = (((header.width + 7) >> 3) + 7) >> 3;
    int minLog2 = 0;
    while ((64 << minLog2) < blocks)
        minLog2++;
    int maxLog2 = 1;
    while ((blocks >> maxLog2) >= 4)
        maxLog2++;
    maxLog2--;
    header.log2TileColumns = minLog2;
    while (header.log2TileColumns < maxLog2 && reader.bit())
        header.log2TileColumns++;
    header.log2TileRows = reader.bits(1);
    if (header.log2TileRows != 0)
        header.log2TileRows += reader.bits(1);

    header.compressedSize = reader.bits(16);
    while (reader.position() % 8 != 0)
        reader.bits(1);
    header.headerSize = reader.position() / 8;
    return !reader.failed() && header.width > 0 && header.height > 0 && header.compressedSize > 0
        && header.headerSize + header.compressedSize <= size;
}

bool Vp9Decoder::configure(const uint8_t* configuration, size_t size, int width, int height)
{
    if (width < 1 || height < 1 || size < 12)
        return false;
    m_profile = configuration[4];
    const int bitDepth = configuration[6] >> 4;
    const int chroma = configuration[6] >> 1 & 7;
    if (chroma > 1)
        return false;
    VAProfile profile;
    if (m_profile == 0 && bitDepth == 8)
        profile = VAProfileVP9Profile0;
    else if (m_profile == 2 && bitDepth == 10)
        profile = VAProfileVP9Profile2;
    else
        return false;
    m_display = VaDisplay::open();
    if (!m_display || !m_display->supports(profile))
        return false;
    codedWidth = width;
    codedHeight = height;
    depth = bitDepth;
    m_vaProfile = profile;
    m_context = std::make_unique<Context>(m_display, profile, codedWidth, codedHeight, depth);
    return m_context->isValid();
}

void Vp9Decoder::fillSegments(const Header& header, VASliceParameterBufferVP9& parameters) const
{
    const int16_t* dc = depth <= 8 ? dc8 : dc10;
    const int16_t* ac = depth <= 8 ? ac8 : ac10;
    const auto quantizer = [](const int16_t* table, int index) { return table[std::clamp(index, 0, 255)]; };
    const auto filterLevel = [](int level) { return uint8_t(std::clamp(level, 0, 63)); };
    for (int i = 0; i < 8; i++) {
        VASegmentParameterVP9& segment = parameters.seg_param[i];
        const bool* enabled = m_state.enabled[i];
        const int* data = m_state.data[i];
        const auto has = [&](Feature feature) { return header.segmentation && enabled[feature]; };
        segment.segment_flags.fields.segment_reference_enabled = has(ReferenceFeature);
        segment.segment_flags.fields.segment_reference = data[ReferenceFeature];
        segment.segment_flags.fields.segment_reference_skipped = has(SkipFeature);

        int q = header.baseQ;
        if (has(QuantizerFeature)) {
            q = data[QuantizerFeature];
            if (!m_state.absolute)
                q += header.baseQ;
            q = std::clamp(q, 0, 255);
        }
        segment.luma_ac_quant_scale = quantizer(ac, q);
        segment.luma_dc_quant_scale = quantizer(dc, q + header.deltaYDc);
        segment.chroma_ac_quant_scale = quantizer(ac, q + header.deltaUvAc);
        segment.chroma_dc_quant_scale = quantizer(dc, q + header.deltaUvDc);

        int level = header.filterLevel;
        if (has(FilterFeature)) {
            level = data[FilterFeature];
            if (!m_state.absolute)
                level += header.filterLevel;
            level = std::clamp(level, 0, 63);
        }
        if (!m_state.deltaEnabled) {
            memset(segment.filter_level, filterLevel(level), sizeof(segment.filter_level));
            continue;
        }
        const int shift = level >> 5;
        segment.filter_level[0][0] = segment.filter_level[0][1] = filterLevel(level + (m_state.refDeltas[0] << shift));
        for (int reference = 1; reference < 4; reference++) {
            for (int mode = 0; mode < 2; mode++) {
                segment.filter_level[reference][mode] = filterLevel(level + (m_state.refDeltas[reference] << shift)
                                                                    + (m_state.modeDeltas[mode] << shift));
            }
        }
    }
}

bool Vp9Decoder::decode(const uint8_t* data, size_t size, std::shared_ptr<Surface>& surface)
{
    surface.reset();
    if (!data || size == 0)
        return false;
    Header header;
    if (!parseHeader(data, size, header))
        return false;
    if (header.showExisting)
        return true;
    if (header.bitDepth != depth || !header.subsamplingX || !header.subsamplingY)
        return false;
    if (header.width != codedWidth || header.height != codedHeight) {
        if (header.isIntra()) {
            auto context = std::make_unique<Context>(m_display, m_vaProfile, header.width, header.height, depth);
            if (!context->isValid())
                return false;
            m_context = std::move(context);
        } else {
            // The frames before are scaled, the decoder stays.
            m_context->resize(header.width, header.height);
        }
        codedWidth = header.width;
        codedHeight = header.height;
    }
    if (!header.isIntra()) {
        for (int index : header.references) {
            if (!m_references[index].surface)
                return false;
        }
    }

    std::shared_ptr<Surface> target = m_context->takeSurface();
    if (!target)
        return false;
    VADecPictureParameterBufferVP9 picture = {};
    picture.frame_width = header.width;
    picture.frame_height = header.height;
    for (int i = 0; i < 8; i++) {
        const Reference& reference = m_references[i];
        const bool usable = reference.surface
            && (!header.isIntra() || (reference.width == header.width && reference.height == header.height));
        picture.reference_frames[i] = usable ? reference.surface->id() : VA_INVALID_SURFACE;
    }
    picture.pic_fields.bits.subsampling_x = header.subsamplingX;
    picture.pic_fields.bits.subsampling_y = header.subsamplingY;
    picture.pic_fields.bits.frame_type = !header.keyFrame;
    picture.pic_fields.bits.show_frame = header.showFrame;
    picture.pic_fields.bits.error_resilient_mode = header.errorResilient;
    picture.pic_fields.bits.intra_only = header.intraOnly;
    picture.pic_fields.bits.allow_high_precision_mv = header.highPrecision;
    picture.pic_fields.bits.mcomp_filter_type = header.filter;
    picture.pic_fields.bits.frame_parallel_decoding_mode = header.parallel;
    picture.pic_fields.bits.reset_frame_context = header.resetContext;
    picture.pic_fields.bits.refresh_frame_context = header.refreshContext;
    picture.pic_fields.bits.frame_context_idx = header.contextIndex;
    picture.pic_fields.bits.segmentation_enabled = header.segmentation;
    picture.pic_fields.bits.segmentation_temporal_update = header.temporalUpdate;
    picture.pic_fields.bits.segmentation_update_map = header.updateMap;
    picture.pic_fields.bits.last_ref_frame = header.references[0];
    picture.pic_fields.bits.last_ref_frame_sign_bias = header.signBias[0];
    picture.pic_fields.bits.golden_ref_frame = header.references[1];
    picture.pic_fields.bits.golden_ref_frame_sign_bias = header.signBias[1];
    picture.pic_fields.bits.alt_ref_frame = header.references[2];
    picture.pic_fields.bits.alt_ref_frame_sign_bias = header.signBias[2];
    picture.pic_fields.bits.lossless_flag = header.lossless;
    picture.filter_level = header.filterLevel;
    picture.sharpness_level = header.sharpness;
    picture.log2_tile_rows = header.log2TileRows;
    picture.log2_tile_columns = header.log2TileColumns;
    picture.frame_header_length_in_bytes = header.headerSize;
    picture.first_partition_size = header.compressedSize;
    memcpy(picture.mb_segment_tree_probs, header.treeProbabilities, sizeof(picture.mb_segment_tree_probs));
    memcpy(picture.segment_pred_probs, header.predictionProbabilities, sizeof(picture.segment_pred_probs));
    picture.profile = header.profile;
    picture.bit_depth = header.bitDepth;

    VASliceParameterBufferVP9 parameters = {};
    parameters.slice_data_size = size;
    fillSegments(header, parameters);

    if (!m_context->addBuffer(VAPictureParameterBufferType, &picture, sizeof(picture))
        || !m_context->addBuffer(VASliceParameterBufferType, &parameters, sizeof(parameters))
        || !m_context->addBuffer(VASliceDataBufferType, data, size)) {
        m_context->discard();
        return false;
    }
    if (!m_context->render(target->id()))
        return false;

    for (int i = 0; i < 8; i++) {
        if (header.refreshFlags >> i & 1)
            m_references[i] = {target, header.width, header.height};
    }
    surface = std::move(target);
    return true;
}

std::unique_ptr<Decoder> makeVp9Decoder(const uint8_t* configuration, size_t size, int width, int height)
{
    auto decoder = std::make_unique<Vp9Decoder>();
    if (!decoder->configure(configuration, size, width, height))
        return nullptr;
    return decoder;
}

} // namespace cutefish::reef::video

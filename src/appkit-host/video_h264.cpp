// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// H.264: the parameter sets, the slice headers, the reference pictures. Frames only, 8 bits, 4:2:0.

#include "video_private.h"

#include <algorithm>
#include <cstring>
#include <map>

namespace cutefish::reef::video {

namespace {

enum UnitType : uint8_t {
    SliceUnit = 1,
    IdrUnit = 5,
    SpsUnit = 7,
    PpsUnit = 8,
};

enum SliceType {
    P = 0,
    B = 1,
    I = 2,
    SP = 3,
    SI = 4,
};

// In the order of the stream.
struct ScalingLists
{
    uint8_t small[6][16];
    uint8_t large[6][64];
};

struct Sps
{
    int profile = 0;
    int constraints = 0;
    int level = 0;
    int chromaFormat = 1;
    bool separatePlanes = false;
    int lumaDepth = 8;
    int chromaDepth = 8;
    bool hasScalingLists = false;
    ScalingLists scalingLists;
    int log2MaxFrameNumber = 4;
    int pocType = 0;
    int log2MaxPocLsb = 4;
    bool deltaPocAlwaysZero = false;
    int offsetForNonReference = 0;
    int offsetForBottom = 0;
    int pocCycleLength = 0;
    int pocCycle[256] = {};
    int maxReferences = 0;
    bool frameNumberGaps = false;
    int widthMbs = 0;
    int heightMbs = 0;
    bool frameMbsOnly = true;
    bool mbaff = false;
    bool direct8x8 = false;
    int cropLeft = 0;
    int cropRight = 0;
    int cropTop = 0;
    int cropBottom = 0;
};

struct Pps
{
    int sps = 0;
    bool cabac = false;
    bool bottomPoc = false;
    int sliceGroups = 1;
    int referenceCounts[2] = {1, 1};
    bool weightedPrediction = false;
    int weightedBiprediction = 0;
    int qp = 26;
    int qs = 26;
    int chromaQpOffset = 0;
    int secondChromaQpOffset = 0;
    bool deblockingControl = false;
    bool constrainedIntra = false;
    bool redundantPictures = false;
    bool transform8x8 = false;
    ScalingLists scalingLists;
};

struct Modification
{
    uint32_t operation;
    uint32_t value;
};

struct Marking
{
    int operation = 0;
    int difference = 0;
    int longTermNumber = 0;
    int longTermIndex = 0;
    int longTermCount = 0;
};

struct Slice
{
    const uint8_t* data = nullptr;
    size_t size = 0;
    int referenceIdc = 0;
    int unitType = 0;
    size_t headerBits = 0;
    int firstMb = 0;
    int type = 0;
    int pps = 0;
    int frameNumber = 0;
    int idrId = 0;
    int pocLsb = 0;
    int deltaPocBottom = 0;
    int deltaPoc[2] = {};
    bool directSpatial = false;
    int referenceCounts[2] = {};
    bool modified[2] = {};
    std::vector<Modification> modifications[2];
    int lumaLog2Denominator = 0;
    int chromaLog2Denominator = 0;
    bool lumaWeighted[2] = {};
    bool chromaWeighted[2] = {};
    int16_t lumaWeight[2][32] = {};
    int16_t lumaOffset[2][32] = {};
    int16_t chromaWeight[2][32][2] = {};
    int16_t chromaOffset[2][32][2] = {};
    bool noOutputOfPrior = false;
    bool longTermReference = false;
    bool adaptiveMarking = false;
    std::vector<Marking> markings;
    int cabacInit = 0;
    int qpDelta = 0;
    int disableDeblocking = 0;
    int alphaOffset = 0;
    int betaOffset = 0;
};

struct Reference
{
    std::shared_ptr<Surface> surface;
    int frameNumber = 0;
    int frameNumberWrap = 0;
    int longTermIndex = 0;
    bool shortTerm = false;
    bool longTerm = false;
    int top = 0;
    int bottom = 0;
    // For a frame that was left out.
    bool missing = false;

    int poc() const { return std::min(top, bottom); }
};

} // namespace

static const uint8_t defaultIntra4x4[16] = {6, 13, 13, 20, 20, 20, 28, 28, 28, 28, 32, 32, 32, 37, 37, 42};
static const uint8_t defaultInter4x4[16] = {10, 14, 14, 20, 20, 20, 24, 24, 24, 24, 27, 27, 27, 30, 30, 34};
static const uint8_t defaultIntra8x8[64] = {
    6, 10, 10, 13, 11, 13, 16, 16, 16, 16, 18, 18, 18, 18, 18, 23, 23, 23, 23, 23, 23, 25, 25, 25, 25, 25, 25, 25, 27, 27, 27, 27,
    27, 27, 27, 27, 29, 29, 29, 29, 29, 29, 29, 31, 31, 31, 31, 31, 31, 33, 33, 33, 33, 33, 36, 36, 36, 36, 38, 38, 38, 40, 40, 42,
};
static const uint8_t defaultInter8x8[64] = {
    9, 13, 13, 15, 13, 15, 17, 17, 17, 17, 19, 19, 19, 19, 19, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 24, 24, 24, 24,
    24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 30, 30, 30, 30, 32, 32, 32, 33, 33, 35,
};

static const uint8_t zigzag4x4[16] = {0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15};
static const uint8_t zigzag8x8[64] = {
    0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63,
};

static const uint8_t* defaultScalingList(int index)
{
    if (index < 6)
        return index < 3 ? defaultIntra4x4 : defaultInter4x4;
    return index % 2 == 0 ? defaultIntra8x8 : defaultInter8x8;
}

static uint8_t* scalingList(ScalingLists& lists, int index)
{
    return index < 6 ? lists.small[index] : lists.large[index - 6];
}

static const uint8_t* scalingList(const ScalingLists& lists, int index)
{
    return index < 6 ? lists.small[index] : lists.large[index - 6];
}

static int scalingListSize(int index)
{
    return index < 6 ? 16 : 64;
}

static bool readScalingList(BitReader& reader, uint8_t* list, int size, bool& useDefault)
{
    int last = 8;
    int next = 8;
    useDefault = false;
    for (int j = 0; j < size; j++) {
        if (next != 0) {
            const int delta = reader.se();
            if (delta < -128 || delta > 127)
                return false;
            next = (last + delta + 256) % 256;
            useDefault = j == 0 && next == 0;
            if (useDefault)
                return true;
        }
        list[j] = next == 0 ? last : next;
        last = list[j];
    }
    return true;
}

static bool readScalingLists(BitReader& reader, ScalingLists& lists, int count, const ScalingLists* fallback)
{
    for (int i = 0; i < 12; i++) {
        if (i >= count || !reader.bit()) {
            if (i == 0 || i == 3 || i == 6 || i == 7) {
                if (fallback)
                    memcpy(scalingList(lists, i), scalingList(*fallback, i), scalingListSize(i));
                else
                    memcpy(scalingList(lists, i), defaultScalingList(i), scalingListSize(i));
            } else {
                memcpy(scalingList(lists, i), scalingList(lists, i < 6 ? i - 1 : i - 2), scalingListSize(i));
            }
            continue;
        }
        bool useDefault;
        if (!readScalingList(reader, scalingList(lists, i), scalingListSize(i), useDefault))
            return false;
        if (useDefault)
            memcpy(scalingList(lists, i), defaultScalingList(i), scalingListSize(i));
    }
    return true;
}

static bool parseSps(const Rbsp& rbsp, int& id, Sps& sps)
{
    BitReader reader(rbsp);
    reader.skip(8);
    sps.profile = reader.bits(8);
    sps.constraints = reader.bits(8);
    sps.level = reader.bits(8);
    id = reader.ue();
    if (id > 31)
        return false;
    memset(sps.scalingLists.small, 16, sizeof(sps.scalingLists.small));
    memset(sps.scalingLists.large, 16, sizeof(sps.scalingLists.large));
    static const int highProfiles[] = {100, 110, 122, 244, 44, 83, 86, 118, 128, 138, 139, 134, 135};
    if (std::find(std::begin(highProfiles), std::end(highProfiles), sps.profile) != std::end(highProfiles)) {
        sps.chromaFormat = reader.ue();
        if (sps.chromaFormat > 3)
            return false;
        if (sps.chromaFormat == 3)
            sps.separatePlanes = reader.bit();
        sps.lumaDepth = reader.ue() + 8;
        sps.chromaDepth = reader.ue() + 8;
        reader.bit();
        sps.hasScalingLists = reader.bit();
        if (sps.hasScalingLists && !readScalingLists(reader, sps.scalingLists, sps.chromaFormat != 3 ? 8 : 12, nullptr))
            return false;
    }
    sps.log2MaxFrameNumber = reader.ue() + 4;
    sps.pocType = reader.ue();
    if (sps.log2MaxFrameNumber > 16 || sps.pocType > 2)
        return false;
    if (sps.pocType == 0) {
        sps.log2MaxPocLsb = reader.ue() + 4;
        if (sps.log2MaxPocLsb > 16)
            return false;
    } else if (sps.pocType == 1) {
        sps.deltaPocAlwaysZero = reader.bit();
        sps.offsetForNonReference = reader.se();
        sps.offsetForBottom = reader.se();
        sps.pocCycleLength = reader.ue();
        if (sps.pocCycleLength > 255)
            return false;
        for (int i = 0; i < sps.pocCycleLength; i++)
            sps.pocCycle[i] = reader.se();
    }
    sps.maxReferences = reader.ue();
    sps.frameNumberGaps = reader.bit();
    sps.widthMbs = reader.ue() + 1;
    sps.heightMbs = reader.ue() + 1;
    sps.frameMbsOnly = reader.bit();
    if (!sps.frameMbsOnly)
        sps.mbaff = reader.bit();
    sps.direct8x8 = reader.bit();
    if (reader.bit()) {
        const int unitX = sps.chromaFormat == 0 || sps.separatePlanes ? 1 : 2;
        const int unitY = (sps.chromaFormat == 1 ? unitX : 1) * (2 - sps.frameMbsOnly);
        sps.cropLeft = reader.ue() * unitX;
        sps.cropRight = reader.ue() * unitX;
        sps.cropTop = reader.ue() * unitY;
        sps.cropBottom = reader.ue() * unitY;
    }
    return !reader.failed() && sps.maxReferences <= 16 && sps.widthMbs <= 1024 && sps.heightMbs <= 1024;
}

static bool parsePps(const Rbsp& rbsp, const std::map<int, Sps>& spsMap, int& id, Pps& pps)
{
    BitReader reader(rbsp);
    reader.skip(8);
    id = reader.ue();
    pps.sps = reader.ue();
    if (id > 255)
        return false;
    const auto found = spsMap.find(pps.sps);
    if (found == spsMap.end())
        return false;
    const Sps& sps = found->second;
    pps.cabac = reader.bit();
    pps.bottomPoc = reader.bit();
    pps.sliceGroups = reader.ue() + 1;
    if (pps.sliceGroups > 1)
        return false;
    pps.referenceCounts[0] = reader.ue() + 1;
    pps.referenceCounts[1] = reader.ue() + 1;
    if (pps.referenceCounts[0] > 32 || pps.referenceCounts[1] > 32)
        return false;
    pps.weightedPrediction = reader.bit();
    pps.weightedBiprediction = reader.bits(2);
    pps.qp = reader.se() + 26;
    pps.qs = reader.se() + 26;
    pps.chromaQpOffset = reader.se();
    pps.deblockingControl = reader.bit();
    pps.constrainedIntra = reader.bit();
    pps.redundantPictures = reader.bit();
    pps.secondChromaQpOffset = pps.chromaQpOffset;
    pps.scalingLists = sps.scalingLists;
    if (reader.more()) {
        pps.transform8x8 = reader.bit();
        if (reader.bit()) {
            const int count = 6 + (sps.chromaFormat != 3 ? 2 : 6) * pps.transform8x8;
            if (!readScalingLists(reader, pps.scalingLists, count, sps.hasScalingLists ? &sps.scalingLists : nullptr))
                return false;
        }
        pps.secondChromaQpOffset = reader.se();
    }
    return !reader.failed();
}

static bool parseSliceHeader(const Rbsp& rbsp, const std::map<int, Sps>& spsMap, const std::map<int, Pps>& ppsMap, Slice& slice)
{
    BitReader reader(rbsp);
    reader.skip(8);
    slice.firstMb = reader.ue();
    slice.type = int(reader.ue()) % 5;
    slice.pps = reader.ue();
    const auto foundPps = ppsMap.find(slice.pps);
    if (foundPps == ppsMap.end())
        return false;
    const Pps& pps = foundPps->second;
    const auto foundSps = spsMap.find(pps.sps);
    if (foundSps == spsMap.end())
        return false;
    const Sps& sps = foundSps->second;
    if (sps.separatePlanes)
        reader.skip(2);
    slice.frameNumber = reader.bits(sps.log2MaxFrameNumber);
    // No fields.
    if (!sps.frameMbsOnly && reader.bit())
        return false;
    if (slice.unitType == IdrUnit)
        slice.idrId = reader.ue();
    if (sps.pocType == 0) {
        slice.pocLsb = reader.bits(sps.log2MaxPocLsb);
        if (pps.bottomPoc)
            slice.deltaPocBottom = reader.se();
    }
    if (sps.pocType == 1 && !sps.deltaPocAlwaysZero) {
        slice.deltaPoc[0] = reader.se();
        if (pps.bottomPoc)
            slice.deltaPoc[1] = reader.se();
    }
    if (pps.redundantPictures && reader.ue() != 0)
        return false;
    if (slice.type == B)
        slice.directSpatial = reader.bit();
    slice.referenceCounts[0] = pps.referenceCounts[0];
    slice.referenceCounts[1] = pps.referenceCounts[1];
    if (slice.type == P || slice.type == B || slice.type == SP) {
        if (reader.bit()) {
            slice.referenceCounts[0] = reader.ue() + 1;
            if (slice.type == B)
                slice.referenceCounts[1] = reader.ue() + 1;
        }
        if (slice.referenceCounts[0] > 32 || slice.referenceCounts[1] > 32)
            return false;
    }
    if (slice.type != B)
        slice.referenceCounts[1] = 0;
    if (slice.type == I || slice.type == SI)
        slice.referenceCounts[0] = 0;
    for (int list = 0; list < 2; list++) {
        if (list == 0 && (slice.type == I || slice.type == SI))
            continue;
        if (list == 1 && slice.type != B)
            continue;
        slice.modified[list] = reader.bit();
        if (!slice.modified[list])
            continue;
        for (int i = 0;; i++) {
            const uint32_t operation = reader.ue();
            if (operation == 3)
                break;
            if (operation > 2 || i > 32 || reader.failed())
                return false;
            slice.modifications[list].push_back({operation, reader.ue()});
        }
    }
    if ((pps.weightedPrediction && (slice.type == P || slice.type == SP)) || (pps.weightedBiprediction == 1 && slice.type == B)) {
        slice.lumaLog2Denominator = reader.ue();
        const bool chroma = sps.chromaFormat != 0 && !sps.separatePlanes;
        if (chroma)
            slice.chromaLog2Denominator = reader.ue();
        if (slice.lumaLog2Denominator > 7 || slice.chromaLog2Denominator > 7)
            return false;
        for (int list = 0; list < (slice.type == B ? 2 : 1); list++) {
            for (int i = 0; i < slice.referenceCounts[list]; i++) {
                slice.lumaWeight[list][i] = 1 << slice.lumaLog2Denominator;
                slice.lumaOffset[list][i] = 0;
                if (reader.bit()) {
                    slice.lumaWeighted[list] = true;
                    slice.lumaWeight[list][i] = reader.se();
                    slice.lumaOffset[list][i] = reader.se();
                }
                for (int j = 0; j < 2; j++) {
                    slice.chromaWeight[list][i][j] = 1 << slice.chromaLog2Denominator;
                    slice.chromaOffset[list][i][j] = 0;
                }
                if (chroma && reader.bit()) {
                    slice.chromaWeighted[list] = true;
                    for (int j = 0; j < 2; j++) {
                        slice.chromaWeight[list][i][j] = reader.se();
                        slice.chromaOffset[list][i][j] = reader.se();
                    }
                }
            }
        }
    }
    if (slice.referenceIdc != 0) {
        if (slice.unitType == IdrUnit) {
            slice.noOutputOfPrior = reader.bit();
            slice.longTermReference = reader.bit();
        } else {
            slice.adaptiveMarking = reader.bit();
            if (slice.adaptiveMarking) {
                for (int i = 0;; i++) {
                    const uint32_t operation = reader.ue();
                    if (operation == 0)
                        break;
                    if (operation > 6 || i > 66 || reader.failed())
                        return false;
                    Marking marking;
                    marking.operation = operation;
                    if (operation == 1 || operation == 3)
                        marking.difference = reader.ue() + 1;
                    if (operation == 2)
                        marking.longTermNumber = reader.ue();
                    if (operation == 3 || operation == 6)
                        marking.longTermIndex = reader.ue();
                    if (operation == 4)
                        marking.longTermCount = reader.ue();
                    slice.markings.push_back(marking);
                }
            }
        }
    }
    if (pps.cabac && slice.type != I && slice.type != SI)
        slice.cabacInit = reader.ue();
    slice.qpDelta = reader.se();
    if (slice.type == SP)
        reader.bit();
    if (slice.type == SP || slice.type == SI)
        reader.se();
    if (pps.deblockingControl) {
        slice.disableDeblocking = reader.ue();
        if (slice.disableDeblocking != 1) {
            slice.alphaOffset = reader.se();
            slice.betaOffset = reader.se();
        }
    }
    slice.headerBits = reader.position();
    return !reader.failed();
}

static VAPictureH264 vaPicture(const Reference* reference)
{
    VAPictureH264 picture = {};
    if (!reference || !reference->surface) {
        picture.picture_id = VA_INVALID_SURFACE;
        picture.flags = VA_PICTURE_H264_INVALID;
        return picture;
    }
    picture.picture_id = reference->surface->id();
    picture.frame_idx = reference->longTerm ? reference->longTermIndex : reference->frameNumber;
    if (reference->longTerm)
        picture.flags = VA_PICTURE_H264_LONG_TERM_REFERENCE;
    else if (reference->shortTerm)
        picture.flags = VA_PICTURE_H264_SHORT_TERM_REFERENCE;
    picture.TopFieldOrderCnt = reference->top;
    picture.BottomFieldOrderCnt = reference->bottom;
    return picture;
}

namespace {

class H264Decoder : public Decoder
{
public:
    bool configure(const uint8_t* configuration, size_t size);
    bool decode(const uint8_t* data, size_t size, std::shared_ptr<Surface>& surface) override;

private:
    bool parseParameters(const uint8_t* data, size_t size);
    bool decodePicture(const std::vector<Slice>& slices, std::shared_ptr<Surface>& surface);
    void updateNumbers(int frameNumber, int maxFrameNumber);
    void fillGap(const Sps& sps, int frameNumber);
    void slideWindow(const Sps& sps);
    void removeUnused();
    std::vector<Reference*> initialList(const Slice& slice, int list, int poc);
    bool addSlice(const Sps& sps, const Slice& slice, int poc);

    int m_lengthSize = 4;
    std::map<int, Sps> m_sps;
    // They depend on the sequence parameter sets and are parsed again with them.
    std::map<int, Rbsp> m_ppsData;
    std::map<int, Pps> m_pps;
    std::shared_ptr<VaDisplay> m_display;
    std::unique_ptr<Context> m_context;
    int m_widthMbs = 0;
    int m_heightMbs = 0;
    std::vector<Reference> m_references;
    int m_maxLongTermIndex = -1;
    int m_previousFrameNumber = 0;
    int m_frameNumberOffset = 0;
    int m_previousReferenceFrameNumber = 0;
    int m_previousPocMsb = 0;
    int m_previousPocLsb = 0;
    bool m_previousReset = false;
    int m_previousTop = 0;
    bool m_started = false;
};

bool H264Decoder::configure(const uint8_t* configuration, size_t size)
{
    if (size < 7 || configuration[0] != 1)
        return false;
    m_lengthSize = (configuration[4] & 3) + 1;
    size_t offset = 6;
    int count = configuration[5] & 31;
    for (int pass = 0; pass < 2; pass++) {
        for (int i = 0; i < count; i++) {
            if (offset + 2 > size)
                return false;
            const size_t length = configuration[offset] << 8 | configuration[offset + 1];
            offset += 2;
            if (length == 0 || offset + length > size)
                return false;
            if (!parseParameters(configuration + offset, length))
                return false;
            offset += length;
        }
        if (pass == 1)
            break;
        if (offset >= size)
            return false;
        count = configuration[offset++];
    }
    if (m_sps.empty() || m_pps.empty())
        return false;
    const Sps& sps = m_sps.begin()->second;
    if (!sps.frameMbsOnly || sps.chromaFormat != 1 || sps.lumaDepth != 8 || sps.chromaDepth != 8)
        return false;
    VAProfile profile;
    if (sps.profile == 66)
        profile = VAProfileH264ConstrainedBaseline;
    else if (sps.profile == 77)
        profile = VAProfileH264Main;
    else if (sps.profile == 100)
        profile = VAProfileH264High;
    else
        return false;
    m_display = VaDisplay::open();
    if (!m_display || !m_display->supports(profile))
        return false;
    m_widthMbs = sps.widthMbs;
    m_heightMbs = sps.heightMbs;
    codedWidth = m_widthMbs * 16;
    codedHeight = m_heightMbs * 16;
    if (sps.cropLeft + sps.cropRight < codedWidth && sps.cropTop + sps.cropBottom < codedHeight) {
        width = codedWidth - sps.cropLeft - sps.cropRight;
        height = codedHeight - sps.cropTop - sps.cropBottom;
    }
    depth = 8;
    m_context = std::make_unique<Context>(m_display, profile, codedWidth, codedHeight, 8);
    return m_context->isValid();
}

bool H264Decoder::parseParameters(const uint8_t* data, size_t size)
{
    const uint8_t type = data[0] & 31;
    Rbsp rbsp = unescape(data, size);
    int id = 0;
    if (type == SpsUnit) {
        Sps sps;
        if (!parseSps(rbsp, id, sps))
            return false;
        m_sps[id] = sps;
        for (const auto& [ppsId, ppsData] : m_ppsData) {
            Pps pps;
            int parsedId = 0;
            if (parsePps(ppsData, m_sps, parsedId, pps))
                m_pps[ppsId] = pps;
        }
    } else if (type == PpsUnit) {
        Pps pps;
        if (!parsePps(rbsp, m_sps, id, pps))
            return false;
        m_pps[id] = pps;
        m_ppsData[id] = std::move(rbsp);
    }
    return true;
}

void H264Decoder::updateNumbers(int frameNumber, int maxFrameNumber)
{
    for (Reference& reference : m_references) {
        if (reference.shortTerm) {
            reference.frameNumberWrap
                = reference.frameNumber > frameNumber ? reference.frameNumber - maxFrameNumber : reference.frameNumber;
        }
    }
}

void H264Decoder::slideWindow(const Sps& sps)
{
    const int count = std::count_if(m_references.begin(), m_references.end(),
                                    [](const Reference& reference) { return reference.shortTerm || reference.longTerm; });
    if (count < std::max(sps.maxReferences, 1))
        return;
    Reference* oldest = nullptr;
    for (Reference& reference : m_references) {
        if (reference.shortTerm && (!oldest || reference.frameNumberWrap < oldest->frameNumberWrap))
            oldest = &reference;
    }
    if (oldest)
        oldest->shortTerm = false;
}

void H264Decoder::removeUnused()
{
    std::erase_if(m_references, [](const Reference& reference) { return !reference.shortTerm && !reference.longTerm; });
}

void H264Decoder::fillGap(const Sps& sps, int frameNumber)
{
    const int maxFrameNumber = 1 << sps.log2MaxFrameNumber;
    if (m_previousReferenceFrameNumber == frameNumber)
        return;
    int number = (m_previousReferenceFrameNumber + 1) % maxFrameNumber;
    for (int i = 0; number != frameNumber && i < maxFrameNumber; i++) {
        updateNumbers(number, maxFrameNumber);
        slideWindow(sps);
        removeUnused();
        Reference reference;
        reference.frameNumber = number;
        reference.frameNumberWrap = number;
        reference.shortTerm = true;
        reference.missing = true;
        m_references.push_back(std::move(reference));
        m_previousReferenceFrameNumber = number;
        m_previousFrameNumber = number;
        number = (number + 1) % maxFrameNumber;
    }
}

std::vector<Reference*> H264Decoder::initialList(const Slice& slice, int list, int poc)
{
    std::vector<Reference*> shortTerm;
    std::vector<Reference*> longTerm;
    for (Reference& reference : m_references) {
        if (reference.shortTerm)
            shortTerm.push_back(&reference);
        else if (reference.longTerm)
            longTerm.push_back(&reference);
    }
    std::sort(longTerm.begin(), longTerm.end(),
              [](const Reference* a, const Reference* b) { return a->longTermIndex < b->longTermIndex; });
    std::vector<Reference*> result;
    if (slice.type == P || slice.type == SP) {
        std::sort(shortTerm.begin(), shortTerm.end(),
                  [](const Reference* a, const Reference* b) { return a->frameNumberWrap > b->frameNumberWrap; });
        result = shortTerm;
    } else {
        std::vector<Reference*> before;
        std::vector<Reference*> after;
        for (Reference* reference : shortTerm) {
            if (reference->missing)
                continue;
            (reference->poc() < poc ? before : after).push_back(reference);
        }
        std::sort(before.begin(), before.end(), [](const Reference* a, const Reference* b) { return a->poc() > b->poc(); });
        std::sort(after.begin(), after.end(), [](const Reference* a, const Reference* b) { return a->poc() < b->poc(); });
        result = list == 0 ? before : after;
        const std::vector<Reference*>& rest = list == 0 ? after : before;
        result.insert(result.end(), rest.begin(), rest.end());
    }
    result.insert(result.end(), longTerm.begin(), longTerm.end());
    return result;
}

bool H264Decoder::addSlice(const Sps& sps, const Slice& slice, int poc)
{
    if (!m_pps.contains(slice.pps))
        return false;
    VASliceParameterBufferH264 parameters = {};
    parameters.slice_data_size = slice.size;
    parameters.slice_data_bit_offset = slice.headerBits;
    parameters.first_mb_in_slice = slice.firstMb;
    parameters.slice_type = slice.type;
    parameters.direct_spatial_mv_pred_flag = slice.directSpatial;
    parameters.num_ref_idx_l0_active_minus1 = std::max(slice.referenceCounts[0] - 1, 0);
    parameters.num_ref_idx_l1_active_minus1 = std::max(slice.referenceCounts[1] - 1, 0);
    parameters.cabac_init_idc = slice.cabacInit;
    parameters.slice_qp_delta = slice.qpDelta;
    parameters.disable_deblocking_filter_idc = slice.disableDeblocking;
    parameters.slice_alpha_c0_offset_div2 = slice.alphaOffset;
    parameters.slice_beta_offset_div2 = slice.betaOffset;
    for (int i = 0; i < 32; i++) {
        parameters.RefPicList0[i] = vaPicture(nullptr);
        parameters.RefPicList1[i] = vaPicture(nullptr);
    }
    bool ok = true;
    for (int list = 0; list < 2 && ok; list++) {
        const int count = slice.referenceCounts[list];
        if (count == 0)
            continue;
        std::vector<Reference*> references = initialList(slice, list, poc);
        if (list == 1 && references.size() > 1 && initialList(slice, 0, poc) == references)
            std::swap(references[0], references[1]);
        if (references.size() > size_t(count))
            references.resize(count);
        references.resize(count + 1);
        if (slice.modified[list]) {
            const int maxFrameNumber = 1 << sps.log2MaxFrameNumber;
            int predicted = slice.frameNumber;
            int index = 0;
            for (const Modification& modification : slice.modifications[list]) {
                if (index >= count) {
                    ok = false;
                    break;
                }
                Reference* found = nullptr;
                const bool longTerm = modification.operation >= 2;
                int number = modification.value;
                if (longTerm) {
                    for (Reference& reference : m_references) {
                        if (reference.longTerm && reference.longTermIndex == number)
                            found = &reference;
                    }
                } else {
                    if (modification.operation == 0) {
                        predicted -= modification.value + 1;
                        if (predicted < 0)
                            predicted += maxFrameNumber;
                    } else {
                        predicted += modification.value + 1;
                        if (predicted >= maxFrameNumber)
                            predicted -= maxFrameNumber;
                    }
                    number = predicted > slice.frameNumber ? predicted - maxFrameNumber : predicted;
                    for (Reference& reference : m_references) {
                        if (reference.shortTerm && reference.frameNumberWrap == number)
                            found = &reference;
                    }
                }
                for (int i = count; i > index; i--)
                    references[i] = references[i - 1];
                references[index++] = found;
                int kept = index;
                for (int i = index; i <= count; i++) {
                    Reference* reference = references[i];
                    const bool same = reference
                        && (longTerm ? reference->longTerm && reference->longTermIndex == number
                                     : reference->shortTerm && reference->frameNumberWrap == number);
                    if (!same)
                        references[kept++] = reference;
                }
            }
            if (!ok)
                break;
        }
        references.resize(count);
        VAPictureH264* pictures = list == 0 ? parameters.RefPicList0 : parameters.RefPicList1;
        for (size_t i = 0; i < references.size() && i < 32; i++)
            pictures[i] = vaPicture(references[i]);
    }
    parameters.luma_log2_weight_denom = slice.lumaLog2Denominator;
    parameters.chroma_log2_weight_denom = slice.chromaLog2Denominator;
    parameters.luma_weight_l0_flag = slice.lumaWeighted[0];
    parameters.chroma_weight_l0_flag = slice.chromaWeighted[0];
    parameters.luma_weight_l1_flag = slice.lumaWeighted[1];
    parameters.chroma_weight_l1_flag = slice.chromaWeighted[1];
    memcpy(parameters.luma_weight_l0, slice.lumaWeight[0], sizeof(parameters.luma_weight_l0));
    memcpy(parameters.luma_offset_l0, slice.lumaOffset[0], sizeof(parameters.luma_offset_l0));
    memcpy(parameters.chroma_weight_l0, slice.chromaWeight[0], sizeof(parameters.chroma_weight_l0));
    memcpy(parameters.chroma_offset_l0, slice.chromaOffset[0], sizeof(parameters.chroma_offset_l0));
    memcpy(parameters.luma_weight_l1, slice.lumaWeight[1], sizeof(parameters.luma_weight_l1));
    memcpy(parameters.luma_offset_l1, slice.lumaOffset[1], sizeof(parameters.luma_offset_l1));
    memcpy(parameters.chroma_weight_l1, slice.chromaWeight[1], sizeof(parameters.chroma_weight_l1));
    memcpy(parameters.chroma_offset_l1, slice.chromaOffset[1], sizeof(parameters.chroma_offset_l1));
    return ok && m_context->addBuffer(VASliceParameterBufferType, &parameters, sizeof(parameters))
        && m_context->addBuffer(VASliceDataBufferType, slice.data, slice.size);
}

bool H264Decoder::decodePicture(const std::vector<Slice>& slices, std::shared_ptr<Surface>& surface)
{
    const Slice& first = slices.front();
    const Pps& pps = m_pps.at(first.pps);
    const Sps& sps = m_sps.at(pps.sps);
    if (sps.widthMbs != m_widthMbs || sps.heightMbs != m_heightMbs || !sps.frameMbsOnly || sps.chromaFormat != 1
        || sps.lumaDepth != 8)
        return false;
    const int maxFrameNumber = 1 << sps.log2MaxFrameNumber;
    if (first.unitType == IdrUnit) {
        m_references.clear();
        m_previousReferenceFrameNumber = 0;
    }
    if (!m_started) {
        if (first.unitType != IdrUnit && first.frameNumber != 0)
            m_previousReferenceFrameNumber = (first.frameNumber - 1 + maxFrameNumber) % maxFrameNumber;
    } else if (sps.frameNumberGaps && first.unitType != IdrUnit) {
        fillGap(sps, first.frameNumber);
    }
    int frameNumber = first.frameNumber;
    updateNumbers(frameNumber, maxFrameNumber);

    int top = 0;
    int bottom = 0;
    if (sps.pocType == 0) {
        int msb = 0;
        int previousLsb = 0;
        if (first.unitType != IdrUnit) {
            if (m_previousReset) {
                previousLsb = m_previousTop;
            } else {
                msb = m_previousPocMsb;
                previousLsb = m_previousPocLsb;
            }
        }
        const int maxLsb = 1 << sps.log2MaxPocLsb;
        if (first.pocLsb < previousLsb && previousLsb - first.pocLsb >= maxLsb / 2)
            msb += maxLsb;
        else if (first.pocLsb > previousLsb && first.pocLsb - previousLsb > maxLsb / 2)
            msb -= maxLsb;
        top = msb + first.pocLsb;
        bottom = top + first.deltaPocBottom;
        if (first.referenceIdc != 0) {
            m_previousPocMsb = msb;
            m_previousPocLsb = first.pocLsb;
        }
    } else {
        int offset = 0;
        if (first.unitType != IdrUnit) {
            if (!m_previousReset)
                offset = m_frameNumberOffset;
            if (m_previousFrameNumber > frameNumber)
                offset += 1 << sps.log2MaxFrameNumber;
        }
        m_frameNumberOffset = offset;
        if (sps.pocType == 1) {
            int number = sps.pocCycleLength != 0 ? offset + frameNumber : 0;
            if (first.referenceIdc == 0 && number > 0)
                number--;
            int expected = 0;
            if (number > 0) {
                int delta = 0;
                for (int i = 0; i < sps.pocCycleLength; i++)
                    delta += sps.pocCycle[i];
                expected = (number - 1) / sps.pocCycleLength * delta;
                for (int i = 0; i <= (number - 1) % sps.pocCycleLength; i++)
                    expected += sps.pocCycle[i];
            }
            if (first.referenceIdc == 0)
                expected += sps.offsetForNonReference;
            top = expected + first.deltaPoc[0];
            bottom = top + sps.offsetForBottom + first.deltaPoc[1];
        } else {
            if (first.unitType != IdrUnit)
                top = 2 * (offset + frameNumber) - (first.referenceIdc == 0 ? 1 : 0);
            bottom = top;
        }
    }

    std::shared_ptr<Surface> target = m_context->takeSurface();
    if (!target)
        return false;
    VAPictureParameterBufferH264 picture = {};
    picture.CurrPic.picture_id = target->id();
    picture.CurrPic.frame_idx = frameNumber;
    picture.CurrPic.TopFieldOrderCnt = top;
    picture.CurrPic.BottomFieldOrderCnt = bottom;
    size_t count = 0;
    for (const Reference& reference : m_references) {
        if (!reference.missing && (reference.shortTerm || reference.longTerm) && count < 16)
            picture.ReferenceFrames[count++] = vaPicture(&reference);
    }
    for (; count < 16; count++)
        picture.ReferenceFrames[count] = vaPicture(nullptr);
    picture.picture_width_in_mbs_minus1 = sps.widthMbs - 1;
    picture.picture_height_in_mbs_minus1 = sps.heightMbs - 1;
    picture.bit_depth_luma_minus8 = sps.lumaDepth - 8;
    picture.bit_depth_chroma_minus8 = sps.chromaDepth - 8;
    picture.num_ref_frames = sps.maxReferences;
    picture.seq_fields.bits.chroma_format_idc = sps.chromaFormat;
    picture.seq_fields.bits.residual_colour_transform_flag = sps.separatePlanes;
    picture.seq_fields.bits.gaps_in_frame_num_value_allowed_flag = sps.frameNumberGaps;
    picture.seq_fields.bits.frame_mbs_only_flag = sps.frameMbsOnly;
    picture.seq_fields.bits.mb_adaptive_frame_field_flag = sps.mbaff;
    picture.seq_fields.bits.direct_8x8_inference_flag = sps.direct8x8;
    picture.seq_fields.bits.MinLumaBiPredSize8x8 = sps.level >= 31;
    picture.seq_fields.bits.log2_max_frame_num_minus4 = sps.log2MaxFrameNumber - 4;
    picture.seq_fields.bits.pic_order_cnt_type = sps.pocType;
    picture.seq_fields.bits.log2_max_pic_order_cnt_lsb_minus4 = sps.log2MaxPocLsb - 4;
    picture.seq_fields.bits.delta_pic_order_always_zero_flag = sps.deltaPocAlwaysZero;
    picture.pic_init_qp_minus26 = pps.qp - 26;
    picture.pic_init_qs_minus26 = pps.qs - 26;
    picture.chroma_qp_index_offset = pps.chromaQpOffset;
    picture.second_chroma_qp_index_offset = pps.secondChromaQpOffset;
    picture.pic_fields.bits.entropy_coding_mode_flag = pps.cabac;
    picture.pic_fields.bits.weighted_pred_flag = pps.weightedPrediction;
    picture.pic_fields.bits.weighted_bipred_idc = pps.weightedBiprediction;
    picture.pic_fields.bits.transform_8x8_mode_flag = pps.transform8x8;
    picture.pic_fields.bits.constrained_intra_pred_flag = pps.constrainedIntra;
    picture.pic_fields.bits.pic_order_present_flag = pps.bottomPoc;
    picture.pic_fields.bits.deblocking_filter_control_present_flag = pps.deblockingControl;
    picture.pic_fields.bits.redundant_pic_cnt_present_flag = pps.redundantPictures;
    picture.pic_fields.bits.reference_pic_flag = first.referenceIdc != 0;
    picture.frame_num = first.frameNumber;

    VAIQMatrixBufferH264 matrix = {};
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 16; j++)
            matrix.ScalingList4x4[i][zigzag4x4[j]] = pps.scalingLists.small[i][j];
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j++)
            matrix.ScalingList8x8[i][zigzag8x8[j]] = pps.scalingLists.large[i][j];
    }

    bool ok = m_context->addBuffer(VAPictureParameterBufferType, &picture, sizeof(picture))
        && m_context->addBuffer(VAIQMatrixBufferType, &matrix, sizeof(matrix));
    for (const Slice& slice : slices) {
        ok = ok && addSlice(sps, slice, std::min(top, bottom));
        if (!ok)
            break;
    }
    if (!ok) {
        m_context->discard();
        return false;
    }
    if (!m_context->render(target->id()))
        return false;

    m_previousReset = false;
    bool shortTerm = first.referenceIdc != 0;
    bool longTerm = false;
    int longTermIndex = 0;
    if (first.referenceIdc != 0) {
        if (first.unitType == IdrUnit) {
            for (Reference& reference : m_references)
                reference.shortTerm = reference.longTerm = false;
            if (first.longTermReference) {
                m_maxLongTermIndex = 0;
                shortTerm = false;
                longTerm = true;
            } else {
                m_maxLongTermIndex = -1;
            }
        } else if (first.adaptiveMarking) {
            for (const Marking& marking : first.markings) {
                const int number = first.frameNumber - marking.difference;
                switch (marking.operation) {
                case 1:
                    for (Reference& reference : m_references) {
                        if (reference.shortTerm && reference.frameNumberWrap == number)
                            reference.shortTerm = false;
                    }
                    break;
                case 2:
                    for (Reference& reference : m_references) {
                        if (reference.longTerm && reference.longTermIndex == marking.longTermNumber)
                            reference.longTerm = false;
                    }
                    break;
                case 3:
                    for (Reference& reference : m_references) {
                        if (reference.longTerm && reference.longTermIndex == marking.longTermIndex)
                            reference.longTerm = false;
                    }
                    for (Reference& reference : m_references) {
                        if (reference.shortTerm && reference.frameNumberWrap == number) {
                            reference.shortTerm = false;
                            reference.longTerm = true;
                            reference.longTermIndex = marking.longTermIndex;
                        }
                    }
                    break;
                case 4:
                    m_maxLongTermIndex = marking.longTermCount - 1;
                    for (Reference& reference : m_references) {
                        if (reference.longTerm && reference.longTermIndex > m_maxLongTermIndex)
                            reference.longTerm = false;
                    }
                    break;
                case 5:
                    for (Reference& reference : m_references)
                        reference.shortTerm = reference.longTerm = false;
                    m_maxLongTermIndex = -1;
                    m_previousReset = true;
                    break;
                case 6:
                    for (Reference& reference : m_references) {
                        if (reference.longTerm && reference.longTermIndex == marking.longTermIndex)
                            reference.longTerm = false;
                    }
                    shortTerm = false;
                    longTerm = true;
                    longTermIndex = marking.longTermIndex;
                    break;
                }
            }
        } else {
            slideWindow(sps);
        }
    }
    if (m_previousReset) {
        const int base = std::min(top, bottom);
        top -= base;
        bottom -= base;
        frameNumber = 0;
        m_previousTop = top;
        if (sps.pocType == 0) {
            m_previousPocMsb = 0;
            m_previousPocLsb = top;
        }
    }
    m_previousFrameNumber = frameNumber;
    if (first.referenceIdc != 0)
        m_previousReferenceFrameNumber = frameNumber;

    surface = target;
    removeUnused();
    if (shortTerm || longTerm) {
        Reference reference;
        reference.surface = std::move(target);
        reference.frameNumber = frameNumber;
        reference.frameNumberWrap = frameNumber;
        reference.longTermIndex = longTermIndex;
        reference.shortTerm = shortTerm;
        reference.longTerm = longTerm;
        reference.top = top;
        reference.bottom = bottom;
        m_references.push_back(std::move(reference));
    }
    while (int(m_references.size()) > std::max(sps.maxReferences, 1)) {
        m_references.erase(std::min_element(m_references.begin(), m_references.end(), [](const Reference& a, const Reference& b) {
            return a.frameNumberWrap < b.frameNumberWrap;
        }));
    }
    m_started = true;
    return true;
}

bool H264Decoder::decode(const uint8_t* data, size_t size, std::shared_ptr<Surface>& surface)
{
    surface.reset();
    std::vector<Slice> slices;
    const auto unit = [this, &slices](const uint8_t* unit, size_t length) {
        const uint8_t type = unit[0] & 31;
        if (type == SpsUnit || type == PpsUnit)
            return parseParameters(unit, length);
        if (type != SliceUnit && type != IdrUnit)
            return true;
        Slice blank;
        blank.data = unit;
        blank.size = length;
        blank.referenceIdc = unit[0] >> 5 & 3;
        blank.unitType = type;
        // The header is in the beginning, the rest need not be copied.
        Slice slice = blank;
        if (length <= 1024 || !parseSliceHeader(unescape(unit, 1024), m_sps, m_pps, slice)) {
            slice = blank;
            if (!parseSliceHeader(unescape(unit, length), m_sps, m_pps, slice))
                return false;
        }
        slices.push_back(std::move(slice));
        return true;
    };
    size_t offset = 0;
    while (offset + m_lengthSize <= size) {
        size_t length = 0;
        for (int i = 0; i < m_lengthSize; i++)
            length = length << 8 | data[offset + i];
        offset += m_lengthSize;
        if (length > size - offset)
            return false;
        if (length == 0)
            continue;
        if (!unit(data + offset, length))
            return false;
        offset += length;
    }
    if (offset != size)
        return false;
    return slices.empty() || decodePicture(slices, surface);
}

} // namespace

std::unique_ptr<Decoder> makeH264Decoder(const uint8_t* configuration, size_t size)
{
    auto decoder = std::make_unique<H264Decoder>();
    if (!decoder->configure(configuration, size))
        return nullptr;
    return decoder;
}

} // namespace cutefish::reef::video

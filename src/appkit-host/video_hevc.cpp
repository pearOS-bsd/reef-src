// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HEVC: the parameter sets, the slice headers, the reference picture sets. 8 and 10 bits, 4:2:0.

#include "video_private.h"

#include <algorithm>
#include <cstring>
#include <map>

namespace cutefish::reef::video {

namespace {

enum UnitType : uint8_t {
    RadlN = 6,
    RaslN = 8,
    RaslR = 9,
    LastSubLayerUnit = 14,
    FirstIrap = 16,
    LastBla = 18,
    IdrWithRadl = 19,
    IdrNoLeading = 20,
    LastIrap = 23,
    SpsUnit = 33,
    PpsUnit = 34,
    EndOfSequenceUnit = 36,
};

enum SliceType {
    B = 0,
    P = 1,
    I = 2,
};

// In the order of the stream.
struct ScalingLists
{
    uint8_t lists[4][6][64];
    uint8_t dc[4][6];
};

struct ShortTermSet
{
    int negativeCount = 0;
    int positiveCount = 0;
    int negativeDelta[16] = {};
    int positiveDelta[16] = {};
    bool negativeUsed[16] = {};
    bool positiveUsed[16] = {};
};

struct Sps
{
    int subLayers = 1;
    int profile = 0;
    int chromaFormat = 1;
    bool separatePlanes = false;
    int width = 0;
    int height = 0;
    int cropLeft = 0;
    int cropRight = 0;
    int cropTop = 0;
    int cropBottom = 0;
    int lumaDepth = 8;
    int chromaDepth = 8;
    int log2MaxPocLsb = 4;
    int maxDecPicBuffering = 1;
    int maxReorder = 0;
    int log2MinCodingBlock = 3;
    int log2CodingBlockRange = 0;
    int log2MinTransformBlock = 2;
    int log2TransformBlockRange = 0;
    int transformDepthInter = 0;
    int transformDepthIntra = 0;
    bool hasScalingLists = false;
    ScalingLists scalingLists;
    bool amp = false;
    bool sao = false;
    bool pcm = false;
    int pcmLumaDepth = 8;
    int pcmChromaDepth = 8;
    int log2MinPcm = 3;
    int log2PcmRange = 0;
    bool pcmLoopFilterDisabled = false;
    int shortTermSetCount = 0;
    ShortTermSet shortTermSets[65];
    bool longTermReferences = false;
    int longTermCount = 0;
    int longTermPocLsb[33] = {};
    bool longTermUsed[33] = {};
    bool temporalMvp = false;
    bool strongIntraSmoothing = false;

    int codingTreeBlock() const { return 1 << (log2MinCodingBlock + log2CodingBlockRange); }
    int columns() const { return (width + codingTreeBlock() - 1) / codingTreeBlock(); }
    int rows() const { return (height + codingTreeBlock() - 1) / codingTreeBlock(); }
    bool hasChroma() const { return chromaFormat != 0 && !separatePlanes; }
};

struct Pps
{
    int sps = 0;
    bool dependentSlices = false;
    bool outputFlag = false;
    int extraHeaderBits = 0;
    bool signHiding = false;
    bool cabacInit = false;
    int referenceCounts[2] = {1, 1};
    int qp = 26;
    bool constrainedIntra = false;
    bool transformSkip = false;
    bool cuQpDelta = false;
    int cuQpDeltaDepth = 0;
    int cbQpOffset = 0;
    int crQpOffset = 0;
    bool sliceChromaQpOffsets = false;
    bool weightedPrediction = false;
    bool weightedBiprediction = false;
    bool transquantBypass = false;
    bool tiles = false;
    bool entropySync = false;
    int tileColumns = 1;
    int tileRows = 1;
    bool uniformTiles = true;
    int columnWidths[20] = {};
    int rowHeights[22] = {};
    bool loopFilterAcrossTiles = true;
    bool loopFilterAcrossSlices = false;
    bool deblockingControl = false;
    bool deblockingOverride = false;
    bool deblockingDisabled = false;
    int betaOffset = 0;
    int tcOffset = 0;
    bool hasScalingLists = false;
    ScalingLists scalingLists;
    bool listsModification = false;
    int parallelMergeLevel = 2;
    bool headerExtension = false;
};

struct SliceHeader
{
    bool first = false;
    bool dependent = false;
    int pps = 0;
    int address = 0;
    int type = I;
    bool output = true;
    int colourPlane = 0;
    int pocLsb = 0;
    bool shortTermFromSps = false;
    ShortTermSet shortTerm;
    int shortTermIndex = 0;
    size_t shortTermBits = 0;
    int longTermSpsCount = 0;
    int longTermCount = 0;
    int longTermPocLsb[32] = {};
    bool longTermUsed[32] = {};
    bool longTermHasMsb[32] = {};
    int longTermMsb[32] = {};
    bool temporalMvp = false;
    bool saoLuma = false;
    bool saoChroma = false;
    int referenceCounts[2] = {};
    bool modified[2] = {};
    int entries[2][16] = {};
    bool mvdL1Zero = false;
    bool cabacInit = false;
    bool collocatedFromL0 = true;
    int collocatedIndex = 0;
    int lumaLog2Denominator = 0;
    int chromaLog2Denominator = 0;
    int8_t lumaWeight[2][15] = {};
    int8_t lumaOffset[2][15] = {};
    int8_t chromaWeight[2][15][2] = {};
    int8_t chromaOffset[2][15][2] = {};
    int mergeCandidates = 0;
    int qpDelta = 0;
    int cbQpOffset = 0;
    int crQpOffset = 0;
    bool deblockingDisabled = false;
    int betaOffset = 0;
    int tcOffset = 0;
    bool loopFilterAcross = false;
    int entryPoints = 0;
};

struct Slice
{
    const uint8_t* data = nullptr;
    size_t size = 0;
    int unitType = 0;
    int temporalId = 0;
    SliceHeader header;
    size_t headerBytes = 0;
    size_t headerEscapes = 0;
};

struct Reference
{
    std::shared_ptr<Surface> surface;
    int poc = 0;
    bool shortTerm = false;
    bool longTerm = false;

    bool isUsed() const { return shortTerm || longTerm; }
};

} // namespace

static const uint8_t defaultIntra[64] = {
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 16, 17, 16, 17, 18, 17, 18, 18, 17, 18, 21, 19, 20, 21, 20, 19, 21, 24, 22, 22, 24,
    24, 22, 22, 24, 25, 25, 27, 30, 27, 25, 25, 29, 31, 35, 35, 31, 29, 36, 41, 44, 41, 36, 47, 54, 54, 47, 65, 70, 65, 88, 88, 115,
};
static const uint8_t defaultInter[64] = {
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 20, 20, 20, 20, 20, 20, 20, 24, 24, 24, 24,
    24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 28, 28, 28, 28, 28, 28, 33, 33, 33, 33, 33, 41, 41, 41, 41, 54, 54, 54, 71, 71, 91,
};

static int ceilLog2(uint32_t count)
{
    int bits = 0;
    while ((1u << bits) < count)
        bits++;
    return bits;
}

static bool isIrap(int unitType)
{
    return unitType >= FirstIrap && unitType <= LastIrap;
}

static bool isBla(int unitType)
{
    return unitType >= FirstIrap && unitType <= LastBla;
}

static bool isIdr(int unitType)
{
    return unitType == IdrWithRadl || unitType == IdrNoLeading;
}

static void setDefault(ScalingLists& lists, int sizeId, int matrixId)
{
    if (sizeId == 0)
        memset(lists.lists[sizeId][matrixId], 16, 64);
    else
        memcpy(lists.lists[sizeId][matrixId], matrixId < 3 ? defaultIntra : defaultInter, 64);
    lists.dc[sizeId][matrixId] = 16;
}

static bool readScalingLists(BitReader& reader, ScalingLists& lists)
{
    for (int sizeId = 0; sizeId < 4; sizeId++) {
        const int step = sizeId == 3 ? 3 : 1;
        for (int matrixId = 0; matrixId < 6; matrixId += step) {
            uint8_t* list = lists.lists[sizeId][matrixId];
            const int count = std::min(64, 1 << (4 + (sizeId << 1)));
            if (!reader.bit()) {
                const int delta = reader.ue();
                if (delta == 0) {
                    setDefault(lists, sizeId, matrixId);
                } else {
                    const int reference = matrixId - delta * step;
                    if (reference < 0)
                        return false;
                    memcpy(list, lists.lists[sizeId][reference], 64);
                    lists.dc[sizeId][matrixId] = lists.dc[sizeId][reference];
                }
                continue;
            }
            int next = 8;
            if (sizeId > 1) {
                next = reader.se() + 8;
                if (next < 1 || next > 255)
                    return false;
                lists.dc[sizeId][matrixId] = next;
            }
            for (int i = 0; i < count; i++) {
                const int delta = reader.se();
                if (delta < -128 || delta > 127)
                    return false;
                next = (next + delta + 256) % 256;
                list[i] = next;
            }
            if (sizeId < 2)
                lists.dc[sizeId][matrixId] = list[0];
        }
    }
    // With 4:4:4 the chroma of the largest blocks is scaled like the size below.
    for (int matrixId = 1; matrixId < 6; matrixId++) {
        if (matrixId == 3)
            continue;
        memcpy(lists.lists[3][matrixId], lists.lists[2][matrixId], 64);
        lists.dc[3][matrixId] = lists.dc[2][matrixId];
    }
    return !reader.failed();
}

static bool parseShortTermSet(BitReader& reader, const Sps& sps, int index, ShortTermSet& set)
{
    if (index != 0 && reader.bit()) {
        int deltaIndex = 1;
        if (index == sps.shortTermSetCount)
            deltaIndex = reader.ue() + 1;
        if (deltaIndex > index)
            return false;
        const bool sign = reader.bit();
        const int absolute = reader.ue() + 1;
        if (absolute > 0x8000)
            return false;
        const int delta = sign ? -absolute : absolute;
        const ShortTermSet& reference = sps.shortTermSets[index - deltaIndex];
        bool used[33] = {};
        bool useDelta[33] = {};
        for (int j = 0; j <= reference.negativeCount + reference.positiveCount; j++) {
            used[j] = reader.bit();
            useDelta[j] = used[j] || reader.bit();
        }

        int count = 0;
        for (int j = reference.positiveCount - 1; j >= 0; j--) {
            const int value = reference.positiveDelta[j] + delta;
            if (value < 0 && useDelta[reference.negativeCount + j] && count < 16) {
                set.negativeDelta[count] = value;
                set.negativeUsed[count++] = used[reference.negativeCount + j];
            }
        }
        if (delta < 0 && useDelta[reference.negativeCount + reference.positiveCount] && count < 16) {
            set.negativeDelta[count] = delta;
            set.negativeUsed[count++] = used[reference.negativeCount + reference.positiveCount];
        }
        for (int j = 0; j < reference.negativeCount; j++) {
            const int value = reference.negativeDelta[j] + delta;
            if (value < 0 && useDelta[j] && count < 16) {
                set.negativeDelta[count] = value;
                set.negativeUsed[count++] = used[j];
            }
        }
        set.negativeCount = count;

        count = 0;
        for (int j = reference.negativeCount - 1; j >= 0; j--) {
            const int value = reference.negativeDelta[j] + delta;
            if (value > 0 && useDelta[j] && count < 16) {
                set.positiveDelta[count] = value;
                set.positiveUsed[count++] = used[j];
            }
        }
        if (delta > 0 && useDelta[reference.negativeCount + reference.positiveCount] && count < 16) {
            set.positiveDelta[count] = delta;
            set.positiveUsed[count++] = used[reference.negativeCount + reference.positiveCount];
        }
        for (int j = 0; j < reference.positiveCount; j++) {
            const int value = reference.positiveDelta[j] + delta;
            if (value > 0 && useDelta[reference.negativeCount + j] && count < 16) {
                set.positiveDelta[count] = value;
                set.positiveUsed[count++] = used[reference.negativeCount + j];
            }
        }
        set.positiveCount = count;
    } else {
        set.negativeCount = reader.ue();
        set.positiveCount = reader.ue();
        if (set.negativeCount > 16 || set.positiveCount > 16 || set.negativeCount + set.positiveCount > 16)
            return false;
        int delta = 0;
        for (int i = 0; i < set.negativeCount; i++) {
            delta -= reader.ue() + 1;
            set.negativeDelta[i] = delta;
            set.negativeUsed[i] = reader.bit();
        }
        delta = 0;
        for (int i = 0; i < set.positiveCount; i++) {
            delta += reader.ue() + 1;
            set.positiveDelta[i] = delta;
            set.positiveUsed[i] = reader.bit();
        }
    }
    return !reader.failed();
}

static bool parseSps(const Rbsp& rbsp, int& id, Sps& sps)
{
    BitReader reader(rbsp);
    reader.skip(16);
    reader.skip(4);
    const int subLayers = reader.bits(3);
    reader.skip(1);
    if (subLayers > 6)
        return false;
    sps.subLayers = subLayers + 1;
    // Profile, tier and level, also those of the sub-layers.
    reader.skip(3);
    sps.profile = reader.bits(5);
    reader.skip(88);
    bool profilePresent[8] = {};
    bool levelPresent[8] = {};
    for (int i = 0; i < subLayers; i++) {
        profilePresent[i] = reader.bit();
        levelPresent[i] = reader.bit();
    }
    if (subLayers > 0) {
        for (int i = subLayers; i < 8; i++)
            reader.skip(2);
    }
    for (int i = 0; i < subLayers; i++) {
        if (profilePresent[i])
            reader.skip(88);
        if (levelPresent[i])
            reader.skip(8);
    }
    id = reader.ue();
    if (id > 15)
        return false;
    sps.chromaFormat = reader.ue();
    if (sps.chromaFormat == 3)
        sps.separatePlanes = reader.bit();
    sps.width = reader.ue();
    sps.height = reader.ue();
    if (reader.bit()) {
        const int unitX = sps.chromaFormat == 1 || sps.chromaFormat == 2 ? 2 : 1;
        const int unitY = sps.chromaFormat == 1 ? 2 : 1;
        sps.cropLeft = reader.ue() * unitX;
        sps.cropRight = reader.ue() * unitX;
        sps.cropTop = reader.ue() * unitY;
        sps.cropBottom = reader.ue() * unitY;
    }
    sps.lumaDepth = reader.ue() + 8;
    sps.chromaDepth = reader.ue() + 8;
    sps.log2MaxPocLsb = reader.ue() + 4;
    if (sps.log2MaxPocLsb > 16 || sps.lumaDepth > 16 || sps.chromaDepth > 16)
        return false;
    for (int i = reader.bit() ? 0 : subLayers; i <= subLayers; i++) {
        sps.maxDecPicBuffering = reader.ue() + 1;
        sps.maxReorder = reader.ue();
        reader.ue();
    }
    sps.log2MinCodingBlock = reader.ue() + 3;
    sps.log2CodingBlockRange = reader.ue();
    sps.log2MinTransformBlock = reader.ue() + 2;
    sps.log2TransformBlockRange = reader.ue();
    sps.transformDepthInter = reader.ue();
    sps.transformDepthIntra = reader.ue();
    if (sps.log2MinCodingBlock + sps.log2CodingBlockRange > 6 || sps.maxDecPicBuffering > 16)
        return false;
    for (int sizeId = 0; sizeId < 4; sizeId++) {
        for (int matrixId = 0; matrixId < 6; matrixId++)
            setDefault(sps.scalingLists, sizeId, matrixId);
    }
    sps.hasScalingLists = reader.bit();
    if (sps.hasScalingLists && reader.bit() && !readScalingLists(reader, sps.scalingLists))
        return false;
    sps.amp = reader.bit();
    sps.sao = reader.bit();
    sps.pcm = reader.bit();
    if (sps.pcm) {
        sps.pcmLumaDepth = reader.bits(4) + 1;
        sps.pcmChromaDepth = reader.bits(4) + 1;
        sps.log2MinPcm = reader.ue() + 3;
        sps.log2PcmRange = reader.ue();
        sps.pcmLoopFilterDisabled = reader.bit();
    }
    sps.shortTermSetCount = reader.ue();
    if (sps.shortTermSetCount > 64)
        return false;
    for (int i = 0; i < sps.shortTermSetCount; i++) {
        if (!parseShortTermSet(reader, sps, i, sps.shortTermSets[i]))
            return false;
    }
    sps.longTermReferences = reader.bit();
    if (sps.longTermReferences) {
        sps.longTermCount = reader.ue();
        if (sps.longTermCount > 32)
            return false;
        for (int i = 0; i < sps.longTermCount; i++) {
            sps.longTermPocLsb[i] = reader.bits(sps.log2MaxPocLsb);
            sps.longTermUsed[i] = reader.bit();
        }
    }
    sps.temporalMvp = reader.bit();
    sps.strongIntraSmoothing = reader.bit();
    return !reader.failed() && sps.width > 0 && sps.height > 0;
}

static bool parsePps(const Rbsp& rbsp, const std::map<int, Sps>& spsMap, int& id, Pps& pps)
{
    BitReader reader(rbsp);
    reader.skip(16);
    id = reader.ue();
    pps.sps = reader.ue();
    if (id > 63)
        return false;
    const auto found = spsMap.find(pps.sps);
    if (found == spsMap.end())
        return false;
    const Sps& sps = found->second;
    pps.dependentSlices = reader.bit();
    pps.outputFlag = reader.bit();
    pps.extraHeaderBits = reader.bits(3);
    pps.signHiding = reader.bit();
    pps.cabacInit = reader.bit();
    pps.referenceCounts[0] = reader.ue() + 1;
    pps.referenceCounts[1] = reader.ue() + 1;
    if (pps.referenceCounts[0] > 15 || pps.referenceCounts[1] > 15)
        return false;
    pps.qp = reader.se() + 26;
    pps.constrainedIntra = reader.bit();
    pps.transformSkip = reader.bit();
    pps.cuQpDelta = reader.bit();
    if (pps.cuQpDelta)
        pps.cuQpDeltaDepth = reader.ue();
    pps.cbQpOffset = reader.se();
    pps.crQpOffset = reader.se();
    pps.sliceChromaQpOffsets = reader.bit();
    pps.weightedPrediction = reader.bit();
    pps.weightedBiprediction = reader.bit();
    pps.transquantBypass = reader.bit();
    pps.tiles = reader.bit();
    pps.entropySync = reader.bit();
    if (pps.tiles) {
        pps.tileColumns = reader.ue() + 1;
        pps.tileRows = reader.ue() + 1;
        if (pps.tileColumns > 20 || pps.tileRows > 22)
            return false;
        pps.uniformTiles = reader.bit();
        const int columns = sps.columns();
        const int rows = sps.rows();
        if (pps.uniformTiles) {
            for (int i = 0; i < pps.tileColumns; i++)
                pps.columnWidths[i] = (i + 1) * columns / pps.tileColumns - i * columns / pps.tileColumns;
            for (int i = 0; i < pps.tileRows; i++)
                pps.rowHeights[i] = (i + 1) * rows / pps.tileRows - i * rows / pps.tileRows;
        } else {
            int used = 0;
            for (int i = 0; i < pps.tileColumns - 1; i++) {
                pps.columnWidths[i] = reader.ue() + 1;
                used += pps.columnWidths[i];
            }
            pps.columnWidths[pps.tileColumns - 1] = columns - used;
            used = 0;
            for (int i = 0; i < pps.tileRows - 1; i++) {
                pps.rowHeights[i] = reader.ue() + 1;
                used += pps.rowHeights[i];
            }
            pps.rowHeights[pps.tileRows - 1] = rows - used;
        }
        pps.loopFilterAcrossTiles = reader.bit();
    }
    pps.loopFilterAcrossSlices = reader.bit();
    pps.deblockingControl = reader.bit();
    if (pps.deblockingControl) {
        pps.deblockingOverride = reader.bit();
        pps.deblockingDisabled = reader.bit();
        if (!pps.deblockingDisabled) {
            pps.betaOffset = reader.se();
            pps.tcOffset = reader.se();
        }
    }
    pps.scalingLists = sps.scalingLists;
    pps.hasScalingLists = reader.bit();
    if (pps.hasScalingLists && !readScalingLists(reader, pps.scalingLists))
        return false;
    pps.listsModification = reader.bit();
    pps.parallelMergeLevel = reader.ue() + 2;
    pps.headerExtension = reader.bit();
    return !reader.failed();
}

namespace {

class HevcDecoder : public Decoder
{
public:
    bool configure(const uint8_t* configuration, size_t size);
    bool decode(const uint8_t* data, size_t size, std::shared_ptr<Surface>& surface) override;

private:
    bool parseParameters(const uint8_t* data, size_t size);
    bool parseSliceHeader(const Rbsp& rbsp, Slice& slice, const SliceHeader* previous) const;
    bool decodePicture(const std::vector<Slice>& slices, std::shared_ptr<Surface>& surface);

    int m_lengthSize = 4;
    std::map<int, Sps> m_sps;
    std::map<int, Rbsp> m_ppsData;
    std::map<int, Pps> m_pps;
    std::shared_ptr<VaDisplay> m_display;
    std::unique_ptr<Context> m_context;
    std::vector<Reference> m_references;
    int m_previousPocLsb = 0;
    int m_previousPocMsb = 0;
    bool m_endOfSequence = true;
    // Whether the pictures that lead the last random access point refer to something that is not there.
    bool m_noRaslOutput = false;
};

bool HevcDecoder::parseParameters(const uint8_t* data, size_t size)
{
    if (size < 3)
        return false;
    const uint8_t type = data[0] >> 1 & 63;
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

bool HevcDecoder::configure(const uint8_t* configuration, size_t size)
{
    if (size < 23 || configuration[0] != 1)
        return false;
    m_lengthSize = (configuration[21] & 3) + 1;
    const int arrays = configuration[22];
    size_t offset = 23;
    for (int i = 0; i < arrays; i++) {
        if (offset + 3 > size)
            return false;
        const int count = configuration[offset + 1] << 8 | configuration[offset + 2];
        offset += 3;
        for (int j = 0; j < count; j++) {
            if (offset + 2 > size)
                return false;
            const size_t length = configuration[offset] << 8 | configuration[offset + 1];
            offset += 2;
            if (length < 2 || offset + length > size)
                return false;
            const int type = configuration[offset] >> 1 & 63;
            if ((type == SpsUnit || type == PpsUnit) && !parseParameters(configuration + offset, length))
                return false;
            offset += length;
        }
    }
    if (m_sps.empty() || m_pps.empty())
        return false;
    const Sps& sps = m_sps.begin()->second;
    if (sps.chromaFormat != 1 || sps.lumaDepth != sps.chromaDepth)
        return false;
    VAProfile profile;
    if (sps.lumaDepth == 8 && sps.profile >= 1 && sps.profile <= 3)
        profile = VAProfileHEVCMain;
    else if (sps.lumaDepth == 10 && sps.profile == 2)
        profile = VAProfileHEVCMain10;
    else
        return false;
    m_display = VaDisplay::open();
    if (!m_display || !m_display->supports(profile))
        return false;
    codedWidth = sps.width;
    codedHeight = sps.height;
    if (sps.cropLeft + sps.cropRight < codedWidth && sps.cropTop + sps.cropBottom < codedHeight) {
        width = codedWidth - sps.cropLeft - sps.cropRight;
        height = codedHeight - sps.cropTop - sps.cropBottom;
    }
    depth = sps.lumaDepth;
    m_context = std::make_unique<Context>(m_display, profile, codedWidth, codedHeight, depth);
    return m_context->isValid();
}

// For a dependent slice, previous is the one it continues.
bool HevcDecoder::parseSliceHeader(const Rbsp& rbsp, Slice& slice, const SliceHeader* previous) const
{
    BitReader reader(rbsp);
    reader.skip(16);
    SliceHeader& header = slice.header;
    header.first = reader.bit();
    if (isIrap(slice.unitType))
        reader.bit();
    header.pps = reader.ue();
    const auto found = m_pps.find(header.pps);
    if (found == m_pps.end())
        return false;
    const Pps& pps = found->second;
    const Sps& sps = m_sps.at(pps.sps);
    if (!header.first) {
        if (pps.dependentSlices)
            header.dependent = reader.bit();
        header.address = reader.bits(ceilLog2(sps.columns() * sps.rows()));
    }
    if (header.dependent) {
        if (!previous)
            return false;
        const int address = header.address;
        header = *previous;
        header.first = false;
        header.dependent = true;
        header.address = address;
    } else {
        reader.skip(pps.extraHeaderBits);
        header.type = reader.ue();
        if (header.type > I)
            return false;
        if (pps.outputFlag)
            header.output = reader.bit();
        if (sps.separatePlanes)
            header.colourPlane = reader.bits(2);
        if (!isIdr(slice.unitType)) {
            header.pocLsb = reader.bits(sps.log2MaxPocLsb);
            header.shortTermFromSps = reader.bit();
            if (!header.shortTermFromSps) {
                const size_t start = reader.position();
                if (!parseShortTermSet(reader, sps, sps.shortTermSetCount, header.shortTerm))
                    return false;
                header.shortTermBits = reader.position() - start;
            } else {
                if (sps.shortTermSetCount == 0)
                    return false;
                if (sps.shortTermSetCount > 1)
                    header.shortTermIndex = reader.bits(ceilLog2(sps.shortTermSetCount));
                if (header.shortTermIndex >= sps.shortTermSetCount)
                    return false;
                header.shortTerm = sps.shortTermSets[header.shortTermIndex];
            }
            if (sps.longTermReferences) {
                if (sps.longTermCount > 0)
                    header.longTermSpsCount = reader.ue();
                header.longTermCount = reader.ue();
                if (header.longTermSpsCount > sps.longTermCount || header.longTermSpsCount + header.longTermCount > 32)
                    return false;
                for (int i = 0; i < header.longTermSpsCount + header.longTermCount; i++) {
                    if (i < header.longTermSpsCount) {
                        int index = 0;
                        if (sps.longTermCount > 1)
                            index = reader.bits(ceilLog2(sps.longTermCount));
                        if (index >= sps.longTermCount)
                            return false;
                        header.longTermPocLsb[i] = sps.longTermPocLsb[index];
                        header.longTermUsed[i] = sps.longTermUsed[index];
                    } else {
                        header.longTermPocLsb[i] = reader.bits(sps.log2MaxPocLsb);
                        header.longTermUsed[i] = reader.bit();
                    }
                    header.longTermHasMsb[i] = reader.bit();
                    int delta = header.longTermHasMsb[i] ? reader.ue() : 0;
                    // Those of the SPS and the others are counted on their own.
                    if (i != 0 && i != header.longTermSpsCount)
                        delta += header.longTermMsb[i - 1];
                    header.longTermMsb[i] = delta;
                }
            }
            if (sps.temporalMvp)
                header.temporalMvp = reader.bit();
        }
        if (sps.sao) {
            header.saoLuma = reader.bit();
            if (sps.hasChroma())
                header.saoChroma = reader.bit();
        }
        header.referenceCounts[0] = 0;
        header.referenceCounts[1] = 0;
        if (header.type == P || header.type == B) {
            header.referenceCounts[0] = pps.referenceCounts[0];
            if (header.type == B)
                header.referenceCounts[1] = pps.referenceCounts[1];
            if (reader.bit()) {
                header.referenceCounts[0] = reader.ue() + 1;
                if (header.type == B)
                    header.referenceCounts[1] = reader.ue() + 1;
            }
            if (header.referenceCounts[0] > 15 || header.referenceCounts[1] > 15)
                return false;

            int total = 0;
            for (int i = 0; i < header.shortTerm.negativeCount; i++)
                total += header.shortTerm.negativeUsed[i];
            for (int i = 0; i < header.shortTerm.positiveCount; i++)
                total += header.shortTerm.positiveUsed[i];
            for (int i = 0; i < header.longTermSpsCount + header.longTermCount; i++)
                total += header.longTermUsed[i];
            if (pps.listsModification && total > 1) {
                const int bits = ceilLog2(total);
                for (int list = 0; list < (header.type == B ? 2 : 1); list++) {
                    header.modified[list] = reader.bit();
                    if (!header.modified[list])
                        continue;
                    for (int i = 0; i < header.referenceCounts[list]; i++)
                        header.entries[list][i] = reader.bits(bits);
                }
            }
            if (header.type == B)
                header.mvdL1Zero = reader.bit();
            if (pps.cabacInit)
                header.cabacInit = reader.bit();
            if (header.temporalMvp) {
                header.collocatedFromL0 = true;
                if (header.type == B)
                    header.collocatedFromL0 = reader.bit();
                if ((header.collocatedFromL0 ? header.referenceCounts[0] : header.referenceCounts[1]) > 1)
                    header.collocatedIndex = reader.ue();
            }
            if ((pps.weightedPrediction && header.type == P) || (pps.weightedBiprediction && header.type == B)) {
                header.lumaLog2Denominator = reader.ue();
                const bool chroma = sps.hasChroma();
                if (chroma)
                    header.chromaLog2Denominator = header.lumaLog2Denominator + reader.se();
                const int shift = header.chromaLog2Denominator;
                for (int list = 0; list < (header.type == B ? 2 : 1); list++) {
                    bool lumaWeighted[15] = {};
                    bool chromaWeighted[15] = {};
                    const int count = header.referenceCounts[list];
                    for (int i = 0; i < count; i++)
                        lumaWeighted[i] = reader.bit();
                    for (int i = 0; i < count && chroma; i++)
                        chromaWeighted[i] = reader.bit();
                    for (int i = 0; i < count; i++) {
                        if (lumaWeighted[i]) {
                            header.lumaWeight[list][i] = reader.se();
                            header.lumaOffset[list][i] = reader.se();
                        }
                        if (!chromaWeighted[i])
                            continue;
                        for (int j = 0; j < 2; j++) {
                            const int weight = reader.se();
                            const int offset = reader.se();
                            header.chromaWeight[list][i][j] = weight;
                            header.chromaOffset[list][i][j]
                                = std::clamp(128 + offset - ((128 * ((1 << shift) + weight)) >> shift), -128, 127);
                        }
                    }
                }
            }
            header.mergeCandidates = reader.ue();
        }
        header.qpDelta = reader.se();
        if (pps.sliceChromaQpOffsets) {
            header.cbQpOffset = reader.se();
            header.crQpOffset = reader.se();
        }
        const bool deblockingOverride = pps.deblockingOverride && reader.bit();
        header.deblockingDisabled = pps.deblockingDisabled;
        header.betaOffset = pps.betaOffset;
        header.tcOffset = pps.tcOffset;
        if (deblockingOverride) {
            header.deblockingDisabled = reader.bit();
            if (!header.deblockingDisabled) {
                header.betaOffset = reader.se();
                header.tcOffset = reader.se();
            }
        }
        header.loopFilterAcross = pps.loopFilterAcrossSlices;
        if (pps.loopFilterAcrossSlices && (header.saoLuma || header.saoChroma || !header.deblockingDisabled))
            header.loopFilterAcross = reader.bit();
    }
    header.entryPoints = 0;
    if (pps.tiles || pps.entropySync) {
        header.entryPoints = reader.ue();
        // The most rows of blocks or tiles of any level.
        if (header.entryPoints > 4400)
            return false;
        if (header.entryPoints > 0) {
            const int length = reader.ue() + 1;
            if (length > 32)
                return false;
            for (int i = 0; i < header.entryPoints; i++)
                reader.bits(length);
        }
    }
    if (pps.headerExtension) {
        const size_t length = reader.ue();
        reader.skip(length * 8);
    }
    if (!reader.bit())
        return false;
    while (reader.position() % 8 != 0)
        reader.bits(1);
    if (reader.failed())
        return false;
    slice.headerBytes = reader.position() / 8;
    slice.headerEscapes = rbsp.removedBefore(slice.headerBytes);
    return true;
}

static bool contains(const std::vector<Reference*>& references, const Reference* reference)
{
    return std::find(references.begin(), references.end(), reference) != references.end();
}

// The places of a block in the order of its scaling list.
static void diagonalScan(uint8_t* scan, int size)
{
    int count = 0;
    for (int sum = 0; count < size * size; sum++) {
        for (int y = sum; y >= 0; y--) {
            const int x = sum - y;
            if (x < size && y < size)
                scan[count++] = y * size + x;
        }
    }
}

bool HevcDecoder::decodePicture(const std::vector<Slice>& slices, std::shared_ptr<Surface>& surface)
{
    const Slice& first = slices.front();
    const SliceHeader& header = first.header;
    if (!header.first)
        return false;
    const Pps& pps = m_pps.at(header.pps);
    const Sps& sps = m_sps.at(pps.sps);
    if (sps.width != codedWidth || sps.height != codedHeight || sps.lumaDepth != depth || sps.chromaFormat != 1)
        return false;
    const bool irap = isIrap(first.unitType);
    const bool idr = isIdr(first.unitType);
    if (irap) {
        m_noRaslOutput = idr || isBla(first.unitType) || m_endOfSequence;
        m_endOfSequence = false;
    } else if (m_endOfSequence) {
        return true;
    } else if (m_noRaslOutput && (first.unitType == RaslN || first.unitType == RaslR)) {
        return true;
    }

    const int maxPocLsb = 1 << sps.log2MaxPocLsb;
    int pocMsb = 0;
    if (!irap || !m_noRaslOutput) {
        pocMsb = m_previousPocMsb;
        if (header.pocLsb < m_previousPocLsb && m_previousPocLsb - header.pocLsb >= maxPocLsb / 2)
            pocMsb += maxPocLsb;
        else if (header.pocLsb > m_previousPocLsb && header.pocLsb - m_previousPocLsb > maxPocLsb / 2)
            pocMsb -= maxPocLsb;
    }
    const int poc = pocMsb + header.pocLsb;
    const bool subLayerNonReference = first.unitType <= LastSubLayerUnit && first.unitType % 2 == 0;
    const bool leading = first.unitType >= RadlN && first.unitType <= RaslR;
    if (first.temporalId == 0 && !subLayerNonReference && !leading) {
        m_previousPocLsb = header.pocLsb;
        m_previousPocMsb = pocMsb;
    }
    if (irap && m_noRaslOutput) {
        for (Reference& reference : m_references)
            reference.shortTerm = reference.longTerm = false;
    }

    std::vector<Reference*> before;
    std::vector<Reference*> after;
    std::vector<Reference*> shortTermLater;
    std::vector<Reference*> longTerm;
    std::vector<Reference*> longTermLater;
    if (!idr) {
        const int longTermTotal = header.longTermSpsCount + header.longTermCount;
        for (int i = 0; i < longTermTotal; i++) {
            const bool hasMsb = header.longTermHasMsb[i];
            int wanted = header.longTermPocLsb[i];
            if (hasMsb)
                wanted += poc - (header.longTermMsb[i] << sps.log2MaxPocLsb) - (poc & (maxPocLsb - 1));
            Reference* found = nullptr;
            for (Reference& reference : m_references) {
                if (reference.isUsed() && (hasMsb ? reference.poc : reference.poc & (maxPocLsb - 1)) == wanted)
                    found = &reference;
            }
            if (found) {
                found->shortTerm = false;
                found->longTerm = true;
            }
            (header.longTermUsed[i] ? longTerm : longTermLater).push_back(found);
        }
        const auto findShortTerm = [this](int poc) -> Reference* {
            for (Reference& reference : m_references) {
                if (reference.shortTerm && reference.poc == poc)
                    return &reference;
            }
            return nullptr;
        };
        const ShortTermSet& set = header.shortTerm;
        for (int i = 0; i < set.negativeCount; i++)
            (set.negativeUsed[i] ? before : shortTermLater).push_back(findShortTerm(poc + set.negativeDelta[i]));
        for (int i = 0; i < set.positiveCount; i++)
            (set.positiveUsed[i] ? after : shortTermLater).push_back(findShortTerm(poc + set.positiveDelta[i]));
    }
    for (Reference& reference : m_references) {
        const bool isLongTerm = contains(longTerm, &reference) || contains(longTermLater, &reference);
        reference.shortTerm = !isLongTerm
            && (contains(before, &reference) || contains(after, &reference) || contains(shortTermLater, &reference));
        reference.longTerm = isLongTerm;
    }

    std::shared_ptr<Surface> target = m_context->takeSurface();
    if (!target)
        return false;
    VAPictureParameterBufferHEVC picture = {};
    picture.CurrPic.picture_id = target->id();
    picture.CurrPic.pic_order_cnt = poc;
    std::vector<const Reference*> used;
    for (const Reference& reference : m_references) {
        if (reference.isUsed() && reference.surface && used.size() < 15)
            used.push_back(&reference);
    }
    for (size_t i = 0; i < 15; i++) {
        VAPictureHEVC& entry = picture.ReferenceFrames[i];
        if (i >= used.size()) {
            entry.picture_id = VA_INVALID_SURFACE;
            entry.flags = VA_PICTURE_HEVC_INVALID;
            continue;
        }
        const Reference* reference = used[i];
        entry.picture_id = reference->surface->id();
        entry.pic_order_cnt = reference->poc;
        entry.flags = reference->longTerm ? VA_PICTURE_HEVC_LONG_TERM_REFERENCE : 0;
        if (contains(before, reference))
            entry.flags |= VA_PICTURE_HEVC_RPS_ST_CURR_BEFORE;
        else if (contains(after, reference))
            entry.flags |= VA_PICTURE_HEVC_RPS_ST_CURR_AFTER;
        else if (contains(longTerm, reference))
            entry.flags |= VA_PICTURE_HEVC_RPS_LT_CURR;
    }
    picture.pic_width_in_luma_samples = sps.width;
    picture.pic_height_in_luma_samples = sps.height;
    picture.pic_fields.bits.chroma_format_idc = sps.chromaFormat;
    picture.pic_fields.bits.separate_colour_plane_flag = sps.separatePlanes;
    picture.pic_fields.bits.pcm_enabled_flag = sps.pcm;
    picture.pic_fields.bits.scaling_list_enabled_flag = sps.hasScalingLists;
    picture.pic_fields.bits.transform_skip_enabled_flag = pps.transformSkip;
    picture.pic_fields.bits.amp_enabled_flag = sps.amp;
    picture.pic_fields.bits.strong_intra_smoothing_enabled_flag = sps.strongIntraSmoothing;
    picture.pic_fields.bits.sign_data_hiding_enabled_flag = pps.signHiding;
    picture.pic_fields.bits.constrained_intra_pred_flag = pps.constrainedIntra;
    picture.pic_fields.bits.cu_qp_delta_enabled_flag = pps.cuQpDelta;
    picture.pic_fields.bits.weighted_pred_flag = pps.weightedPrediction;
    picture.pic_fields.bits.weighted_bipred_flag = pps.weightedBiprediction;
    picture.pic_fields.bits.transquant_bypass_enabled_flag = pps.transquantBypass;
    picture.pic_fields.bits.tiles_enabled_flag = pps.tiles;
    picture.pic_fields.bits.entropy_coding_sync_enabled_flag = pps.entropySync;
    picture.pic_fields.bits.pps_loop_filter_across_slices_enabled_flag = pps.loopFilterAcrossSlices;
    picture.pic_fields.bits.loop_filter_across_tiles_enabled_flag = pps.loopFilterAcrossTiles;
    picture.pic_fields.bits.pcm_loop_filter_disabled_flag = sps.pcmLoopFilterDisabled;
    picture.pic_fields.bits.NoPicReorderingFlag = sps.maxReorder == 0;
    picture.pic_fields.bits.NoBiPredFlag = 0;
    picture.sps_max_dec_pic_buffering_minus1 = sps.maxDecPicBuffering - 1;
    picture.bit_depth_luma_minus8 = sps.lumaDepth - 8;
    picture.bit_depth_chroma_minus8 = sps.chromaDepth - 8;
    picture.pcm_sample_bit_depth_luma_minus1 = sps.pcmLumaDepth - 1;
    picture.pcm_sample_bit_depth_chroma_minus1 = sps.pcmChromaDepth - 1;
    picture.log2_min_luma_coding_block_size_minus3 = sps.log2MinCodingBlock - 3;
    picture.log2_diff_max_min_luma_coding_block_size = sps.log2CodingBlockRange;
    picture.log2_min_transform_block_size_minus2 = sps.log2MinTransformBlock - 2;
    picture.log2_diff_max_min_transform_block_size = sps.log2TransformBlockRange;
    picture.log2_min_pcm_luma_coding_block_size_minus3 = sps.log2MinPcm - 3;
    picture.log2_diff_max_min_pcm_luma_coding_block_size = sps.log2PcmRange;
    picture.max_transform_hierarchy_depth_intra = sps.transformDepthIntra;
    picture.max_transform_hierarchy_depth_inter = sps.transformDepthInter;
    picture.init_qp_minus26 = pps.qp - 26;
    picture.diff_cu_qp_delta_depth = pps.cuQpDeltaDepth;
    picture.pps_cb_qp_offset = pps.cbQpOffset;
    picture.pps_cr_qp_offset = pps.crQpOffset;
    picture.log2_parallel_merge_level_minus2 = pps.parallelMergeLevel - 2;
    picture.num_tile_columns_minus1 = pps.tileColumns - 1;
    picture.num_tile_rows_minus1 = pps.tileRows - 1;
    for (int i = 0; i < pps.tileColumns && i < 19; i++)
        picture.column_width_minus1[i] = pps.columnWidths[i] - 1;
    for (int i = 0; i < pps.tileRows && i < 21; i++)
        picture.row_height_minus1[i] = pps.rowHeights[i] - 1;
    picture.slice_parsing_fields.bits.lists_modification_present_flag = pps.listsModification;
    picture.slice_parsing_fields.bits.long_term_ref_pics_present_flag = sps.longTermReferences;
    picture.slice_parsing_fields.bits.sps_temporal_mvp_enabled_flag = sps.temporalMvp;
    picture.slice_parsing_fields.bits.cabac_init_present_flag = pps.cabacInit;
    picture.slice_parsing_fields.bits.output_flag_present_flag = pps.outputFlag;
    picture.slice_parsing_fields.bits.dependent_slice_segments_enabled_flag = pps.dependentSlices;
    picture.slice_parsing_fields.bits.pps_slice_chroma_qp_offsets_present_flag = pps.sliceChromaQpOffsets;
    picture.slice_parsing_fields.bits.sample_adaptive_offset_enabled_flag = sps.sao;
    picture.slice_parsing_fields.bits.deblocking_filter_override_enabled_flag = pps.deblockingOverride;
    picture.slice_parsing_fields.bits.pps_disable_deblocking_filter_flag = pps.deblockingDisabled;
    picture.slice_parsing_fields.bits.slice_segment_header_extension_present_flag = pps.headerExtension;
    picture.slice_parsing_fields.bits.RapPicFlag = irap;
    picture.slice_parsing_fields.bits.IdrPicFlag = idr;
    picture.slice_parsing_fields.bits.IntraPicFlag = irap;
    picture.log2_max_pic_order_cnt_lsb_minus4 = sps.log2MaxPocLsb - 4;
    picture.num_short_term_ref_pic_sets = sps.shortTermSetCount;
    picture.num_long_term_ref_pic_sps = sps.longTermCount;
    picture.num_ref_idx_l0_default_active_minus1 = pps.referenceCounts[0] - 1;
    picture.num_ref_idx_l1_default_active_minus1 = pps.referenceCounts[1] - 1;
    picture.pps_beta_offset_div2 = pps.betaOffset;
    picture.pps_tc_offset_div2 = pps.tcOffset;
    picture.num_extra_slice_header_bits = pps.extraHeaderBits;
    picture.st_rps_bits = header.shortTermBits;
    bool ok = m_context->addBuffer(VAPictureParameterBufferType, &picture, sizeof(picture));

    if (ok && sps.hasScalingLists) {
        VAIQMatrixBufferHEVC matrix = {};
        uint8_t scan4x4[16];
        uint8_t scan8x8[64];
        diagonalScan(scan4x4, 4);
        diagonalScan(scan8x8, 8);
        const ScalingLists& lists = pps.scalingLists;
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 16; j++)
                matrix.ScalingList4x4[i][scan4x4[j]] = lists.lists[0][i][j];
            for (int j = 0; j < 64; j++) {
                matrix.ScalingList8x8[i][scan8x8[j]] = lists.lists[1][i][j];
                matrix.ScalingList16x16[i][scan8x8[j]] = lists.lists[2][i][j];
            }
            matrix.ScalingListDC16x16[i] = lists.dc[2][i];
        }
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 64; j++)
                matrix.ScalingList32x32[i][scan8x8[j]] = lists.lists[3][i * 3][j];
            matrix.ScalingListDC32x32[i] = lists.dc[3][i * 3];
        }
        ok = m_context->addBuffer(VAIQMatrixBufferType, &matrix, sizeof(matrix));
    }

    const int total = int(before.size()) + int(after.size()) + int(longTerm.size());
    for (size_t i = 0; i < slices.size() && ok; i++) {
        const Slice& slice = slices[i];
        const SliceHeader& header = slice.header;
        VASliceParameterBufferHEVC parameters = {};
        parameters.slice_data_size = slice.size;
        parameters.slice_data_byte_offset = slice.headerBytes;
        parameters.slice_segment_address = header.address;
        memset(parameters.RefPicList, 0xff, sizeof(parameters.RefPicList));
        for (int list = 0; list < 2 && total > 0; list++) {
            const int count = header.referenceCounts[list];
            if (count == 0)
                continue;
            std::vector<Reference*> candidates;
            const int needed = std::max(count, total);
            while (int(candidates.size()) < needed) {
                for (Reference* reference : list == 0 ? before : after)
                    candidates.push_back(reference);
                for (Reference* reference : list == 0 ? after : before)
                    candidates.push_back(reference);
                for (Reference* reference : longTerm)
                    candidates.push_back(reference);
            }
            for (int j = 0; j < count; j++) {
                const int entry = header.modified[list] ? header.entries[list][j] : j;
                if (entry >= int(candidates.size()))
                    return false;
                const Reference* reference = candidates[entry];
                const auto found = std::find(used.begin(), used.end(), reference);
                parameters.RefPicList[list][j] = reference && found != used.end() ? found - used.begin() : 0xff;
            }
        }
        parameters.LongSliceFlags.fields.LastSliceOfPic = i + 1 == slices.size();
        parameters.LongSliceFlags.fields.dependent_slice_segment_flag = header.dependent;
        parameters.LongSliceFlags.fields.slice_type = header.type;
        parameters.LongSliceFlags.fields.color_plane_id = header.colourPlane;
        parameters.LongSliceFlags.fields.slice_sao_luma_flag = header.saoLuma;
        parameters.LongSliceFlags.fields.slice_sao_chroma_flag = header.saoChroma;
        parameters.LongSliceFlags.fields.mvd_l1_zero_flag = header.mvdL1Zero;
        parameters.LongSliceFlags.fields.cabac_init_flag = header.cabacInit;
        parameters.LongSliceFlags.fields.slice_temporal_mvp_enabled_flag = header.temporalMvp;
        parameters.LongSliceFlags.fields.slice_deblocking_filter_disabled_flag = header.deblockingDisabled;
        parameters.LongSliceFlags.fields.collocated_from_l0_flag = header.collocatedFromL0;
        parameters.LongSliceFlags.fields.slice_loop_filter_across_slices_enabled_flag = header.loopFilterAcross;
        parameters.collocated_ref_idx = header.temporalMvp ? header.collocatedIndex : 0xff;
        parameters.num_ref_idx_l0_active_minus1 = std::max(header.referenceCounts[0] - 1, 0);
        parameters.num_ref_idx_l1_active_minus1 = std::max(header.referenceCounts[1] - 1, 0);
        parameters.slice_qp_delta = header.qpDelta;
        parameters.slice_cb_qp_offset = header.cbQpOffset;
        parameters.slice_cr_qp_offset = header.crQpOffset;
        parameters.slice_beta_offset_div2 = header.betaOffset;
        parameters.slice_tc_offset_div2 = header.tcOffset;
        parameters.luma_log2_weight_denom = header.lumaLog2Denominator;
        parameters.delta_chroma_log2_weight_denom = header.chromaLog2Denominator - header.lumaLog2Denominator;
        memcpy(parameters.delta_luma_weight_l0, header.lumaWeight[0], sizeof(parameters.delta_luma_weight_l0));
        memcpy(parameters.luma_offset_l0, header.lumaOffset[0], sizeof(parameters.luma_offset_l0));
        memcpy(parameters.delta_chroma_weight_l0, header.chromaWeight[0], sizeof(parameters.delta_chroma_weight_l0));
        memcpy(parameters.ChromaOffsetL0, header.chromaOffset[0], sizeof(parameters.ChromaOffsetL0));
        memcpy(parameters.delta_luma_weight_l1, header.lumaWeight[1], sizeof(parameters.delta_luma_weight_l1));
        memcpy(parameters.luma_offset_l1, header.lumaOffset[1], sizeof(parameters.luma_offset_l1));
        memcpy(parameters.delta_chroma_weight_l1, header.chromaWeight[1], sizeof(parameters.delta_chroma_weight_l1));
        memcpy(parameters.ChromaOffsetL1, header.chromaOffset[1], sizeof(parameters.ChromaOffsetL1));
        parameters.five_minus_max_num_merge_cand = header.mergeCandidates;
        parameters.num_entry_point_offsets = header.entryPoints;
        parameters.entry_offset_to_subset_array = 0;
        parameters.slice_data_num_emu_prevn_bytes = slice.headerEscapes;
        ok = m_context->addBuffer(VASliceParameterBufferType, &parameters, sizeof(parameters))
            && m_context->addBuffer(VASliceDataBufferType, slice.data, slice.size);
    }
    if (!ok) {
        m_context->discard();
        return false;
    }
    if (!m_context->render(target->id()))
        return false;

    m_references.erase(std::remove_if(m_references.begin(), m_references.end(),
                                      [](const Reference& reference) { return !reference.isUsed(); }),
                       m_references.end());
    surface = target;
    m_references.push_back({std::move(target), poc, true, false});
    while (int(m_references.size()) > std::max(sps.maxDecPicBuffering, 1))
        m_references.erase(m_references.begin());
    return true;
}

bool HevcDecoder::decode(const uint8_t* data, size_t size, std::shared_ptr<Surface>& surface)
{
    surface.reset();
    SliceHeader independent;
    bool hasIndependent = false;
    std::vector<Slice> slices;
    const bool complete = forEachUnit(data, size, m_lengthSize, [&](const uint8_t* unit, size_t length) {
        if (length < 3)
            return false;
        const int type = unit[0] >> 1 & 63;
        if (type == SpsUnit || type == PpsUnit)
            return parseParameters(unit, length);
        if (type == EndOfSequenceUnit) {
            m_endOfSequence = true;
            return true;
        }
        // Reserved ones, and what is not a slice.
        if (type > LastIrap || (type >= 10 && type <= 15))
            return true;
        Slice blank;
        blank.data = unit;
        blank.size = length;
        blank.unitType = type;
        blank.temporalId = (unit[1] & 7) - 1;
        // The header is in the beginning, the rest need not be copied.
        Slice slice = blank;
        const SliceHeader* previous = hasIndependent ? &independent : nullptr;
        if (length <= 1024 || !parseSliceHeader(unescape(unit, 1024), slice, previous)) {
            slice = blank;
            if (!parseSliceHeader(unescape(unit, length), slice, previous))
                return false;
        }
        if (!slice.header.dependent) {
            independent = slice.header;
            hasIndependent = true;
        }
        slices.push_back(slice);
        return true;
    });
    if (!complete)
        return false;
    return slices.empty() || decodePicture(slices, surface);
}

} // namespace

std::unique_ptr<Decoder> makeHevcDecoder(const uint8_t* configuration, size_t size)
{
    auto decoder = std::make_unique<HevcDecoder>();
    if (!decoder->configure(configuration, size))
        return nullptr;
    return decoder;
}

} // namespace cutefish::reef::video

// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Faces of fonts: fontconfig finds them, FreeType and HarfBuzz read them, and their glyphs are drawn from a cache of bitmaps.

#include "fonts.h"

#include "paths.h"

#include "ui/application.h"
#include "ui/bitmap.h"
#include "ui/theme.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <list>
#include <memory>
#include <mutex>
#include <tuple>
#include <unordered_map>
#include <utility>

#include <dlfcn.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fontconfig/fontconfig.h>

#include <fontconfig/fcfreetype.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MULTIPLE_MASTERS_H
#include FT_OUTLINE_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H
#include FT_TRUETYPE_TABLES_H

#include <hb-ft.h>
#include <hb-ot.h>
#include <hb-subset.h>
#include <hb.h>

namespace cutefish::reef {

namespace {

// What is asked of fontconfig.
struct MatchKey
{
    std::string name;
    std::string family;
    std::string style;
    double weight = 0;
    double width = 0;
    int italic = -1;
    int fixedPitch = -1;
    std::string language;

    bool operator<(const MatchKey& other) const
    {
        return std::tie(name, family, style, weight, width, italic, fixedPitch, language)
                < std::tie(other.name, other.family, other.style, other.weight, other.width, other.italic, other.fixedPitch,
                           other.language);
    }
};

// A face of a file, and what is made of it.
struct FaceKey
{
    std::string path;
    // The face in the lower half, the named instance above it.
    int index = 0;
    bool bold = false;
    bool oblique = false;
    std::vector<std::pair<uint32_t, double>> variations;
    double stretch = 1;
    // Instead of the file's own.
    std::string family;
    std::string style;

    bool operator<(const FaceKey& other) const
    {
        return std::tie(path, index, bold, oblique, variations, stretch, family, style)
                < std::tie(other.path, other.index, other.bold, other.oblique, other.variations, other.stretch, other.family,
                           other.style);
    }
};

struct Match
{
    // Without a path for no face.
    FaceKey face;
    int exact = 0;
};

// A character that a face lacks.
struct FallbackKey
{
    FaceKey face;
    uint32_t character = 0;
    std::string language;

    bool operator<(const FallbackKey& other) const
    {
        return std::tie(face, character, language) < std::tie(other.face, other.character, other.language);
    }
};

struct MaskKey
{
    const reef_ui_face* face;
    uint32_t glyph;
    // In 64ths of a pixel.
    int width;
    int height;
    // The quarter of a pixel it starts at, and whether it is aliased.
    int variant;

    bool operator<(const MaskKey& other) const
    {
        return std::tie(face, glyph, width, height, variant)
                < std::tie(other.face, other.glyph, other.width, other.height, other.variant);
    }
};

// The bitmap of a glyph.
struct Mask
{
    int left = 0;
    int top = 0;
    int width = 0;
    int height = 0;
    // Four bytes for each pixel, else one.
    bool color = false;
    // What it is drawn at, for a font with bitmaps of some sizes only.
    double scale = 1;
    std::vector<uint8_t> pixels;
};

struct PlacedGlyph
{
    const Mask* mask;
    int x;
    int y;
    double deviceX;
    double deviceY;
};

} // namespace

// Glyphs next to each other in a color, wherever they are.
struct RunKey
{
    std::vector<std::tuple<const Mask*, int, int>> glyphs;
    std::array<double, 4> color;

    bool operator==(const RunKey& other) const
    {
        return glyphs == other.glyphs && memcmp(color.data(), other.color.data(), sizeof color) == 0;
    }
};

struct RunHash
{
    size_t operator()(const RunKey& key) const
    {
        size_t hash = 0;
        const auto add = [&hash](uint64_t value) { hash ^= value + 0x9e3779b97f4a7c15 + (hash << 6) + (hash >> 2); };
        for (const auto& [mask, x, y] : key.glyphs) {
            add(reinterpret_cast<uint64_t>(mask));
            add(uint64_t(uint32_t(x)) << 32 | uint32_t(y));
        }
        for (double channel : key.color)
            add(std::bit_cast<uint64_t>(channel));
        return hash;
    }
};

struct Run
{
    ui::RetainedBitmap bitmap;
    std::list<const RunKey*>::iterator position;
};

} // namespace cutefish::reef

struct reef_ui_face
{
    std::atomic<int> references = 1;
    cutefish::reef::FaceKey key;
    // Of a font that is not in a file.
    std::shared_ptr<std::string> data;
    FT_Face ftFace = nullptr;
    hb_face_t* hbFace = nullptr;
    hb_font_t* font = nullptr;
    reef_ui_face_metrics metrics = {};
    std::vector<reef_ui_face_axis> axes;
    std::vector<std::string> axisNames;
    std::vector<uint32_t> ranges;
    bool hasRanges = false;
    // Null for characters that no font has.
    std::map<uint32_t, reef_ui_face*> fallbacks;
    std::map<std::pair<uint32_t, std::string>, reef_ui_face*> languageFallbacks;
};

namespace cutefish::reef {

static constexpr double obliqueSlant = 0.2;
static constexpr double boldStrength = 0.02;

static constexpr size_t mostMatches = 4096;
static constexpr size_t mostLists = 256;
static constexpr size_t mostFallbacks = 8192;
static constexpr size_t mostMaskBytes = 32 << 20;
static constexpr size_t mostRunBytes = 32 << 20;
static constexpr size_t largestRun = 2 << 20;

static std::recursive_mutex mutex;
static FT_Library library = nullptr;
static std::map<FaceKey, reef_ui_face*> faces;
static std::map<MaskKey, Mask> masks;
static size_t maskBytes = 0;
static std::vector<std::string> applicationFonts;
static std::map<MatchKey, Match> matches;
static std::map<std::pair<bool, MatchKey>, std::string> lists;
static std::unordered_map<RunKey, Run, RunHash> runs;
// The last drawn ones first.
static std::list<const RunKey*> runOrder;
static size_t runBytes = 0;
static std::map<FallbackKey, FaceKey> fallbacks;

// The file with the answers of fontconfig, which is slow to ask; -1 without one.
static int answers = -1;
static bool answersOpened = false;
static uint64_t answersFingerprint = 0;

static FT_Library freetype()
{
    if (!library)
        FT_Init_FreeType(&library);
    return library;
}

static int copyText(const std::string& text, char* buffer, int size)
{
    if (buffer && size > 0) {
        const size_t length = std::min(size_t(size - 1), text.size());
        memcpy(buffer, text.data(), length);
        buffer[length] = 0;
    }
    return int(text.size());
}

static bool equalIgnoringCase(const std::string& text, const char* other)
{
    if (!other || strlen(other) != text.size())
        return false;
    for (size_t i = 0; i < text.size(); i++) {
        if (tolower(uint8_t(text[i])) != tolower(uint8_t(other[i])))
            return false;
    }
    return true;
}

static std::string withoutSpaces(std::string text)
{
    text.erase(std::remove(text.begin(), text.end(), ' '), text.end());
    return text;
}

static void appendUtf8(std::string& text, uint32_t character)
{
    if (character < 0x80) {
        text += char(character);
    } else if (character < 0x800) {
        text += char(0xc0 | character >> 6);
        text += char(0x80 | (character & 0x3f));
    } else if (character < 0x10000) {
        text += char(0xe0 | character >> 12);
        text += char(0x80 | (character >> 6 & 0x3f));
        text += char(0x80 | (character & 0x3f));
    } else {
        text += char(0xf0 | character >> 18);
        text += char(0x80 | (character >> 12 & 0x3f));
        text += char(0x80 | (character >> 6 & 0x3f));
        text += char(0x80 | (character & 0x3f));
    }
}

namespace {

// The fontconfig of this moment, with the fonts of the program.
class Config
{
public:
    Config()
    {
        std::lock_guard lock(mutex);
        m_config = FcConfigReference(nullptr);
        if (!m_config || applicationFonts.empty())
            return;
        // A configuration that was read anew does not have them.
        const FcFontSet* fonts = FcConfigGetFonts(m_config, FcSetApplication);
        if (fonts && fonts->nfont)
            return;
        for (const std::string& file : applicationFonts)
            FcConfigAppFontAddFile(m_config, reinterpret_cast<const FcChar8*>(file.c_str()));
    }
    ~Config()
    {
        if (m_config)
            FcConfigDestroy(m_config);
    }
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    operator FcConfig*() const { return m_config; }

private:
    FcConfig* m_config = nullptr;
};

} // namespace

static std::string patternString(FcPattern* pattern, const char* object)
{
    FcChar8* value = nullptr;
    if (FcPatternGetString(pattern, object, 0, &value) == FcResultMatch && value)
        return reinterpret_cast<const char*>(value);
    return {};
}

// The English one where there are several.
static std::string localizedString(FcPattern* pattern, const char* object, const char* languageObject)
{
    FcChar8* value = nullptr;
    if (languageObject) {
        for (int i = 0; FcPatternGetString(pattern, object, i, &value) == FcResultMatch; i++) {
            FcChar8* language = nullptr;
            if (FcPatternGetString(pattern, languageObject, i, &language) == FcResultMatch && language
                && strcmp(reinterpret_cast<const char*>(language), "en") == 0)
                return reinterpret_cast<const char*>(value);
        }
    }
    if (FcPatternGetString(pattern, object, 0, &value) == FcResultMatch && value)
        return reinterpret_cast<const char*>(value);
    return {};
}

static const double fontconfigWidths[] = {50, 62.5, 75, 87.5, 100, 112.5, 125, 150, 200};

static int widthClass(double width)
{
    int best = 4;
    for (int i = 0; i < 9; i++) {
        if (std::abs(fontconfigWidths[i] - width) < std::abs(fontconfigWidths[best] - width))
            best = i;
    }
    return best + 1;
}

static const char* orEmpty(const char* text)
{
    return text ? text : "";
}

static MatchKey matchKey(const reef_ui_face_query* query)
{
    return {orEmpty(query->name),
            orEmpty(query->family),
            orEmpty(query->style),
            query->weight,
            query->width,
            query->italic,
            query->fixedPitch,
            orEmpty(query->language)};
}

static void addQuery(FcPattern* pattern, const reef_ui_face_query* query, bool traits)
{
    if (query->family && *query->family)
        FcPatternAddString(pattern, FC_FAMILY, reinterpret_cast<const FcChar8*>(query->family));
    if (query->style && *query->style)
        FcPatternAddString(pattern, FC_STYLE, reinterpret_cast<const FcChar8*>(query->style));
    if (!traits)
        return;
    if (query->weight > 0)
        FcPatternAddDouble(pattern, FC_WEIGHT, FcWeightFromOpenTypeDouble(query->weight));
    if (query->width > 0)
        FcPatternAddDouble(pattern, FC_WIDTH, fontconfigWidths[std::clamp(int(std::lround(query->width)), 1, 9) - 1]);
    if (query->italic >= 0)
        FcPatternAddInteger(pattern, FC_SLANT, query->italic ? FC_SLANT_ITALIC : FC_SLANT_ROMAN);
    if (query->fixedPitch == 1)
        FcPatternAddInteger(pattern, FC_SPACING, FC_MONO);
    if (query->language && *query->language)
        FcPatternAddString(pattern, FC_LANG, reinterpret_cast<const FcChar8*>(query->language));
}

static void hashBytes(uint64_t& hash, const void* data, size_t size)
{
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < size; i++)
        hash = (hash ^ bytes[i]) * 0x100000001b3;
}

static void hashFile(uint64_t& hash, const char* path)
{
    hashBytes(hash, path, strlen(path) + 1);
    struct stat info = {};
    if (stat(path, &info) == 0) {
        const int64_t stamp[] = {info.st_size, info.st_mtim.tv_sec, info.st_mtim.tv_nsec};
        hashBytes(hash, stamp, sizeof stamp);
    }
}

// Another one for another host, another configuration, other languages and other fonts.
static uint64_t fingerprint(FcConfig* config)
{
    uint64_t hash = 0xcbf29ce484222325;
    Dl_info info = {};
    if (dladdr(reinterpret_cast<void*>(&fingerprint), &info) && info.dli_fname)
        hashFile(hash, info.dli_fname);
    if (FcStrList* files = FcConfigGetConfigFiles(config)) {
        while (const FcChar8* file = FcStrListNext(files))
            hashFile(hash, reinterpret_cast<const char*>(file));
        FcStrListDone(files);
    }
    if (FcStrSet* languages = FcGetDefaultLangs()) {
        FcStrList* list = FcStrListCreate(languages);
        while (const FcChar8* language = FcStrListNext(list))
            hashBytes(hash, language, strlen(reinterpret_cast<const char*>(language)) + 1);
        FcStrListDone(list);
        FcStrSetDestroy(languages);
    }
    const FcFontSet* fonts = FcConfigGetFonts(config, FcSetSystem);
    for (int i = 0; fonts && i < fonts->nfont; i++) {
        FcChar8* file = nullptr;
        int index = 0;
        if (FcPatternGetString(fonts->fonts[i], FC_FILE, 0, &file) == FcResultMatch && file)
            hashBytes(hash, file, strlen(reinterpret_cast<const char*>(file)) + 1);
        FcPatternGetInteger(fonts->fonts[i], FC_INDEX, 0, &index);
        hashBytes(hash, &index, sizeof index);
    }
    return hash;
}

// A record of the file has its size, the fingerprint, the kind and the question with its answer.
enum AnswerKind : uint8_t {
    MatchAnswer = 1,
    ListAnswer = 2,
    FallbackAnswer = 3,
};

namespace {

struct AnswerWriter
{
    std::string bytes;

    void number(uint64_t value) { bytes.append(reinterpret_cast<const char*>(&value), sizeof value); }
    void real(double value) { number(std::bit_cast<uint64_t>(value)); }
    void flag(bool value) { bytes.push_back(value); }

    void text(const std::string& text)
    {
        number(text.size());
        bytes += text;
    }

    void matchKey(const MatchKey& key)
    {
        text(key.name);
        text(key.family);
        text(key.style);
        real(key.weight);
        real(key.width);
        number(uint64_t(key.italic));
        number(uint64_t(key.fixedPitch));
        text(key.language);
    }

    void faceKey(const FaceKey& key)
    {
        text(key.path);
        number(uint64_t(key.index));
        flag(key.bold);
        flag(key.oblique);
        number(key.variations.size());
        for (const auto& [tag, value] : key.variations) {
            number(tag);
            real(value);
        }
        real(key.stretch);
        text(key.family);
        text(key.style);
    }
};

struct AnswerReader
{
    const char* data;
    size_t size;
    size_t position = 0;
    bool good = true;

    template<typename Value>
    Value value()
    {
        if (!good || size - position < sizeof(Value)) {
            good = false;
            return {};
        }
        Value value;
        memcpy(&value, data + position, sizeof value);
        position += sizeof value;
        return value;
    }
    uint64_t number() { return value<uint64_t>(); }
    double real() { return std::bit_cast<double>(number()); }
    bool flag() { return value<uint8_t>() != 0; }

    std::string text()
    {
        const uint64_t length = number();
        if (!good || size - position < length) {
            good = false;
            return {};
        }
        std::string text(data + position, length);
        position += length;
        return text;
    }

    MatchKey matchKey()
    {
        MatchKey key;
        key.name = text();
        key.family = text();
        key.style = text();
        key.weight = real();
        key.width = real();
        key.italic = int(number());
        key.fixedPitch = int(number());
        key.language = text();
        return key;
    }

    FaceKey faceKey()
    {
        FaceKey key;
        key.path = text();
        key.index = int(number());
        key.bold = flag();
        key.oblique = flag();
        const uint64_t count = number();
        for (uint64_t i = 0; i < count && good; i++) {
            const uint32_t tag = uint32_t(number());
            key.variations.emplace_back(tag, real());
        }
        key.stretch = real();
        key.family = text();
        key.style = text();
        return key;
    }
};

} // namespace

static std::string homeFolder()
{
    const char* home = secure_getenv("CFFIXED_USER_HOME");
    if (home && *home == '/')
        return home;
    const passwd* user = getpwuid(getuid());
    return user && user->pw_dir && *user->pw_dir ? user->pw_dir : "";
}

static std::string answersPath()
{
    const std::string home = homeFolder();
    return home.empty() ? std::string() : home + "/.cache/cutefish-reef/font-answers";
}

// Reads the answers of earlier programs the first time; false when none are to be kept.
static bool openAnswers()
{
    if (!applicationFonts.empty())
        return false;
    if (answersOpened)
        return answers >= 0;
    answersOpened = true;
    const std::string path = answersPath();
    if (path.empty())
        return false;
    answersFingerprint = fingerprint(Config());
    std::string contents;
    if (FILE* file = fopen(path.c_str(), "rb")) {
        char buffer[65536];
        while (const size_t size = fread(buffer, 1, sizeof buffer, file))
            contents.append(buffer, size);
        fclose(file);
    }
    std::vector<std::string> records;
    size_t others = 0;
    size_t position = 0;
    while (contents.size() - position >= sizeof(uint64_t)) {
        uint64_t size;
        memcpy(&size, contents.data() + position, sizeof size);
        if (size < 9 || size > contents.size() - position - sizeof size)
            break;
        const size_t start = position;
        position += sizeof size + size;
        AnswerReader reader = {contents.data() + start + sizeof size, size};
        if (reader.number() != answersFingerprint) {
            others++;
            continue;
        }
        switch (reader.value<uint8_t>()) {
        case MatchAnswer: {
            MatchKey key = reader.matchKey();
            Match match;
            match.face = reader.faceKey();
            match.exact = int(reader.number());
            if (reader.good)
                matches.emplace(std::move(key), std::move(match));
            break;
        }
        case ListAnswer: {
            const bool asked = reader.flag();
            MatchKey key = reader.matchKey();
            std::string list = reader.text();
            if (reader.good)
                lists.emplace(std::pair(asked, std::move(key)), std::move(list));
            break;
        }
        case FallbackAnswer: {
            FallbackKey key;
            key.face = reader.faceKey();
            key.character = uint32_t(reader.number());
            key.language = reader.text();
            FaceKey face = reader.faceKey();
            if (reader.good)
                fallbacks.emplace(std::move(key), std::move(face));
            break;
        }
        }
        records.push_back(contents.substr(start, sizeof size + size));
    }
    const std::string folder = path.substr(0, path.rfind('/'));
    mkdir(folder.substr(0, folder.rfind('/')).c_str(), 0755);
    mkdir(folder.c_str(), 0755);
    // Those of other fingerprints may be of another program that runs now, unless they are many.
    if (others > 256 || position != contents.size()) {
        const std::string temporary = path + '.' + std::to_string(getpid());
        if (FILE* file = fopen(temporary.c_str(), "wb")) {
            bool written = true;
            for (const std::string& record : records)
                written = written && fwrite(record.data(), 1, record.size(), file) == record.size();
            if (fclose(file) != 0 || !written || rename(temporary.c_str(), path.c_str()) != 0)
                unlink(temporary.c_str());
        }
    }
    answers = open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC, 0644);
    return answers >= 0;
}

static AnswerWriter answerRecord(AnswerKind kind, const AnswerWriter& answer)
{
    AnswerWriter head;
    head.number(answersFingerprint);
    head.bytes.push_back(char(kind));
    AnswerWriter record;
    record.number(head.bytes.size() + answer.bytes.size());
    record.bytes += head.bytes;
    record.bytes += answer.bytes;
    return record;
}

static void keepAnswer(AnswerKind kind, const AnswerWriter& answer)
{
    if (!openAnswers())
        return;
    const AnswerWriter record = answerRecord(kind, answer);
    if (write(answers, record.bytes.data(), record.bytes.size()) != ssize_t(record.bytes.size())) {
        close(answers);
        answers = -1;
    }
}

static const uint16_t macRoman[128] = {
    0x00c4, 0x00c5, 0x00c7, 0x00c9, 0x00d1, 0x00d6, 0x00dc, 0x00e1, 0x00e0, 0x00e2, 0x00e4, 0x00e3, 0x00e5, 0x00e7, 0x00e9, 0x00e8,
    0x00ea, 0x00eb, 0x00ed, 0x00ec, 0x00ee, 0x00ef, 0x00f1, 0x00f3, 0x00f2, 0x00f4, 0x00f6, 0x00f5, 0x00fa, 0x00f9, 0x00fb, 0x00fc,
    0x2020, 0x00b0, 0x00a2, 0x00a3, 0x00a7, 0x2022, 0x00b6, 0x00df, 0x00ae, 0x00a9, 0x2122, 0x00b4, 0x00a8, 0x2260, 0x00c6, 0x00d8,
    0x221e, 0x00b1, 0x2264, 0x2265, 0x00a5, 0x00b5, 0x2202, 0x2211, 0x220f, 0x03c0, 0x222b, 0x00aa, 0x00ba, 0x03a9, 0x00e6, 0x00f8,
    0x00bf, 0x00a1, 0x00ac, 0x221a, 0x0192, 0x2248, 0x2206, 0x00ab, 0x00bb, 0x2026, 0x00a0, 0x00c0, 0x00c3, 0x00d5, 0x0152, 0x0153,
    0x2013, 0x2014, 0x201c, 0x201d, 0x2018, 0x2019, 0x00f7, 0x25ca, 0x00ff, 0x0178, 0x2044, 0x20ac, 0x2039, 0x203a, 0xfb01, 0xfb02,
    0x2021, 0x00b7, 0x201a, 0x201e, 0x2030, 0x00c2, 0x00ca, 0x00c1, 0x00cb, 0x00c8, 0x00cd, 0x00ce, 0x00cf, 0x00cc, 0x00d3, 0x00d4,
    0xf8ff, 0x00d2, 0x00da, 0x00db, 0x00d9, 0x0131, 0x02c6, 0x02dc, 0x00af, 0x02d8, 0x02d9, 0x02da, 0x00b8, 0x02dd, 0x02db, 0x02c7,
};

static std::string decodeName(const FT_SfntName& name)
{
    std::string text;
    if (name.platform_id == TT_PLATFORM_MICROSOFT || name.platform_id == TT_PLATFORM_APPLE_UNICODE) {
        const auto unit = [&](FT_UInt i) { return uint16_t(name.string[i] << 8 | name.string[i + 1]); };
        for (FT_UInt i = 0; i + 1 < name.string_len; i += 2) {
            uint32_t character = unit(i);
            if (character >= 0xd800 && character < 0xdc00 && i + 3 < name.string_len) {
                const uint32_t low = unit(i + 2);
                if (low >= 0xdc00 && low < 0xe000) {
                    character = 0x10000 + ((character - 0xd800) << 10) + (low - 0xdc00);
                    i += 2;
                }
            }
            appendUtf8(text, character);
        }
    } else {
        for (FT_UInt i = 0; i < name.string_len; i++) {
            const uint8_t byte = name.string[i];
            appendUtf8(text, byte < 0x80 ? byte : macRoman[byte - 0x80]);
        }
    }
    return text;
}

// A name of the name table, in English where the font has it; false for none.
static bool sfntName(FT_Face face, FT_UShort id, std::string& name)
{
    int best = -1;
    int bestRank = 100;
    FT_SfntName record;
    const FT_UInt count = FT_Get_Sfnt_Name_Count(face);
    for (FT_UInt i = 0; i < count; i++) {
        if (FT_Get_Sfnt_Name(face, i, &record) || record.name_id != id)
            continue;
        int rank = 50;
        if (record.platform_id == TT_PLATFORM_MICROSOFT)
            rank = record.language_id == TT_MS_LANGID_ENGLISH_UNITED_STATES ? 0 : 10;
        else if (record.platform_id == TT_PLATFORM_APPLE_UNICODE)
            rank = 20;
        else if (record.platform_id == TT_PLATFORM_MACINTOSH && record.encoding_id == TT_MAC_ID_ROMAN)
            rank = record.language_id == TT_MAC_LANGID_ENGLISH ? 30 : 40;
        if (rank < bestRank) {
            best = int(i);
            bestRank = rank;
        }
    }
    if (best < 0)
        return false;
    FT_Get_Sfnt_Name(face, FT_UInt(best), &record);
    name = decodeName(record);
    return !name.empty();
}

static std::string familyName(const reef_ui_face* face)
{
    if (!face->key.family.empty())
        return face->key.family;
    std::string name;
    if (sfntName(face->ftFace, TT_NAME_ID_TYPOGRAPHIC_FAMILY, name) || sfntName(face->ftFace, TT_NAME_ID_FONT_FAMILY, name))
        return name;
    return face->ftFace->family_name ? face->ftFace->family_name : "";
}

static std::string styleName(const reef_ui_face* face)
{
    if (!face->key.style.empty())
        return face->key.style;
    std::string name;
    if (face->key.variations.empty()
        && (sfntName(face->ftFace, TT_NAME_ID_TYPOGRAPHIC_SUBFAMILY, name) || sfntName(face->ftFace, TT_NAME_ID_FONT_SUBFAMILY, name))) {
        if (face->key.bold && name.find("Bold") == std::string::npos)
            name = name == "Regular" ? "Bold" : name + " Bold";
        if (face->key.oblique && name.find("Italic") == std::string::npos && name.find("Oblique") == std::string::npos)
            name = name == "Regular" ? "Oblique" : name + " Oblique";
        return name;
    }
    std::string style = face->ftFace->style_name ? face->ftFace->style_name : "Regular";
    if (face->key.bold && style.find("Bold") == std::string::npos)
        style = style == "Regular" ? "Bold" : style + " Bold";
    if (face->key.oblique && style.find("Italic") == std::string::npos)
        style = style == "Regular" ? "Oblique" : style + " Oblique";
    return style;
}

static std::string postScriptName(const reef_ui_face* face)
{
    if (!face->key.family.empty()) {
        std::string name = face->key.family + "-" + face->key.style;
        name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
        return name;
    }
    std::string name;
    if (const char* own = FT_Get_Postscript_Name(face->ftFace))
        name = own;
    else
        sfntName(face->ftFace, TT_NAME_ID_PS_NAME, name);
    if (name.empty()) {
        name = familyName(face);
        name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
        name += "-" + styleName(face);
    }
    if (face->key.bold || face->key.oblique) {
        const std::string base = name.substr(0, name.find('-'));
        std::string style = styleName(face);
        style.erase(std::remove(style.begin(), style.end(), ' '), style.end());
        name = base + "-" + style;
    }
    return name;
}

static double glyphTop(hb_font_t* font, hb_codepoint_t character)
{
    hb_codepoint_t glyph;
    hb_glyph_extents_t extents;
    if (!hb_font_get_nominal_glyph(font, character, &glyph) || !hb_font_get_glyph_extents(font, glyph, &extents))
        return 0;
    return extents.y_bearing;
}

static void readAxes(reef_ui_face* face)
{
    FT_MM_Var* variable = nullptr;
    if (!FT_HAS_MULTIPLE_MASTERS(face->ftFace) || FT_Get_MM_Var(face->ftFace, &variable) || !variable)
        return;
    face->axisNames.resize(variable->num_axis);
    for (FT_UInt i = 0; i < variable->num_axis; i++) {
        const FT_Var_Axis& source = variable->axis[i];
        reef_ui_face_axis axis = {};
        axis.tag = uint32_t(source.tag);
        axis.minimum = source.minimum / 65536.0;
        axis.standard = source.def / 65536.0;
        axis.maximum = source.maximum / 65536.0;
        FT_UInt flags = 0;
        if (!FT_Get_Var_Axis_Flags(variable, i, &flags))
            axis.hidden = flags & FT_VAR_AXIS_FLAG_HIDDEN;
        face->axisNames[i] = source.name ? source.name : "";
        face->axes.push_back(axis);
    }
    for (size_t i = 0; i < face->axes.size(); i++)
        face->axes[i].name = face->axisNames[i].empty() ? nullptr : face->axisNames[i].c_str();
    FT_Done_MM_Var(freetype(), variable);
}

static void setVariations(reef_ui_face* face)
{
    if (face->key.variations.empty() || face->axes.empty())
        return;
    std::vector<FT_Fixed> coordinates;
    std::vector<hb_variation_t> variations;
    for (const reef_ui_face_axis& axis : face->axes) {
        double value = axis.standard;
        for (const auto& [tag, wanted] : face->key.variations) {
            if (tag == axis.tag)
                value = std::min(std::max(wanted, axis.minimum), axis.maximum);
        }
        coordinates.push_back(std::lround(value * 65536));
        variations.push_back({axis.tag, float(value)});
    }
    FT_Set_Var_Design_Coordinates(face->ftFace, FT_UInt(coordinates.size()), coordinates.data());
    hb_font_set_variations(face->font, variations.data(), unsigned(variations.size()));
}

static void readMetrics(reef_ui_face* face)
{
    const FT_Face ftFace = face->ftFace;
    reef_ui_face_metrics& metrics = face->metrics;
    const auto* head = static_cast<const TT_Header*>(FT_Get_Sfnt_Table(ftFace, FT_SFNT_HEAD));
    metrics.unitsPerEm = ftFace->units_per_EM ? ftFace->units_per_EM : head && head->Units_Per_EM ? head->Units_Per_EM : 1000;
    const auto* hhea = static_cast<const TT_HoriHeader*>(FT_Get_Sfnt_Table(ftFace, FT_SFNT_HHEA));
    const auto* os2 = static_cast<const TT_OS2*>(FT_Get_Sfnt_Table(ftFace, FT_SFNT_OS2));
    const auto* post = static_cast<const TT_Postscript*>(FT_Get_Sfnt_Table(ftFace, FT_SFNT_POST));
    const bool hasOs2 = os2 && os2->version != 0xffff;
    const bool typographic = hasOs2 && os2->fsSelection & 128;
    if (!typographic && hhea && (hhea->Ascender || hhea->Descender)) {
        metrics.ascent = hhea->Ascender;
        metrics.descent = -hhea->Descender;
        metrics.leading = hhea->Line_Gap;
    } else if (hasOs2 && (os2->sTypoAscender || os2->sTypoDescender)) {
        metrics.ascent = os2->sTypoAscender;
        metrics.descent = -os2->sTypoDescender;
        metrics.leading = os2->sTypoLineGap;
    } else {
        metrics.ascent = ftFace->ascender;
        metrics.descent = -ftFace->descender;
        metrics.leading = std::max(0, ftFace->height - (ftFace->ascender - ftFace->descender));
    }
    if (!FT_IS_SCALABLE(ftFace) && ftFace->num_fixed_sizes > 0 && metrics.ascent == 0) {
        metrics.ascent = metrics.unitsPerEm * 0.8;
        metrics.descent = metrics.unitsPerEm * 0.2;
    }
    metrics.weight = 400;
    metrics.width = 5;
    if (hasOs2) {
        metrics.weight = os2->usWeightClass;
        metrics.width = os2->usWidthClass;
        if (os2->version >= 2) {
            metrics.capHeight = os2->sCapHeight;
            metrics.xHeight = os2->sxHeight;
        }
    }
    if (post) {
        metrics.italicAngle = post->italicAngle / 65536.0;
        metrics.underlinePosition = post->underlinePosition;
        metrics.underlineThickness = post->underlineThickness;
    }
    if (metrics.underlineThickness <= 0) {
        metrics.underlinePosition = -metrics.unitsPerEm / 10;
        metrics.underlineThickness = metrics.unitsPerEm / 14;
    }
    metrics.bounds[0] = ftFace->bbox.xMin * face->key.stretch;
    metrics.bounds[1] = ftFace->bbox.yMin;
    metrics.bounds[2] = ftFace->bbox.xMax * face->key.stretch;
    metrics.bounds[3] = ftFace->bbox.yMax;
    metrics.glyphCount = uint32_t(ftFace->num_glyphs);
    uint32_t flags = 0;
    if (ftFace->style_flags & FT_STYLE_FLAG_ITALIC)
        flags |= FaceItalic;
    if (ftFace->style_flags & FT_STYLE_FLAG_BOLD || metrics.weight >= 600)
        flags |= FaceBold;
    if (FT_IS_FIXED_WIDTH(ftFace) || (post && post->isFixedPitch))
        flags |= FaceFixedPitch;
    if (FT_HAS_COLOR(ftFace))
        flags |= FaceColor;
    if (FT_IS_SCALABLE(ftFace))
        flags |= FaceScalable;
    if (FT_HAS_VERTICAL(ftFace))
        flags |= FaceVertical;
    if (FT_HAS_MULTIPLE_MASTERS(ftFace))
        flags |= FaceVariable;
    if (face->key.bold)
        flags |= FaceBold | FaceMadeBold;
    if (face->key.oblique) {
        flags |= FaceItalic | FaceMadeOblique;
        metrics.italicAngle = -std::atan(obliqueSlant) * 180 / M_PI;
    }
    metrics.flags = flags;
    if (metrics.capHeight <= 0) {
        metrics.capHeight = glyphTop(face->font, 'H');
        if (metrics.capHeight <= 0)
            metrics.capHeight = metrics.ascent * 0.7;
    }
    if (metrics.xHeight <= 0) {
        metrics.xHeight = glyphTop(face->font, 'x');
        if (metrics.xHeight <= 0)
            metrics.xHeight = metrics.ascent * 0.5;
    }
}

// The face that is there already, else a new one. Null when it cannot be read.
static reef_ui_face* loadFace(const FaceKey& key, std::shared_ptr<std::string> data)
{
    if (!data) {
        const auto it = faces.find(key);
        if (it != faces.end()) {
            faceRetain(it->second);
            return it->second;
        }
    }
    FT_Face ftFace = nullptr;
    const FT_Error error = data
            ? FT_New_Memory_Face(freetype(), reinterpret_cast<const FT_Byte*>(data->data()), FT_Long(data->size()), key.index, &ftFace)
            : FT_New_Face(freetype(), key.path.c_str(), key.index, &ftFace);
    if (error || !ftFace)
        return nullptr;
    if (!ftFace->charmap)
        FT_Select_Charmap(ftFace, FT_ENCODING_UNICODE);
    reef_ui_face* face = new reef_ui_face;
    face->key = key;
    face->data = std::move(data);
    face->ftFace = ftFace;
    hb_blob_t* blob = face->data
            ? hb_blob_create(face->data->data(), unsigned(face->data->size()), HB_MEMORY_MODE_READONLY, nullptr, nullptr)
            : hb_blob_create_from_file_or_fail(key.path.c_str());
    if (!blob) {
        FT_Done_Face(ftFace);
        delete face;
        return nullptr;
    }
    face->hbFace = hb_face_create(blob, key.index & 0xffff);
    hb_blob_destroy(blob);
    if (hb_face_get_glyph_count(face->hbFace) == 0 && ftFace->num_glyphs > 0) {
        // Not one that HarfBuzz reads itself, such as a Type 1 font.
        hb_face_destroy(face->hbFace);
        face->hbFace = nullptr;
        FT_Face own = nullptr;
        const FT_Error error = face->data ? FT_New_Memory_Face(freetype(), reinterpret_cast<const FT_Byte*>(face->data->data()),
                                                               FT_Long(face->data->size()), key.index, &own)
                                          : FT_New_Face(freetype(), key.path.c_str(), key.index, &own);
        if (error || !own) {
            FT_Done_Face(ftFace);
            delete face;
            return nullptr;
        }
        FT_Set_Char_Size(own, 0, own->units_per_EM ? own->units_per_EM : 1000, 72, 72);
        face->font = hb_ft_font_create_referenced(own);
        FT_Done_Face(own);
        hb_ft_font_set_load_flags(face->font, FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP);
        face->hbFace = hb_face_reference(hb_font_get_face(face->font));
    } else {
        face->font = hb_font_create(face->hbFace);
    }
    const int unitsPerEm = int(hb_face_get_upem(face->hbFace));
    hb_font_set_scale(face->font, int(std::lround(unitsPerEm * key.stretch)), unitsPerEm);
    const int instance = key.index >> 16;
    if (instance > 0)
        hb_font_set_var_named_instance(face->font, unsigned(instance - 1));
    if (key.bold)
        hb_font_set_synthetic_bold(face->font, float(boldStrength), float(boldStrength), true);
    if (key.oblique)
        hb_font_set_synthetic_slant(face->font, float(obliqueSlant));
    readAxes(face);
    setVariations(face);
    readMetrics(face);
    if (!face->data)
        faces[key] = face;
    return face;
}

static reef_ui_face* faceFromPattern(FcPattern* pattern, bool bold, bool oblique)
{
    FcChar8* file = nullptr;
    int index = 0;
    if (FcPatternGetString(pattern, FC_FILE, 0, &file) != FcResultMatch || !file)
        return nullptr;
    FcPatternGetInteger(pattern, FC_INDEX, 0, &index);
    FaceKey key;
    key.path = reinterpret_cast<const char*>(file);
    key.index = index;
    key.bold = bold;
    key.oblique = oblique;
    return loadFace(key, nullptr);
}

static reef_ui_face* faceNamed(const char* object, const char* name)
{
    FcPattern* pattern = FcPatternCreate();
    FcPatternAddString(pattern, object, reinterpret_cast<const FcChar8*>(name));
    FcObjectSet* objects = FcObjectSetBuild(FC_FILE, FC_INDEX, FC_VARIABLE, nullptr);
    const Config config;
    FcFontSet* fonts = FcFontList(config, pattern, objects);
    FcObjectSetDestroy(objects);
    FcPatternDestroy(pattern);
    if (!fonts)
        return nullptr;
    reef_ui_face* face = nullptr;
    for (int i = 0; i < fonts->nfont && !face; i++) {
        FcBool variable = FcFalse;
        if (FcPatternGetBool(fonts->fonts[i], FC_VARIABLE, 0, &variable) == FcResultMatch && variable)
            continue;
        face = faceFromPattern(fonts->fonts[i], false, false);
    }
    FcFontSetDestroy(fonts);
    return face;
}

namespace {

// Families whose glyphs are as wide as those of the one that is asked for.
struct Substitute
{
    const char* family;
    const char* names[4];
};

} // namespace

static const Substitute substitutes[] = {
    {"Times", {"Liberation Serif", "Tinos", "Nimbus Roman", "TeX Gyre Termes"}},
    {"Times New Roman", {"Liberation Serif", "Tinos", "Nimbus Roman", "TeX Gyre Termes"}},
    {"Times Roman", {"Liberation Serif", "Tinos", "Nimbus Roman", "TeX Gyre Termes"}},
    {"Arial", {"Liberation Sans", "Arimo", "Nimbus Sans", "TeX Gyre Heros"}},
    {"Courier New", {"Liberation Mono", "Cousine", "Nimbus Mono PS", "TeX Gyre Cursor"}},
    {"Arial Narrow", {"Liberation Sans Narrow", "Nimbus Sans Narrow"}},
    {"Georgia", {"Gelasio"}},
    {"Calibri", {"Carlito"}},
    {"Cambria", {"Caladea"}},
    {"Palatino", {"TeX Gyre Pagella", "P052"}},
    {"Bookman", {"TeX Gyre Bonum", "URW Bookman"}},
    {"New Century Schoolbook", {"TeX Gyre Schola", "C059"}},
};

static bool standsInFor(const char* family, const char* other)
{
    for (const Substitute& substitute : substitutes) {
        if (!equalIgnoringCase(substitute.family, family))
            continue;
        for (const char* name : substitute.names) {
            if (!name)
                break;
            if (equalIgnoringCase(name, other))
                return true;
        }
    }
    return false;
}

static reef_ui_face* findFace(const reef_ui_face_query* query, int* exact)
{
    if (query->name && *query->name) {
        for (const char* object : {FC_POSTSCRIPT_NAME, FC_FULLNAME}) {
            if (reef_ui_face* face = faceNamed(object, query->name)) {
                *exact = 1;
                return face;
            }
        }
    }
    FcPattern* pattern = FcPatternCreate();
    addQuery(pattern, query, true);
    const Config config;
    FcConfigSubstitute(config, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);
    FcResult result;
    FcPattern* match = FcFontMatch(config, pattern, &result);
    FcPatternDestroy(pattern);
    if (!match)
        return nullptr;
    FcBool embolden = FcFalse;
    FcPatternGetBool(match, FC_EMBOLDEN, 0, &embolden);
    double weight = FC_WEIGHT_REGULAR;
    FcPatternGetDouble(match, FC_WEIGHT, 0, &weight);
    int slant = FC_SLANT_ROMAN;
    FcPatternGetInteger(match, FC_SLANT, 0, &slant);
    const bool bold = embolden || (query->weight >= 600 && weight < FcWeightFromOpenTypeDouble(550));
    FcMatrix* matrix = nullptr;
    const bool sheared = FcPatternGetMatrix(match, FC_MATRIX, 0, &matrix) == FcResultMatch && matrix && matrix->xy != 0;
    const bool oblique = query->italic == 1 && (slant == FC_SLANT_ROMAN || sheared);
    reef_ui_face* face = faceFromPattern(match, bold, oblique);
    if (face && query->family && *query->family) {
        FcChar8* value = nullptr;
        for (int i = 0; FcPatternGetString(match, FC_FAMILY, i, &value) == FcResultMatch; i++) {
            const char* family = reinterpret_cast<const char*>(value);
            if (equalIgnoringCase(family, query->family) || standsInFor(query->family, family))
                *exact = 1;
        }
    }
    FcPatternDestroy(match);
    return face;
}

namespace {

struct CondensedStyle
{
    double weight;
    const char* name;
    // The advance of the digits, in ems.
    double digitWidth;
};

} // namespace

// Avenir Next Condensed is made of the font of the interface, narrowed until its digits are as wide.
static const char condensedFamily[] = "Avenir Next Condensed";

static const CondensedStyle condensedStyles[] = {
    {200, "Ultra Light", 0.46}, {400, "Regular", 0.47}, {500, "Medium", 0.482},
    {600, "Demi Bold", 0.5},    {700, "Bold", 0.52},    {800, "Heavy", 0.54},
};

// Null when another family is asked for.
static const CondensedStyle* condensedStyle(const reef_ui_face_query* query)
{
    static const std::string family = condensedFamily;
    static const std::string compact = withoutSpaces(family);
    const auto styleNamed = [](const std::string& name) -> const CondensedStyle* {
        for (const CondensedStyle& style : condensedStyles) {
            if (equalIgnoringCase(withoutSpaces(style.name), name.c_str()))
                return &style;
        }
        return nullptr;
    };
    const CondensedStyle* regular = &condensedStyles[1];
    if (query->name && *query->name) {
        const std::string name = query->name;
        const size_t dash = name.find('-');
        if (equalIgnoringCase(withoutSpaces(name.substr(0, dash)), compact.c_str()))
            return dash == std::string::npos ? regular : styleNamed(withoutSpaces(name.substr(dash + 1)));
    }
    if (!query->family || !equalIgnoringCase(withoutSpaces(query->family), compact.c_str()))
        return nullptr;
    if (query->style && *query->style) {
        if (const CondensedStyle* style = styleNamed(withoutSpaces(query->style)))
            return style;
    }
    const double weight = query->weight > 0 ? query->weight : 400;
    const CondensedStyle* best = regular;
    for (const CondensedStyle& style : condensedStyles) {
        if (std::abs(style.weight - weight) < std::abs(best->weight - weight))
            best = &style;
    }
    return best;
}

static reef_ui_face* condensedFace(const reef_ui_face_query* query)
{
    const CondensedStyle* style = condensedStyle(query);
    if (!style)
        return nullptr;
    const std::string& themeFamily = ui::Theme::current().fontFamily;
    const std::string family = themeFamily.empty() ? "sans-serif" : themeFamily;
    reef_ui_face_query plain = {};
    plain.family = family.c_str();
    plain.weight = style->weight;
    plain.italic = query->italic;
    plain.fixedPitch = -1;
    int exact = 0;
    reef_ui_face* face = findFace(&plain, &exact);
    if (!face)
        return nullptr;
    hb_codepoint_t glyph = 0;
    const unsigned unitsPerEm = hb_face_get_upem(face->hbFace);
    if (!hb_font_get_nominal_glyph(face->font, '8', &glyph) || unitsPerEm == 0)
        return face;
    const double advance = hb_font_get_glyph_h_advance(face->font, glyph) / double(unitsPerEm);
    if (advance <= 0)
        return face;
    FaceKey key = face->key;
    faceRelease(face);
    key.stretch = style->digitWidth / advance;
    key.family = condensedFamily;
    key.style = style->name;
    return loadFace(key, nullptr);
}

// 121
reef_ui_face* faceMatch(const reef_ui_face_query* query, int* exact)
{
    if (exact)
        *exact = 0;
    if (!query)
        return nullptr;
    std::lock_guard lock(mutex);
    const bool keep = openAnswers();
    MatchKey key = matchKey(query);
    const auto it = matches.find(key);
    if (it != matches.end()) {
        reef_ui_face* face = it->second.face.path.empty() ? nullptr : loadFace(it->second.face, nullptr);
        if (face && exact)
            *exact = it->second.exact;
        return face;
    }
    Match match;
    reef_ui_face* face = findFace(query, &match.exact);
    if (!match.exact) {
        if (reef_ui_face* condensed = condensedFace(query)) {
            if (face)
                faceRelease(face);
            face = condensed;
            match.exact = 1;
        }
    }
    if (face)
        match.face = face->key;
    else
        match.exact = 0;
    if (exact)
        *exact = match.exact;
    if (keep) {
        AnswerWriter answer;
        answer.matchKey(key);
        answer.faceKey(match.face);
        answer.number(uint64_t(match.exact));
        keepAnswer(MatchAnswer, answer);
    }
    if (matches.size() >= mostMatches)
        matches.clear();
    matches.emplace(std::move(key), std::move(match));
    return face;
}

static std::string listFaces(const reef_ui_face_query* query)
{
    FcPattern* pattern = FcPatternCreate();
    if (query) {
        addQuery(pattern, query, false);
        if (query->name && *query->name)
            FcPatternAddString(pattern, FC_POSTSCRIPT_NAME, reinterpret_cast<const FcChar8*>(query->name));
    }
    FcObjectSet* objects = FcObjectSetBuild(FC_FILE, FC_INDEX, FC_POSTSCRIPT_NAME, FC_FAMILY, FC_FAMILYLANG, FC_STYLE, FC_STYLELANG,
                                            FC_WEIGHT, FC_WIDTH, FC_SLANT, FC_SPACING, FC_VARIABLE, FC_FONTFORMAT, nullptr);
    const Config config;
    FcFontSet* fonts = FcFontList(config, pattern, objects);
    FcObjectSetDestroy(objects);
    FcPatternDestroy(pattern);
    std::string list;
    if (!fonts)
        return list;
    for (int i = 0; i < fonts->nfont; i++) {
        FcPattern* font = fonts->fonts[i];
        // Its named instances are there too.
        FcBool variable = FcFalse;
        if (FcPatternGetBool(font, FC_VARIABLE, 0, &variable) == FcResultMatch && variable)
            continue;
        const std::string format = patternString(font, FC_FONTFORMAT);
        if (format == "PCF" || format == "BDF" || format == "PFR" || format == "Type 1")
            continue;
        double weight = FC_WEIGHT_REGULAR;
        double width = FC_WIDTH_NORMAL;
        int slant = FC_SLANT_ROMAN;
        int spacing = FC_PROPORTIONAL;
        int index = 0;
        FcPatternGetDouble(font, FC_WEIGHT, 0, &weight);
        FcPatternGetDouble(font, FC_WIDTH, 0, &width);
        FcPatternGetInteger(font, FC_SLANT, 0, &slant);
        FcPatternGetInteger(font, FC_SPACING, 0, &spacing);
        FcPatternGetInteger(font, FC_INDEX, 0, &index);
        const double openTypeWeight = FcWeightToOpenTypeDouble(weight);
        if (query) {
            if (query->weight > 0 && std::abs(openTypeWeight - query->weight) > 50)
                continue;
            if (query->italic >= 0 && (slant != 0) != (query->italic != 0))
                continue;
            if (query->fixedPitch == 1 && spacing < FC_MONO)
                continue;
        }
        const std::string name = patternString(font, FC_POSTSCRIPT_NAME);
        list += patternString(font, FC_FILE) + "\t" + std::to_string(index) + "\t" + name + "\t"
                + localizedString(font, FC_FAMILY, FC_FAMILYLANG) + "\t" + localizedString(font, FC_STYLE, FC_STYLELANG) + "\t"
                + std::to_string(int(std::lround(openTypeWeight))) + "\t" + std::to_string(widthClass(width)) + "\t"
                + (slant ? "1" : "0") + "\t" + (spacing >= FC_MONO ? "1" : "0") + "\n";
    }
    FcFontSetDestroy(fonts);
    return list;
}

// 122
int faceList(const reef_ui_face_query* query, char* buffer, int size)
{
    std::lock_guard lock(mutex);
    const bool keep = openAnswers();
    std::pair<bool, MatchKey> key(query != nullptr, query ? matchKey(query) : MatchKey());
    const auto it = lists.find(key);
    if (it != lists.end())
        return copyText(it->second, buffer, size);
    std::string list = listFaces(query);
    if (keep) {
        AnswerWriter answer;
        answer.flag(key.first);
        answer.matchKey(key.second);
        answer.text(list);
        keepAnswer(ListAnswer, answer);
    }
    if (lists.size() >= mostLists)
        lists.clear();
    return copyText(lists.emplace(std::move(key), std::move(list)).first->second, buffer, size);
}

// 123
reef_ui_face* faceFromFile(const char* path, int index)
{
    if (!path || !*path)
        return nullptr;
    std::lock_guard lock(mutex);
    FaceKey key;
    key.path = path;
    key.index = index;
    return loadFace(key, nullptr);
}

// 124
reef_ui_face* faceFromData(const void* data, size_t size, int index)
{
    if (!data || size == 0)
        return nullptr;
    std::lock_guard lock(mutex);
    FaceKey key;
    key.index = index;
    return loadFace(key, std::make_shared<std::string>(static_cast<const char*>(data), size));
}

// 125
void faceRetain(reef_ui_face* face)
{
    if (face)
        face->references++;
}

static void clearRuns()
{
    runs.clear();
    runOrder.clear();
    runBytes = 0;
}

// 126
void faceRelease(reef_ui_face* face)
{
    if (!face)
        return;
    std::lock_guard lock(mutex);
    if (--face->references > 0)
        return;
    const auto it = faces.find(face->key);
    if (it != faces.end() && it->second == face)
        faces.erase(it);
    for (const auto& [character, fallback] : face->fallbacks) {
        if (fallback)
            faceRelease(fallback);
    }
    for (const auto& [character, fallback] : face->languageFallbacks) {
        if (fallback)
            faceRelease(fallback);
    }
    clearRuns();
    for (auto mask = masks.begin(); mask != masks.end();) {
        if (mask->first.face == face) {
            maskBytes -= mask->second.pixels.size();
            mask = masks.erase(mask);
        } else {
            ++mask;
        }
    }
    if (face->font)
        hb_font_destroy(face->font);
    if (face->hbFace)
        hb_face_destroy(face->hbFace);
    if (face->ftFace)
        FT_Done_Face(face->ftFace);
    delete face;
}

// 127
void faceMetrics(reef_ui_face* face, reef_ui_face_metrics* metrics)
{
    if (face && metrics)
        *metrics = face->metrics;
}

// 128
int faceName(reef_ui_face* face, int id, char* buffer, int size)
{
    if (!face)
        return -1;
    std::lock_guard lock(mutex);
    std::string name;
    switch (id) {
    case -1:
        if (face->key.path.empty())
            return -1;
        name = face->key.path;
        break;
    case TT_NAME_ID_FONT_FAMILY:
        name = familyName(face);
        break;
    case TT_NAME_ID_FONT_SUBFAMILY:
        name = styleName(face);
        break;
    case TT_NAME_ID_FULL_NAME:
        if (face->key.bold || face->key.oblique || !face->key.variations.empty() || !face->key.family.empty()
            || !sfntName(face->ftFace, TT_NAME_ID_FULL_NAME, name)) {
            name = familyName(face);
            const std::string style = styleName(face);
            if (style != "Regular")
                name += " " + style;
        }
        break;
    case TT_NAME_ID_PS_NAME:
        name = postScriptName(face);
        break;
    default:
        if (!sfntName(face->ftFace, FT_UShort(id), name))
            return -1;
    }
    return copyText(name, buffer, size);
}

// 129
void faceGlyphs(reef_ui_face* face, const uint32_t* characters, uint32_t count, uint32_t* glyphs)
{
    if (!face)
        return;
    std::lock_guard lock(mutex);
    for (uint32_t i = 0; i < count; i++) {
        hb_codepoint_t glyph = 0;
        glyphs[i] = hb_font_get_nominal_glyph(face->font, characters[i], &glyph) ? glyph : 0;
    }
}

// 130
void faceAdvances(reef_ui_face* face, const uint32_t* glyphs, uint32_t count, int vertical, double* advances)
{
    if (!face)
        return;
    std::lock_guard lock(mutex);
    for (uint32_t i = 0; i < count; i++) {
        advances[i] = vertical ? -double(hb_font_get_glyph_v_advance(face->font, glyphs[i]))
                               : hb_font_get_glyph_h_advance(face->font, glyphs[i]);
    }
}

// 131
void faceBounds(reef_ui_face* face, const uint32_t* glyphs, uint32_t count, double* bounds)
{
    if (!face)
        return;
    std::lock_guard lock(mutex);
    for (uint32_t i = 0; i < count; i++) {
        hb_glyph_extents_t extents = {};
        double* rect = bounds + i * 4;
        if (hb_font_get_glyph_extents(face->font, glyphs[i], &extents)) {
            rect[0] = extents.x_bearing;
            rect[1] = extents.y_bearing + extents.height;
            rect[2] = extents.width;
            rect[3] = -extents.height;
        } else {
            rect[0] = rect[1] = rect[2] = rect[3] = 0;
        }
    }
}

namespace {

struct Outline
{
    reef_ui_path_element* elements;
    uint32_t capacity;
    uint32_t count = 0;

    void add(int type, std::initializer_list<double> points)
    {
        const uint32_t index = count;
        if (index < capacity) {
            reef_ui_path_element& element = elements[index];
            element = {};
            element.type = type;
            std::copy(points.begin(), points.end(), element.points);
        }
        count = index + 1;
    }
};

} // namespace

static void outlineMove(hb_draw_funcs_t*, void* outline, hb_draw_state_t*, float x, float y, void*)
{
    static_cast<Outline*>(outline)->add(0, {x, y});
}

static void outlineLine(hb_draw_funcs_t*, void* outline, hb_draw_state_t*, float x, float y, void*)
{
    static_cast<Outline*>(outline)->add(1, {x, y});
}

static void outlineQuadratic(hb_draw_funcs_t*, void* outline, hb_draw_state_t*, float controlX, float controlY, float x, float y, void*)
{
    static_cast<Outline*>(outline)->add(2, {controlX, controlY, x, y});
}

static void outlineCubic(hb_draw_funcs_t*, void* outline, hb_draw_state_t*, float control1X, float control1Y, float control2X,
                         float control2Y, float x, float y, void*)
{
    static_cast<Outline*>(outline)->add(3, {control1X, control1Y, control2X, control2Y, x, y});
}

static void outlineClose(hb_draw_funcs_t*, void* outline, hb_draw_state_t*, void*)
{
    static_cast<Outline*>(outline)->add(4, {});
}

static hb_draw_funcs_t* outlineFunctions()
{
    static hb_draw_funcs_t* const functions = [] {
        hb_draw_funcs_t* functions = hb_draw_funcs_create();
        hb_draw_funcs_set_move_to_func(functions, outlineMove, nullptr, nullptr);
        hb_draw_funcs_set_line_to_func(functions, outlineLine, nullptr, nullptr);
        hb_draw_funcs_set_quadratic_to_func(functions, outlineQuadratic, nullptr, nullptr);
        hb_draw_funcs_set_cubic_to_func(functions, outlineCubic, nullptr, nullptr);
        hb_draw_funcs_set_close_path_func(functions, outlineClose, nullptr, nullptr);
        hb_draw_funcs_make_immutable(functions);
        return functions;
    }();
    return functions;
}

// 132
uint32_t faceOutline(reef_ui_face* face, uint32_t glyph, reef_ui_path_element* elements, uint32_t capacity)
{
    if (!face)
        return 0;
    std::lock_guard lock(mutex);
    Outline outline = {elements, elements ? capacity : 0};
    hb_font_draw_glyph(face->font, glyph, outlineFunctions(), &outline);
    return outline.count;
}

// 133
int faceGlyphName(reef_ui_face* face, uint32_t glyph, char* buffer, int size)
{
    if (!face)
        return -1;
    std::lock_guard lock(mutex);
    char name[256];
    if (!hb_font_get_glyph_name(face->font, glyph, name, sizeof name) || !*name)
        return -1;
    return copyText(name, buffer, size);
}

// 134
uint32_t faceGlyphNamed(reef_ui_face* face, const char* name)
{
    if (!face || !name)
        return 0;
    std::lock_guard lock(mutex);
    hb_codepoint_t glyph = 0;
    return hb_font_get_glyph_from_name(face->font, name, -1, &glyph) ? glyph : 0;
}

// 135
size_t faceTable(reef_ui_face* face, uint32_t tag, void* buffer, size_t size)
{
    if (!face)
        return size_t(-1);
    std::lock_guard lock(mutex);
    FT_ULong length = 0;
    if (FT_Load_Sfnt_Table(face->ftFace, tag, 0, nullptr, &length))
        return size_t(-1);
    if (buffer && size >= length)
        FT_Load_Sfnt_Table(face->ftFace, tag, 0, static_cast<FT_Byte*>(buffer), &length);
    return length;
}

// 136
uint32_t faceTables(reef_ui_face* face, uint32_t* tags, uint32_t capacity)
{
    if (!face)
        return 0;
    std::lock_guard lock(mutex);
    FT_ULong count = 0;
    if (FT_Sfnt_Table_Info(face->ftFace, 0, nullptr, &count))
        return 0;
    for (uint32_t i = 0; i < count && i < capacity && tags; i++) {
        FT_ULong tag = 0;
        FT_ULong length = 0;
        FT_Sfnt_Table_Info(face->ftFace, i, &tag, &length);
        tags[i] = uint32_t(tag);
    }
    return uint32_t(count);
}

// 137
uint32_t faceCharacterRanges(reef_ui_face* face, uint32_t* ranges, uint32_t capacity)
{
    if (!face)
        return 0;
    std::lock_guard lock(mutex);
    if (!face->hasRanges) {
        face->hasRanges = true;
        hb_set_t* characters = hb_set_create();
        hb_face_collect_unicodes(face->hbFace, characters);
        hb_codepoint_t first = HB_SET_VALUE_INVALID;
        hb_codepoint_t last = HB_SET_VALUE_INVALID;
        while (hb_set_next_range(characters, &first, &last)) {
            face->ranges.push_back(first);
            face->ranges.push_back(last);
        }
        hb_set_destroy(characters);
    }
    const uint32_t count = uint32_t(face->ranges.size() / 2);
    if (ranges)
        memcpy(ranges, face->ranges.data(), std::min(capacity, count) * 2 * sizeof(uint32_t));
    return count;
}

// 138
int faceLanguages(reef_ui_face* face, char* buffer, int size)
{
    if (!face)
        return copyText({}, buffer, size);
    std::lock_guard lock(mutex);
    std::string text;
    FcPattern* pattern = FcFreeTypeQueryFace(face->ftFace, reinterpret_cast<const FcChar8*>(face->key.path.c_str()),
                                             unsigned(face->key.index), nullptr);
    FcLangSet* languages = nullptr;
    if (pattern) {
        if (FcPatternGetLangSet(pattern, FC_LANG, 0, &languages) == FcResultMatch && languages) {
            FcStrSet* names = FcLangSetGetLangs(languages);
            FcStrList* list = FcStrListCreate(names);
            while (const FcChar8* name = FcStrListNext(list)) {
                text += reinterpret_cast<const char*>(name);
                text += '\n';
            }
            FcStrListDone(list);
            FcStrSetDestroy(names);
        }
        FcPatternDestroy(pattern);
    }
    return copyText(text, buffer, size);
}

// 139
reef_ui_face* faceFallback(reef_ui_face* face, uint32_t character, const char* language)
{
    if (!face)
        return nullptr;
    std::lock_guard lock(mutex);
    const bool hasLanguage = language && *language;
    const auto remember = [&](reef_ui_face* found) {
        if ((hasLanguage ? face->languageFallbacks.size() : face->fallbacks.size()) >= mostFallbacks)
            return;
        faceRetain(found);
        if (hasLanguage)
            face->languageFallbacks[{character, language}] = found;
        else
            face->fallbacks[character] = found;
    };
    if (hasLanguage) {
        const auto it = face->languageFallbacks.find({character, language});
        if (it != face->languageFallbacks.end()) {
            faceRetain(it->second);
            return it->second;
        }
    } else {
        const auto it = face->fallbacks.find(character);
        if (it != face->fallbacks.end()) {
            faceRetain(it->second);
            return it->second;
        }
    }
    // Text in a script goes on in the same font.
    for (const auto& [known, fallback] : face->fallbacks) {
        if (hasLanguage)
            break;
        hb_codepoint_t glyph = 0;
        if (fallback && hb_font_get_nominal_glyph(fallback->font, character, &glyph) && glyph) {
            faceRetain(fallback);
            remember(fallback);
            return fallback;
        }
    }
    const bool keep = !face->data && openAnswers();
    const FallbackKey key = {face->key, character, orEmpty(language)};
    if (keep) {
        const auto it = fallbacks.find(key);
        if (it != fallbacks.end()) {
            reef_ui_face* found = it->second.path.empty() ? nullptr : loadFace(it->second, nullptr);
            remember(found);
            return found;
        }
    }
    FcPattern* pattern = FcPatternCreate();
    FcPatternAddDouble(pattern, FC_WEIGHT, FcWeightFromOpenTypeDouble(face->metrics.weight));
    FcPatternAddInteger(pattern, FC_SLANT, face->metrics.flags & FaceItalic ? FC_SLANT_ITALIC : FC_SLANT_ROMAN);
    if (face->metrics.flags & FaceFixedPitch)
        FcPatternAddInteger(pattern, FC_SPACING, FC_MONO);
    if (language && *language)
        FcPatternAddString(pattern, FC_LANG, reinterpret_cast<const FcChar8*>(language));
    FcCharSet* characters = FcCharSetCreate();
    FcCharSetAddChar(characters, character);
    FcPatternAddCharSet(pattern, FC_CHARSET, characters);
    FcCharSetDestroy(characters);
    const Config config;
    FcConfigSubstitute(config, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);
    FcResult result;
    FcFontSet* fonts = FcFontSort(config, pattern, FcFalse, nullptr, &result);
    FcPatternDestroy(pattern);
    reef_ui_face* found = nullptr;
    if (fonts) {
        for (int i = 0; i < fonts->nfont; i++) {
            FcCharSet* covered = nullptr;
            if (FcPatternGetCharSet(fonts->fonts[i], FC_CHARSET, 0, &covered) != FcResultMatch || !FcCharSetHasChar(covered, character))
                continue;
            reef_ui_face* other = faceFromPattern(fonts->fonts[i], false, false);
            if (!other)
                continue;
            hb_codepoint_t glyph = 0;
            if (other != face && hb_font_get_nominal_glyph(other->font, character, &glyph) && glyph) {
                found = other;
                break;
            }
            faceRelease(other);
        }
        FcFontSetDestroy(fonts);
    }
    if (keep) {
        AnswerWriter answer;
        answer.faceKey(key.face);
        answer.number(character);
        answer.text(key.language);
        answer.faceKey(found ? found->key : FaceKey());
        keepAnswer(FallbackAnswer, answer);
        fallbacks.emplace(key, found ? found->key : FaceKey());
    }
    remember(found);
    return found;
}

// 140
uint32_t faceShape(reef_ui_face* face, const uint16_t* text, uint32_t length, uint32_t start, uint32_t count, double size, int flags,
                   uint32_t script, const char* language, const reef_ui_feature* features, uint32_t featureCount, reef_ui_glyph* glyphs,
                   uint32_t capacity)
{
    if (!face || !text || start > length)
        return 0;
    count = std::min(count, length - start);
    std::lock_guard lock(mutex);
    hb_buffer_t* buffer = hb_buffer_create();
    hb_buffer_add_utf16(buffer, text, int(length), start, int(count));
    if (flags & ShapeTopToBottom)
        hb_buffer_set_direction(buffer, HB_DIRECTION_TTB);
    else
        hb_buffer_set_direction(buffer, flags & ShapeRightToLeft ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
    if (script)
        hb_buffer_set_script(buffer, hb_script_from_iso15924_tag(script));
    if (language && *language)
        hb_buffer_set_language(buffer, hb_language_from_string(language, -1));
    hb_buffer_guess_segment_properties(buffer);
    std::vector<hb_feature_t> list;
    for (uint32_t i = 0; i < featureCount; i++)
        list.push_back({features[i].tag, features[i].value, features[i].start, features[i].end});
    hb_shape(face->font, buffer, list.data(), unsigned(list.size()));
    unsigned glyphCount = 0;
    const hb_glyph_info_t* infos = hb_buffer_get_glyph_infos(buffer, &glyphCount);
    const hb_glyph_position_t* positions = hb_buffer_get_glyph_positions(buffer, nullptr);
    if (glyphs && glyphCount <= capacity) {
        const double scale = size / face->metrics.unitsPerEm;
        for (unsigned i = 0; i < glyphCount; i++) {
            glyphs[i].glyph = infos[i].codepoint;
            glyphs[i].cluster = infos[i].cluster;
            glyphs[i].xAdvance = positions[i].x_advance * scale;
            glyphs[i].yAdvance = positions[i].y_advance * scale;
            glyphs[i].xOffset = positions[i].x_offset * scale;
            glyphs[i].yOffset = positions[i].y_offset * scale;
        }
    }
    hb_buffer_destroy(buffer);
    return glyphCount;
}

// 141
uint32_t faceAxes(reef_ui_face* face, reef_ui_face_axis* axes, uint32_t capacity)
{
    if (!face)
        return 0;
    std::lock_guard lock(mutex);
    for (uint32_t i = 0; i < face->axes.size() && i < capacity && axes; i++)
        axes[i] = face->axes[i];
    return uint32_t(face->axes.size());
}

// 142
uint32_t faceCoordinates(reef_ui_face* face, uint32_t* tags, double* values, uint32_t capacity)
{
    if (!face)
        return 0;
    std::lock_guard lock(mutex);
    std::vector<FT_Fixed> coordinates(face->axes.size());
    if (face->axes.empty() || FT_Get_Var_Design_Coordinates(face->ftFace, FT_UInt(coordinates.size()), coordinates.data()))
        return 0;
    for (uint32_t i = 0; i < face->axes.size() && i < capacity; i++) {
        if (tags)
            tags[i] = face->axes[i].tag;
        if (values)
            values[i] = coordinates[i] / 65536.0;
    }
    return uint32_t(face->axes.size());
}

// 143
reef_ui_face* faceWithVariations(reef_ui_face* face, const uint32_t* tags, const double* values, uint32_t count)
{
    if (!face)
        return nullptr;
    std::lock_guard lock(mutex);
    FaceKey key = face->key;
    key.variations.clear();
    for (uint32_t i = 0; i < count; i++)
        key.variations.emplace_back(tags[i], values[i]);
    std::sort(key.variations.begin(), key.variations.end());
    return loadFace(key, face->data);
}

// 144
int fontRegister(const char* path)
{
    if (!path)
        return 1;
    std::lock_guard lock(mutex);
    if (std::find(applicationFonts.begin(), applicationFonts.end(), path) != applicationFonts.end())
        return 0;
    // The toolkit sets up its own fontconfig first.
    if (const ui::Application* application = ui::Application::instance())
        application->text();
    if (!FcConfigAppFontAddFile(Config(), reinterpret_cast<const FcChar8*>(path)))
        return 1;
    applicationFonts.push_back(path);
    matches.clear();
    lists.clear();
    return 0;
}

// 145
int fontUnregister(const char* path)
{
    if (!path)
        return 1;
    std::lock_guard lock(mutex);
    const auto it = std::find(applicationFonts.begin(), applicationFonts.end(), path);
    if (it == applicationFonts.end())
        return 1;
    applicationFonts.erase(it);
    const Config config;
    FcConfigAppFontClear(config);
    for (const std::string& file : applicationFonts)
        FcConfigAppFontAddFile(config, reinterpret_cast<const FcChar8*>(file.c_str()));
    matches.clear();
    lists.clear();
    return 0;
}

double madeObliqueSlant()
{
    return obliqueSlant;
}

double madeBoldStrength()
{
    return boldStrength;
}

double faceStretch(reef_ui_face* face)
{
    return face ? face->key.stretch : 1;
}

static bool hasTable(hb_face_t* face, hb_tag_t tag)
{
    hb_blob_t* table = hb_face_reference_table(face, tag);
    const bool has = hb_blob_get_length(table) > 0;
    hb_blob_destroy(table);
    return has;
}

OutlineFormat faceOutlineFormat(reef_ui_face* face)
{
    if (!face)
        return OutlinesNone;
    std::lock_guard lock(mutex);
    if ((face->metrics.flags & (FaceScalable | FaceColor)) != FaceScalable)
        return OutlinesNone;
    if (hasTable(face->hbFace, HB_TAG('g', 'l', 'y', 'f')))
        return OutlinesTrueType;
    return hasTable(face->hbFace, HB_TAG('C', 'F', 'F', ' ')) ? OutlinesCff : OutlinesNone;
}

std::map<uint32_t, uint32_t> faceCharacters(reef_ui_face* face)
{
    std::map<uint32_t, uint32_t> characters;
    if (!face)
        return characters;
    std::lock_guard lock(mutex);
    hb_map_t* mapping = hb_map_create();
    hb_set_t* covered = hb_set_create();
    hb_face_collect_nominal_glyph_mapping(face->hbFace, mapping, covered);
    hb_codepoint_t character = HB_SET_VALUE_INVALID;
    while (hb_set_next(covered, &character)) {
        const hb_codepoint_t glyph = hb_map_get(mapping, character);
        if (glyph != HB_MAP_VALUE_INVALID)
            characters.emplace(glyph, character);
    }
    hb_set_destroy(covered);
    hb_map_destroy(mapping);
    return characters;
}

std::string faceSubset(reef_ui_face* face, const std::vector<uint32_t>& glyphs, OutlineFormat* format)
{
    *format = OutlinesNone;
    if (!face)
        return {};
    std::lock_guard lock(mutex);
    hb_face_t* source = face->hbFace;
    OutlineFormat found = OutlinesTrueType;
    if (!hasTable(source, HB_TAG('g', 'l', 'y', 'f'))) {
        if (!hasTable(source, HB_TAG('C', 'F', 'F', ' ')))
            return {};
        found = OutlinesCff;
    }
    hb_subset_input_t* input = hb_subset_input_create_or_fail();
    if (!input)
        return {};
    hb_set_t* kept = hb_subset_input_glyph_set(input);
    hb_set_add(kept, 0);
    for (uint32_t glyph : glyphs)
        hb_set_add(kept, glyph);
    hb_subset_input_set_flags(input, HB_SUBSET_FLAGS_NO_HINTING | HB_SUBSET_FLAGS_RETAIN_GIDS | HB_SUBSET_FLAGS_DESUBROUTINIZE
                                             | HB_SUBSET_FLAGS_NOTDEF_OUTLINE);
    hb_set_t* dropped = hb_subset_input_set(input, HB_SUBSET_SETS_DROP_TABLE_TAG);
    for (hb_tag_t tag :
         {HB_TAG('G', 'S', 'U', 'B'), HB_TAG('G', 'P', 'O', 'S'), HB_TAG('G', 'D', 'E', 'F'), HB_TAG('k', 'e', 'r', 'n'),
          HB_TAG('B', 'A', 'S', 'E'), HB_TAG('J', 'S', 'T', 'F'), HB_TAG('M', 'A', 'T', 'H'), HB_TAG('D', 'S', 'I', 'G'),
          HB_TAG('V', 'O', 'R', 'G'), HB_TAG('S', 'T', 'A', 'T'), HB_TAG('m', 'o', 'r', 'x'), HB_TAG('k', 'e', 'r', 'x')})
        hb_set_add(dropped, tag);
    if (!face->axes.empty()) {
        std::vector<FT_Fixed> coordinates(face->axes.size());
        if (!FT_Get_Var_Design_Coordinates(face->ftFace, FT_UInt(coordinates.size()), coordinates.data())) {
            for (size_t i = 0; i < face->axes.size(); i++)
                hb_subset_input_pin_axis_location(input, source, face->axes[i].tag, float(coordinates[i] / 65536.0));
        }
    }
    hb_face_t* subset = hb_subset_or_fail(source, input);
    hb_subset_input_destroy(input);
    if (!subset)
        return {};
    hb_blob_t* blob = hb_face_reference_blob(subset);
    unsigned length = 0;
    const char* data = hb_blob_get_data(blob, &length);
    std::string bytes(data, data ? length : 0);
    hb_blob_destroy(blob);
    if (found == OutlinesCff && !hasTable(subset, HB_TAG('C', 'F', 'F', ' ')))
        bytes.clear();
    hb_face_destroy(subset);
    if (!bytes.empty())
        *format = found;
    return bytes;
}

std::vector<reef_ui_feature> parseFeatures(const char* text)
{
    std::vector<reef_ui_feature> features;
    if (!text)
        return features;
    const std::string list = text;
    size_t start = 0;
    while (start <= list.size()) {
        size_t end = list.find(',', start);
        if (end == std::string::npos)
            end = list.size();
        const std::string item = list.substr(start, end - start);
        hb_feature_t feature;
        if (!item.empty() && hb_feature_from_string(item.data(), int(item.size()), &feature))
            features.push_back({feature.tag, feature.value, feature.start, feature.end});
        start = end + 1;
    }
    return features;
}

static uint32_t blend(uint32_t below, uint32_t alpha, uint32_t red, uint32_t green, uint32_t blue)
{
    const uint32_t inverse = 255 - alpha;
    const auto channel = [&](int shift, uint32_t value) {
        return std::min<uint32_t>(value + ((below >> shift & 0xff) * inverse + 127) / 255, 255) << shift;
    };
    return channel(24, alpha) | channel(16, red) | channel(8, green) | channel(0, blue);
}

static void drawRun(ui::Canvas2D& canvas, const std::vector<const PlacedGlyph*>& glyphs, const ui::ColorF& color)
{
    if (glyphs.empty())
        return;
    int left = INT_MAX;
    int top = INT_MAX;
    int right = INT_MIN;
    int bottom = INT_MIN;
    for (const PlacedGlyph* glyph : glyphs) {
        left = std::min(left, glyph->x);
        top = std::min(top, glyph->y);
        right = std::max(right, glyph->x + glyph->mask->width);
        bottom = std::max(bottom, glyph->y + glyph->mask->height);
    }
    if (left >= right || top >= bottom)
        return;
    const int width = right - left;
    const int height = bottom - top;
    const ui::RectF rect = {double(left), double(top), double(width), double(height)};
    RunKey key;
    key.glyphs.reserve(glyphs.size());
    for (const PlacedGlyph* glyph : glyphs)
        key.glyphs.emplace_back(glyph->mask, glyph->x - left, glyph->y - top);
    key.color = {color.red, color.green, color.blue, color.alpha};
    const auto it = runs.find(key);
    if (it != runs.end()) {
        runOrder.splice(runOrder.begin(), runOrder, it->second.position);
        canvas.drawBitmap(it->second.bitmap, rect);
        return;
    }
    std::vector<uint32_t> pixels(size_t(width) * height, 0);
    const double alpha = std::clamp(color.alpha, 0.0, 1.0);
    const double red = std::clamp(color.red, 0.0, 1.0);
    const double green = std::clamp(color.green, 0.0, 1.0);
    const double blue = std::clamp(color.blue, 0.0, 1.0);
    for (const PlacedGlyph* glyph : glyphs) {
        const Mask& mask = *glyph->mask;
        for (int y = 0; y < mask.height; y++) {
            uint32_t* out = pixels.data() + size_t(glyph->y - top + y) * width + (glyph->x - left);
            if (mask.color) {
                const uint8_t* in = mask.pixels.data() + size_t(mask.width) * y * 4;
                for (int x = 0; x < mask.width; x++) {
                    const uint8_t* pixel = in + x * 4;
                    if (pixel[3])
                        out[x] = blend(out[x], pixel[3], pixel[2], pixel[1], pixel[0]);
                }
            } else {
                const uint8_t* in = mask.pixels.data() + size_t(mask.width) * y;
                for (int x = 0; x < mask.width; x++) {
                    if (!in[x])
                        continue;
                    const double coverage = in[x] * alpha / 255;
                    out[x] = blend(out[x], uint32_t(std::lround(coverage * 255)), uint32_t(std::lround(coverage * red * 255)),
                                   uint32_t(std::lround(coverage * green * 255)), uint32_t(std::lround(coverage * blue * 255)));
                }
            }
        }
    }
    const ui::RetainedBitmap bitmap(
            ui::Bitmap::wrap(pixels.data(), width, height, size_t(width) * 4, ui::Bitmap::Format::Argb32Premultiplied));
    canvas.drawBitmap(bitmap, rect);
    if (bitmap.isNull() || bitmap.byteCount() > largestRun)
        return;
    while (runBytes + bitmap.byteCount() > mostRunBytes && !runOrder.empty()) {
        const auto last = runs.find(*runOrder.back());
        runBytes -= last->second.bitmap.byteCount();
        runOrder.pop_back();
        runs.erase(last);
    }
    const auto inserted = runs.emplace(std::move(key), Run{bitmap, {}}).first;
    runOrder.push_front(&inserted->first);
    inserted->second.position = runOrder.begin();
    runBytes += bitmap.byteCount();
}

static void drawOutlines(ui::Canvas2D& canvas, reef_ui_face* face, const double* matrix, const uint32_t* glyphs, const double* positions,
                         uint32_t count, bool aliased)
{
    ui::Path path;
    std::vector<reef_ui_path_element> elements(64);
    const double unit = 1 / face->metrics.unitsPerEm;
    for (uint32_t i = 0; i < count; i++) {
        uint32_t size = faceOutline(face, glyphs[i], elements.data(), uint32_t(elements.size()));
        if (size > elements.size()) {
            elements.resize(size);
            size = faceOutline(face, glyphs[i], elements.data(), size);
        }
        const ui::PointF origin = {positions[i * 2], positions[i * 2 + 1]};
        for (uint32_t j = 0; j < size; j++) {
            const double* points = elements[j].points;
            const auto point = [&](int index) {
                const double x = points[index * 2] * unit;
                const double y = points[index * 2 + 1] * unit;
                return ui::PointF{origin.x + x * matrix[0] + y * matrix[2], origin.y + x * matrix[1] + y * matrix[3]};
            };
            switch (elements[j].type) {
            case 0:
                path.moveTo(point(0));
                break;
            case 1:
                path.lineTo(point(0));
                break;
            case 2:
                path.quadTo(point(0), point(1));
                break;
            case 3:
                path.cubicTo(point(0), point(1), point(2));
                break;
            case 4:
                path.close();
                break;
            }
        }
    }
    canvas.save();
    canvas.setAntialias(!aliased);
    canvas.fillPath(path);
    canvas.restore();
}

// Sets the size of a glyph in pixels; false when the font does not have it.
static bool setPixelSize(reef_ui_face* face, double width, double height, double* scale)
{
    const FT_Face ftFace = face->ftFace;
    if (FT_IS_SCALABLE(ftFace) || ftFace->num_fixed_sizes <= 0) {
        return !FT_Set_Char_Size(ftFace, std::lround(width * face->key.stretch * 64), std::lround(height * 64), 72, 72);
    }
    int best = 0;
    for (int i = 1; i < ftFace->num_fixed_sizes; i++) {
        const double current = ftFace->available_sizes[best].y_ppem / 64.0;
        const double other = ftFace->available_sizes[i].y_ppem / 64.0;
        if ((current < height && other > current) || (other >= height && other < current))
            best = i;
    }
    if (FT_Select_Size(ftFace, best))
        return false;
    *scale = height / (ftFace->available_sizes[best].y_ppem / 64.0);
    return true;
}

static const uint8_t* bitmapRow(const FT_Bitmap& bitmap, int y)
{
    return bitmap.pitch < 0 ? bitmap.buffer + ptrdiff_t(int(bitmap.rows) - 1 - y) * -bitmap.pitch
                            : bitmap.buffer + ptrdiff_t(y) * bitmap.pitch;
}

static bool renderMask(reef_ui_face* face, const MaskKey& key, double width, double height, int quarter, bool aliased, Mask& mask)
{
    if (!setPixelSize(face, width, height, &mask.scale))
        return false;
    const FT_Face ftFace = face->ftFace;
    FT_Int32 loadFlags = aliased ? FT_LOAD_TARGET_MONO : FT_LOAD_TARGET_LIGHT;
    if (FT_HAS_COLOR(ftFace))
        loadFlags |= FT_LOAD_COLOR;
    else if (FT_IS_SCALABLE(ftFace))
        loadFlags |= FT_LOAD_NO_BITMAP;
    if (FT_Load_Glyph(ftFace, key.glyph, loadFlags))
        return false;
    const FT_GlyphSlot slot = ftFace->glyph;
    if (slot->format == FT_GLYPH_FORMAT_OUTLINE) {
        if (face->key.bold) {
            const FT_Pos strength = std::lround(width * boldStrength * 64);
            FT_Outline_EmboldenXY(&slot->outline, strength, strength);
            FT_Outline_Translate(&slot->outline, -(strength / 2), -(strength / 2));
        }
        if (face->key.oblique) {
            const FT_Matrix shear = {0x10000, FT_Fixed(obliqueSlant * 0x10000), 0, 0x10000};
            FT_Outline_Transform(&slot->outline, &shear);
        }
        FT_Outline_Translate(&slot->outline, quarter * 16, 0);
        if (FT_Render_Glyph(slot, aliased ? FT_RENDER_MODE_MONO : FT_RENDER_MODE_NORMAL))
            return false;
    }
    const FT_Bitmap& bitmap = slot->bitmap;
    mask.left = slot->bitmap_left;
    mask.top = slot->bitmap_top;
    mask.width = int(bitmap.width);
    mask.height = int(bitmap.rows);
    if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY) {
        mask.pixels.resize(size_t(mask.width) * mask.height);
        for (int y = 0; y < mask.height; y++)
            memcpy(mask.pixels.data() + size_t(y) * mask.width, bitmapRow(bitmap, y), size_t(mask.width));
    } else if (bitmap.pixel_mode == FT_PIXEL_MODE_BGRA) {
        mask.color = true;
        mask.pixels.resize(size_t(mask.width) * mask.height * 4);
        for (int y = 0; y < mask.height; y++)
            memcpy(mask.pixels.data() + size_t(y) * mask.width * 4, bitmapRow(bitmap, y), size_t(mask.width) * 4);
    } else if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
        mask.pixels.resize(size_t(mask.width) * mask.height);
        for (int y = 0; y < mask.height; y++) {
            const uint8_t* row = bitmapRow(bitmap, y);
            for (int x = 0; x < mask.width; x++)
                mask.pixels[size_t(y) * mask.width + x] = row[x >> 3] & 0x80 >> (x & 7) ? 255 : 0;
        }
    } else {
        mask.width = 0;
        mask.height = 0;
    }
    return true;
}

void drawGlyphs(ui::Canvas2D& canvas, reef_ui_face* face, const double* matrix, const uint32_t* glyphs, const double* positions,
                uint32_t count, int flags)
{
    if (!face || count == 0 || !matrix || !glyphs || !positions)
        return;
    std::lock_guard lock(mutex);
    const base::AffineTransform& transform = canvas.transform();
    const double scale = std::sqrt(std::abs(transform.determinant()));
    if (scale < 1e-9)
        return;
    // Device pixels for a unit of the transform's own.
    const double ratio = canvas.deviceScale() / scale;
    const double xx = (matrix[0] * transform.m11() + matrix[1] * transform.m21()) * ratio;
    const double xy = (matrix[0] * transform.m12() + matrix[1] * transform.m22()) * ratio;
    const double yx = (matrix[2] * transform.m11() + matrix[3] * transform.m21()) * ratio;
    const double yy = (matrix[2] * transform.m12() + matrix[3] * transform.m22()) * ratio;
    const bool upright = std::abs(yx) < std::abs(xx) * 1e-6 && std::abs(xy) < std::abs(yy) * 1e-6 && xx > 0 && yy < 0;
    const bool aliased = flags & GlyphsAliased;
    if (FT_IS_SCALABLE(face->ftFace) && (!upright || flags & GlyphsAsPaths || xx > 256 || yy < -256)) {
        drawOutlines(canvas, face, matrix, glyphs, positions, count, aliased);
        return;
    }
    if (!upright)
        return;
    if (maskBytes > mostMaskBytes) {
        clearRuns();
        masks.clear();
        maskBytes = 0;
    }
    std::vector<PlacedGlyph> placed;
    placed.reserve(count);
    int64_t left = INT_MAX;
    int64_t top = INT_MAX;
    int64_t right = INT_MIN;
    int64_t bottom = INT_MIN;
    for (uint32_t i = 0; i < count; i++) {
        double x, y;
        transform.map(positions[i * 2], positions[i * 2 + 1], &x, &y);
        const double deviceX = x * ratio;
        const double pixelX = std::floor(deviceX);
        const int quarter = std::min(int((deviceX - pixelX) * 4), 3);
        const MaskKey key = {face, glyphs[i], int(std::lround(xx * 64)), int(std::lround(-yy * 64)), (aliased ? 8 : 0) | quarter};
        auto it = masks.find(key);
        if (it == masks.end()) {
            Mask mask;
            if (!renderMask(face, key, xx, -yy, quarter, aliased, mask))
                continue;
            maskBytes += sizeof mask + mask.pixels.size();
            it = masks.emplace(key, std::move(mask)).first;
        }
        const Mask& mask = it->second;
        if (mask.width <= 0 || mask.height <= 0)
            continue;
        const double deviceY = y * ratio;
        const PlacedGlyph glyph = {&mask, int(pixelX) + mask.left, int(std::lround(deviceY)) - mask.top, deviceX, deviceY};
        placed.push_back(glyph);
        if (mask.scale == 1) {
            left = std::min<int64_t>(left, glyph.x);
            top = std::min<int64_t>(top, glyph.y);
            right = std::max<int64_t>(right, glyph.x + mask.width);
            bottom = std::max<int64_t>(bottom, glyph.y + mask.height);
        }
    }
    if (placed.empty())
        return;
    const ui::ColorF color = canvas.fillColor();
    canvas.save();
    canvas.setTransform(base::AffineTransform::fromScale(1 / ratio, 1 / ratio));
    canvas.setImageSmoothing(ui::Canvas2D::Smoothing::None);
    std::vector<const PlacedGlyph*> run;
    for (const PlacedGlyph& glyph : placed) {
        if (glyph.mask->scale == 1)
            run.push_back(&glyph);
    }
    if (!run.empty()) {
        if ((right - left) * (bottom - top) <= 4 << 20) {
            drawRun(canvas, run, color);
        } else {
            for (const PlacedGlyph* glyph : run)
                drawRun(canvas, {glyph}, color);
        }
    }
    canvas.setImageSmoothing(ui::Canvas2D::Smoothing::High);
    for (const PlacedGlyph& glyph : placed) {
        const Mask& mask = *glyph.mask;
        if (mask.scale == 1 || !mask.color)
            continue;
        std::vector<uint32_t> pixels(size_t(mask.width) * mask.height);
        memcpy(pixels.data(), mask.pixels.data(), pixels.size() * sizeof(uint32_t));
        const ui::Bitmap bitmap = ui::Bitmap::wrap(pixels.data(), mask.width, mask.height, size_t(mask.width) * 4,
                                                   ui::Bitmap::Format::Argb32Premultiplied);
        canvas.drawBitmap(bitmap, {glyph.deviceX + mask.left * mask.scale, glyph.deviceY - mask.top * mask.scale,
                                   mask.width * mask.scale, mask.height * mask.scale});
    }
    canvas.restore();
}

} // namespace cutefish::reef

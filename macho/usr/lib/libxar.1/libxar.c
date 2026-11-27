// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libxar: Reef's own reading and writing of xar archives (a small XML tree for the table of contents, files kept in memory until xar_close).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <stdint.h>

ssize_t pread(int, void *, size_t, off_t);

// zlib, bzip2, CommonCrypto and libcompression, as far as they are used.
int compress2(unsigned char *dest, unsigned long *destLen, const unsigned char *source, unsigned long sourceLen, int level);
unsigned long compressBound(unsigned long sourceLen);
int uncompress(unsigned char *dest, unsigned long *destLen, const unsigned char *source, unsigned long sourceLen);
int BZ2_bzBuffToBuffCompress(char *dest, unsigned int *destLen, char *source, unsigned int sourceLen, int blockSize100k, int verbosity, int workFactor);
int BZ2_bzBuffToBuffDecompress(char *dest, unsigned int *destLen, char *source, unsigned int sourceLen, int small, int verbosity);
unsigned char *CC_MD5(const void *data, uint32_t length, unsigned char *md);
unsigned char *CC_SHA1(const void *data, uint32_t length, unsigned char *md);
unsigned char *CC_SHA256(const void *data, uint32_t length, unsigned char *md);
unsigned char *CC_SHA512(const void *data, uint32_t length, unsigned char *md);
size_t compression_encode_buffer(uint8_t *dst, size_t dstSize, const uint8_t *src, size_t srcSize, void *scratch, int algorithm);
size_t compression_decode_buffer(uint8_t *dst, size_t dstSize, const uint8_t *src, size_t srcSize, void *scratch, int algorithm);
enum { COMPRESSION_LZMA = 0x306 };

// A node of the table of contents: name, attributes, text and children.
struct Attribute {
    char *name;
    char *value;
};

typedef struct Node {
    char *name;
    struct Attribute *attributes;
    size_t attributeCount;
    char *text;
    struct Node **children;
    size_t childCount;
    struct Node *parent;
} Node;

typedef struct File {
    Node *node;
    struct File *parent;
    struct File **children;
    size_t childCount;
    struct xar *archive;
    char *safePath;
} File;

typedef struct Subdoc {
    Node *node;
    struct xar *archive;
} Subdoc;

typedef struct xar {
    char *path;
    char writable;
    int fd;
    size_t heapStart;
    Node *document;
    Node *toc;
    File **files;
    size_t fileCount;
    Subdoc **subdocs;
    size_t subdocCount;
    unsigned nextID;
    char *compression;
    char *tocChecksum;
    char *fileChecksum;
    unsigned char *heap;
    size_t heapLength;
    size_t heapCapacity;
} *xar_t;

typedef struct Iterator {
    void *unused0;
    size_t unused8;
    void *unused10;
    File *current;
    xar_t archive;
    size_t unused28;
} *xar_iter_t;

struct __attribute__((packed)) Header {
    uint32_t magic;
    uint16_t size;
    uint16_t version;
    uint64_t tocLength;
    uint64_t tocUncompressedLength;
    uint32_t checksumAlgorithm;
};

// A growing output buffer.
typedef struct Buffer {
    unsigned char *data;
    size_t length;
    size_t capacity;
} Buffer;

static void put(Buffer *b, const void *bytes, size_t n)
{
    if (b->capacity < b->length + n + 1) {
        b->capacity = (b->length + n + 1) * 2;
        b->data = realloc(b->data, b->capacity);
    }
    memcpy(b->data + b->length, bytes, n);
    b->length += n;
    b->data[b->length] = 0;
}

static void putByte(Buffer *b, unsigned char c)
{
    put(b, &c, 1);
}

static Node *newNode(const char *name, Node *parent)
{
    Node *node = calloc(1, sizeof(Node));
    node->name = strdup(name);
    node->parent = parent;
    if (parent) {
        parent->children = realloc(parent->children, parent->childCount * sizeof(Node *) + sizeof(Node *));
        parent->children[parent->childCount++] = node;
    }
    return node;
}

static Node *childNamed(Node *node, const char *name)
{
    if (node)
        for (size_t i = 0; i < node->childCount; i++)
            if (strcmp(node->children[i]->name, name) == 0)
                return node->children[i];
    return NULL;
}

static char *attributeOf(Node *node, const char *name)
{
    if (!node)
        return NULL;
    for (size_t i = 0; i < node->attributeCount; i++)
        if (strcmp(node->attributes[i].name, name) == 0)
            return node->attributes[i].value;
    return NULL;
}

// The node at a path of names separated by slashes; created on the way when asked.
static Node *nodeAtPath(Node *node, const char *path, _Bool create)
{
    char *copy = strdup(path);
    char *part = strtok(copy, "/");
    if (!(part == NULL || node == NULL)) {
        do {
            Node *next = NULL;
            for (size_t i = 0; i < node->childCount; i++)
                if (strcmp(node->children[i]->name, part) == 0) {
                    next = node->children[i];
                    break;
                }
            if (!next && create)
                next = newNode(part, node);
            node = next;
            part = strtok(NULL, "/");
        } while (node != NULL && part != NULL);
    }
    free(copy);
    return node;
}

static void digest(const char *style, const void *data, size_t length, unsigned char *md, size_t *mdLength)
{
    if (strcmp(style, "md5") == 0) {
        CC_MD5(data, length, md);
        *mdLength = 16;
    } else if (strcmp(style, "sha256") == 0) {
        CC_SHA256(data, length, md);
        *mdLength = 32;
    } else if (strcmp(style, "sha512") == 0) {
        CC_SHA512(data, length, md);
        *mdLength = 64;
    } else if (strcmp(style, "none") == 0) {
        *mdLength = 0;
    } else {
        CC_SHA1(data, length, md);
        *mdLength = 20;
    }
}

static void freeNode(Node *node)
{
    if (!node)
        return;
    for (size_t i = 0; i < node->childCount; i++)
        freeNode(node->children[i]);
    for (size_t i = 0; i < node->attributeCount * 2; i++)
        free(((char **)node->attributes)[i]);
    free(node->attributes);
    free(node->children);
    free(node->name);
    free(node->text);
    free(node);
}

static void setAttribute(Node *node, const char *name, const char *value)
{
    for (size_t i = 0; i < node->attributeCount; i++)
        if (strcmp(node->attributes[i].name, name) == 0) {
            free(node->attributes[i].value);
            node->attributes[i].value = strdup(value);
            return;
        }
    node->attributes = realloc(node->attributes, node->attributeCount * sizeof(struct Attribute) + sizeof(struct Attribute));
    node->attributes[node->attributeCount].name = strdup(name);
    node->attributes[node->attributeCount].value = strdup(value);
    node->attributeCount++;
}

static char *unescape(const char *s, size_t n);

static void appendText(Node *node, const char *text, size_t length)
{
    size_t old = node->text ? strlen(node->text) : 0;
    node->text = realloc(node->text, old + length + 1);
    memcpy(node->text + old, text, length);
    node->text[old + length] = 0;
}

// Builds the tree of an XML document (comments, declarations and processing instructions are skipped).
static Node *parseXML(const char *s, size_t n)
{
    Node *document = calloc(1, sizeof(Node));
    document->name = strdup("#document");
    Node *current = document;
    size_t i = 0;
    while (i < n) {
        if (s[i] != '<') {
            const char *lt = memchr(s + i, '<', n - i);
            size_t length = lt ? (size_t)(lt - (s + i)) : n - i;
            char *text = unescape(s + i, length);
            appendText(current, text, strlen(text));
            free(text);
            i += length;
            continue;
        }
        if (i + 4 <= n && strncmp(s + i, "<!--", 4) == 0) {
            const char *end = strstr(s + i + 4, "-->");
            i = end ? (size_t)(end + 3 - s) : n;
            continue;
        }
        if (i + 9 <= n && strncmp(s + i, "<![CDATA[", 9) == 0) {
            const char *start = s + i + 9;
            const char *end = strstr(start, "]]>");
            size_t length = end ? (size_t)(end - start) : n - i - 9;
            appendText(current, start, length);
            i = end ? (size_t)(end + 3 - s) : n;
            continue;
        }
        if (s[i + 1] == '!' || s[i + 1] == '?') {
            const char *end = memchr(s + i, '>', n - i);
            i = end ? (size_t)(end + 1 - s) : n;
            continue;
        }
        if (s[i + 1] == '/') {
            const char *end = memchr(s + i, '>', n - i);
            current = current->parent ? current->parent : current;
            i = end ? (size_t)(end + 1 - s) : n;
            continue;
        }
        size_t p = i + 1;
        while (p < n && !(s[p] == ' ' || s[p] == '\t' || s[p] == '\n' || s[p] == '\r' || s[p] == '/' || s[p] == '>'))
            p++;
        char *name = strndup(s + i + 1, p - i - 1);
        Node *node = newNode(name, current);
        free(name);
        if (p >= n) {
            i = p + 1;
            continue;
        }
        int selfClosing = 0;
        while (p < n) {
            unsigned char c = s[p];
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                p++;
            } else if (c == '/') {
                selfClosing = 1;
                p++;
            } else if (c == '>') {
                break;
            } else {
                size_t start = p;
                while (p < n && !(s[p] == ' ' || s[p] == '\t' || s[p] == '\n' || s[p] == '\r' || s[p] == '=' || s[p] == '>'))
                    p++;
                char *attribute = strndup(s + start, p - start);
                char *value;
                while (p < n && (s[p] == ' ' || s[p] == '\t' || s[p] == '\n' || s[p] == '\r' || s[p] == '='))
                    p++;
                if (p < n && (s[p] == '"' || s[p] == '\'')) {
                    unsigned char quote = s[p];
                    const char *end = memchr(s + p + 1, quote, n - p - 1);
                    value = unescape(s + p + 1, end ? (size_t)(end - (s + p + 1)) : 0);
                    p = end ? (size_t)(end + 1 - s) : n;
                } else {
                    value = strdup("");
                }
                node->attributes = realloc(node->attributes, node->attributeCount * sizeof(struct Attribute) + sizeof(struct Attribute));
                node->attributes[node->attributeCount].name = attribute;
                node->attributes[node->attributeCount].value = value;
                node->attributeCount++;
            }
        }
        if (!selfClosing)
            current = node;
        i = p + 1;
    }
    return document;
}

// The files of the table of contents: a File for each <file> element, nested as the elements are.
static void collectFiles(xar_t x, Node *node, File *parent)
{
    for (size_t i = 0; i < node->childCount; i++) {
        Node *child = node->children[i];
        if (strcmp(child->name, "file") != 0)
            continue;
        File *file = calloc(1, sizeof(File));
        file->node = child;
        file->parent = parent;
        file->archive = x;
        File ***list = parent ? &parent->children : &x->files;
        size_t *count = parent ? &parent->childCount : &x->fileCount;
        *list = realloc(*list, *count * sizeof(File *) + sizeof(File *));
        (*list)[(*count)++] = file;
        for (size_t k = 0; k < child->attributeCount; k++) {
            if (strcmp(child->attributes[k].name, "id") == 0) {
                if (child->attributes[k].value) {
                    unsigned id = atoi(child->attributes[k].value);
                    if (x->nextID <= id)
                        x->nextID = id + 1;
                }
                break;
            }
        }
        collectFiles(x, child, file);
    }
}

xar_t xar_open(const char *path, int flags)
{
    if (!path)
        return NULL;
    xar_t x = calloc(1, sizeof(*x));
    char *pathCopy = strdup(path);
    x->path = pathCopy;
    x->writable = flags == 1;
    char *compression = strdup("gzip");
    x->compression = compression;
    char *tocChecksum = strdup("sha1");
    x->tocChecksum = tocChecksum;
    char *fileChecksum = strdup("sha1");
    x->fileChecksum = fileChecksum;
    x->nextID = 1;
    Node *document = NULL;
    if (flags == 1) {
        x->fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
        if (x->fd >= 0) {
            document = newNode("#document", NULL);
            x->document = document;
            x->toc = newNode("toc", newNode("xar", document));
            return x;
        }
    } else {
        x->fd = open(path, O_RDONLY | O_CLOEXEC);
        if (x->fd >= 0) {
            int fd = x->fd;
            struct Header header;
            if (pread(fd, &header, sizeof header, 0) == sizeof header && header.magic == 0x21726178) {
                size_t headerSize = __builtin_bswap16(header.size);
                size_t compressed, uncompressed;
                if (headerSize > 0x1b && (compressed = __builtin_bswap64(header.tocLength)) < 0x4000001 &&
                    (uncompressed = __builtin_bswap64(header.tocUncompressedLength)) < 0x10000001) {
                    unsigned char *packed = malloc(compressed);
                    if (pread(fd, packed, compressed, headerSize) != (ssize_t)compressed) {
                        free(packed);
                        goto fail;
                    }
                    unsigned char *toc = malloc(uncompressed + 1);
                    unsigned long tocLength = uncompressed;
                    if (uncompress(toc, &tocLength, packed, compressed) != 0) {
                        free(packed);
                        free(toc);
                        goto fail;
                    }
                    toc[tocLength] = 0;
                    x->heapStart = headerSize + compressed;
                    document = parseXML((char *)toc, tocLength);
                    x->document = document;
                    free(toc);
                    Node *xarNode = childNamed(document, "xar");
                    Node *tocNode = childNamed(xarNode, "toc");
                    x->toc = tocNode;
                    if (tocNode) {
                        Node *checksum = childNamed(tocNode, "checksum");
                        if (checksum) {
                            char *style = attributeOf(checksum, "style");
                            Node *offsetNode = nodeAtPath(checksum, "offset", 0);
                            const char *offsetText = offsetNode ? offsetNode->text : NULL;
                            unsigned long long offset = strtoull(offsetText ? offsetText : "0", NULL, 10);
                            Node *sizeNode = nodeAtPath(checksum, "size", 0);
                            const char *sizeText = sizeNode ? sizeNode->text : NULL;
                            unsigned long long size = strtoull(sizeText ? sizeText : "0", NULL, 10);
                            size_t length = 0;
                            unsigned char computed[64], stored[64];
                            digest(style ? style : "sha1", packed, compressed, computed, &length);
                            if (size - 1 < 0x40 && (pread(fd, stored, size, x->heapStart + offset) != (ssize_t)size || size != length || memcmp(stored, computed, size) != 0)) {
                                free(packed);
                                goto fail;
                            }
                        }
                        free(packed);
                        collectFiles(x, tocNode, NULL);
                        if (xarNode)
                            for (size_t i = 0; i < xarNode->childCount; i++) {
                                Node *child = xarNode->children[i];
                                if (strcmp(child->name, "subdoc") == 0) {
                                    Subdoc *subdoc = calloc(1, sizeof(Subdoc));
                                    subdoc->node = child;
                                    subdoc->archive = x;
                                    x->subdocs = realloc(x->subdocs, x->subdocCount * sizeof(Subdoc *) + sizeof(Subdoc *));
                                    x->subdocs[x->subdocCount++] = subdoc;
                                }
                            }
                        return x;
                    }
                    free(packed);
                }
            }
        fail:
            close(fd);
        }
    }
    freeNode(document);
    free(pathCopy);
    free(compression);
    free(tocChecksum);
    free(fileChecksum);
    free(x);
    return NULL;
}

static void putEscaped(Buffer *b, const char *s, unsigned attribute);

// Adds an offset to the offset of every <data> element below a node.
static void shiftOffsets(Node *node, size_t shift)
{
    char text[32];
    for (size_t i = 0; i < node->childCount; i++) {
        Node *child = node->children[i];
        if (strcmp(child->name, "data") == 0) {
            for (size_t k = 0; k < child->childCount; k++) {
                Node *field = child->children[k];
                if (strcmp(field->name, "offset") == 0) {
                    if (field->text) {
                        unsigned long long offset = strtoull(field->text, NULL, 10);
                        snprintf(text, sizeof text, "%llu", shift + offset);
                        free(field->text);
                        field->text = strdup(text);
                    }
                    break;
                }
            }
        }
        shiftOffsets(child, shift);
    }
}

static void writeNode(Buffer *b, Node *node, int depth);

static _Bool writeAll(int fd, const void *data, size_t length)
{
    _Bool ok = 1;
    while (length) {
        ssize_t n = write(fd, data, length);
        if (n < 0) {
            if (errno != EINTR) {
                ok = 0;
                break;
            }
        } else if (n == 0) {
            ok = 0;
            break;
        } else {
            data = (const char *)data + n;
            length -= n;
        }
    }
    return ok;
}

static void freeFile(File *file)
{
    for (size_t i = 0; i < file->childCount; i++)
        freeFile(file->children[i]);
    free(file->children);
    free(file->safePath);
    free(file);
}

int xar_close(xar_t x)
{
    if (!x)
        return -1;
    int result = 0;
    if (x->writable) {
        char zero = 0;
        unsigned char md[64];
        size_t checksumLength = 0;
        digest(x->tocChecksum, &zero, 0, md, &checksumLength);
        shiftOffsets(x->toc, checksumLength);
        Node *toc = x->toc;
        Node *checksum = NULL;
        if (toc)
            for (size_t i = 0; i < toc->childCount; i++)
                if (strcmp(toc->children[i]->name, "checksum") == 0) {
                    checksum = toc->children[i];
                    break;
                }
        if (!checksum && checksumLength) {
            checksum = newNode("checksum", toc);
            memmove(x->toc->children + 1, x->toc->children, x->toc->childCount * 8 - 8);
            x->toc->children[0] = checksum;
        }
        if (checksum) {
            char text[32];
            setAttribute(checksum, "style", x->tocChecksum);
            Node *offset = nodeAtPath(checksum, "offset", 1);
            free(offset->text);
            offset->text = strdup("0");
            snprintf(text, sizeof text, "%zu", checksumLength);
            Node *size = nodeAtPath(checksum, "size", 1);
            free(size->text);
            size->text = strdup(text);
        }
        toc = x->toc;
        Node *created = NULL;
        if (toc)
            for (size_t i = 0; i < toc->childCount; i++)
                if (strcmp(toc->children[i]->name, "creation-time") == 0) {
                    created = toc->children[i];
                    break;
                }
        if (!created) {
            char text[32];
            struct tm tm;
            time_t now = time(NULL);
            gmtime_r(&now, &tm);
            strftime(text, sizeof text, "%Y-%m-%dT%H:%M:%S", &tm);
            created = newNode("creation-time", x->toc);
            free(created->text);
            created->text = strdup(text);
        }
        Buffer b;
        b.capacity = 0x50;
        b.data = malloc(0x50);
        memcpy(b.data, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", 40);
        b.length = 39;
        Node *xarNode = childNamed(x->document, "xar");
        writeNode(&b, xarNode, 0);
        unsigned long packedLength = compressBound(b.length);
        unsigned char *packed = malloc(packedLength);
        int status = compress2(packed, &packedLength, b.data, b.length, 9);
        struct Header header;
        header.magic = 0x21726178;
        header.size = __builtin_bswap16(0x1c);
        header.version = __builtin_bswap16(1);
        header.tocLength = __builtin_bswap64(packedLength);
        header.tocUncompressedLength = __builtin_bswap64(b.length);
        unsigned algorithm = 0;
        if (strcmp(x->tocChecksum, "none") != 0) {
            if (strcmp(x->tocChecksum, "md5") == 0)
                algorithm = 2;
            else
                algorithm = strcmp(x->tocChecksum, "sha1") != 0 ? 3 : 1;
        }
        header.checksumAlgorithm = __builtin_bswap32(algorithm);
        size_t tocChecksumLength = 0;
        digest(x->tocChecksum, packed, packedLength, md, &tocChecksumLength);
        result = -1;
        if (status == 0) {
            result = 0;
            if (!writeAll(x->fd, &header, sizeof header) || !writeAll(x->fd, packed, packedLength) ||
                !writeAll(x->fd, md, tocChecksumLength) || !writeAll(x->fd, x->heap, x->heapLength))
                result = -1;
        }
        free(packed);
        free(b.data);
        free(x->heap);
    }
    int closed = close(x->fd);
    for (size_t i = 0; i < x->fileCount; i++)
        freeFile(x->files[i]);
    free(x->files);
    for (size_t i = 0; i < x->subdocCount; i++)
        free(x->subdocs[i]);
    result = (closed != 0 ? -1 : 0) | result;
    free(x->subdocs);
    freeNode(x->document);
    free(x->path);
    free(x->compression);
    free(x->tocChecksum);
    free(x->fileChecksum);
    free(x);
    return result;
}

int xar_opt_set(xar_t x, const char *option, const char *value)
{
    if (!x || !option || !value)
        return -1;
    char **slot;
    if (strcmp(option, "compression") == 0)
        slot = &x->compression;
    else if (strcmp(option, "toc-cksum") == 0)
        slot = &x->tocChecksum;
    else if (strcmp(option, "file-chksum") == 0)
        slot = &x->fileChecksum;
    else
        return 0;
    free(*slot);
    *slot = strdup(value);
    return 0;
}

const char *xar_opt_get(xar_t x, const char *option)
{
    if (!x || !option)
        return NULL;
    if (strcmp(option, "compression") == 0)
        return x->compression;
    if (strcmp(option, "toc-cksum") == 0)
        return x->tocChecksum;
    if (strcmp(option, "file-chksum") == 0)
        return x->fileChecksum;
    return NULL;
}

static File *addEntry(xar_t x, File *parent, const char *name, const char *type, unsigned mode);
static void addData(xar_t x, File *file, const void *data, size_t length);

File *xar_add_frombuffer(xar_t x, File *parent, const char *name, const char *buffer, size_t length)
{
    File *file = NULL;
    if (x && name && x->writable) {
        file = addEntry(x, parent, name, "file", 0644);
        addData(x, file, buffer, length);
    }
    return file;
}

// A new <file> element with its id, name, type, mode, uid and gid.
static File *addEntry(xar_t x, File *parent, const char *name, const char *type, unsigned mode)
{
    char text[32];
    Node *parentNode = parent ? parent->node : x->toc;
    Node *node = newNode("file", parentNode);
    snprintf(text, sizeof text, "%u", x->nextID++);
    setAttribute(node, "id", text);
    Node *field = newNode("name", node);
    field->text = name ? strdup(name) : NULL;
    field = newNode("type", node);
    field->text = type ? strdup(type) : NULL;
    snprintf(text, sizeof text, "%04o", mode & 0xfff);
    field = newNode("mode", node);
    field->text = strdup(text);
    snprintf(text, sizeof text, "%u", getuid());
    field = newNode("uid", node);
    field->text = strdup(text);
    snprintf(text, sizeof text, "%u", getgid());
    field = newNode("gid", node);
    field->text = strdup(text);
    File *file = calloc(1, sizeof(File));
    file->node = node;
    file->parent = parent;
    File ***list = parent ? &parent->children : &x->files;
    size_t *count = parent ? &parent->childCount : &x->fileCount;
    file->archive = x;
    *list = realloc(*list, *count * sizeof(File *) + sizeof(File *));
    (*list)[(*count)++] = file;
    return file;
}

static void hex(const unsigned char *md, size_t length, char *out)
{
    for (size_t i = 0; i < length; i++, out += 2)
        sprintf(out, "%02x", md[i]);
    *out = 0;
}

// The data of a file: compressed as asked for, described by a <data> element, kept in the heap.
static void addData(xar_t x, File *file, const void *data, size_t length)
{
    const char *style = x->compression;
    const char *mime = NULL;
    size_t storedLength = 0;
    unsigned char *dest = NULL;
    int done = 0;
    if (length && strcmp(style, "gzip") == 0) {
        unsigned long bound = compressBound(length);
        dest = malloc(bound);
        if (compress2(dest, &bound, data, length, 6) == 0) {
            mime = "application/x-gzip";
            storedLength = bound;
            done = 1;
        }
    } else if (length && strcmp(style, "bzip2") == 0) {
        unsigned bound = (int)(length / 100) + (unsigned)length + 600;
        dest = malloc(bound);
        if (BZ2_bzBuffToBuffCompress((char *)dest, &bound, (char *)data, (unsigned)length, 9, 0, 0) == 0) {
            storedLength = bound;
            mime = "application/x-bzip2";
            done = 1;
        }
    } else if ((style[0] == 'x' && style[1] == 'z' && style[2] == 0 && length) || (strcmp(style, "lzma") == 0 && length)) {
        size_t capacity = length + (length >> 1) + 0x1000;
        dest = malloc(capacity);
        storedLength = compression_encode_buffer(dest, capacity, data, length, NULL, COMPRESSION_LZMA);
        mime = storedLength ? "application/x-xz" : "application/octet-stream";
        done = 1;
    }
    if (!done || storedLength == 0) {
        free(dest);
        mime = "application/octet-stream";
        storedLength = 0;
        dest = NULL;
    }
    const void *stored = dest ? (const void *)dest : data;
    if (!dest)
        storedLength = length;
    Node *node = newNode("data", file->node);
    char text[160];
    snprintf(text, sizeof text, "%zu", x->heapLength);
    Node *field = newNode("offset", node);
    field->text = strdup(text);
    snprintf(text, sizeof text, "%zu", storedLength);
    field = newNode("length", node);
    field->text = strdup(text);
    snprintf(text, sizeof text, "%zu", length);
    field = newNode("size", node);
    field->text = strdup(text);
    field = newNode("encoding", node);
    setAttribute(field, "style", mime);
    if (strcmp(x->fileChecksum, "none") != 0) {
        unsigned char md[64];
        size_t mdLength = 0;
        digest(x->fileChecksum, data, length, md, &mdLength);
        hex(md, mdLength, text);
        field = newNode("extracted-checksum", node);
        setAttribute(field, "style", x->fileChecksum);
        free(field->text);
        field->text = strdup(text);
        digest(x->fileChecksum, stored, storedLength, md, &mdLength);
        hex(md, mdLength, text);
        field = newNode("archived-checksum", node);
        setAttribute(field, "style", x->fileChecksum);
        free(field->text);
        field->text = strdup(text);
    }
    if (x->heapCapacity < x->heapLength + storedLength) {
        x->heapCapacity = (x->heapLength + storedLength) * 2 + 0x1000;
        x->heap = realloc(x->heap, x->heapCapacity);
    }
    memcpy(x->heap + x->heapLength, stored, storedLength);
    x->heapLength += storedLength;
    free(dest);
}

File *xar_add_frompath(xar_t x, File *parent, const char *name, const char *path)
{
    if (!x || !path || !name || !x->writable)
        return NULL;
    struct stat st;
    char target[4096];
    if (lstat(path, &st) != 0)
        return NULL;
    if ((st.st_mode & S_IFMT) == S_IFLNK) {
        ssize_t n = readlink(path, target, sizeof target - 1);
        if (n < 0)
            return NULL;
        target[n] = 0;
        File *file = addEntry(x, parent, name, "symlink", st.st_mode);
        Node *link = newNode("link", file->node);
        setAttribute(link, "type", "file");
        free(link->text);
        link->text = strdup(target);
        return file;
    }
    if ((st.st_mode & S_IFMT) == S_IFREG) {
        int fd = open(path, O_RDONLY | O_CLOEXEC);
        if (fd < 0)
            return NULL;
        char *buffer = malloc(st.st_size + 1);
        size_t done = 0;
        ssize_t n;
        do {
            if (done >= (size_t)st.st_size)
                break;
            n = pread(fd, buffer + done, st.st_size - done, done);
            done += n;
        } while (n > 0);
        close(fd);
        File *file = addEntry(x, parent, name, "file", st.st_mode);
        addData(x, file, buffer, done);
        free(buffer);
        return file;
    }
    if ((st.st_mode & S_IFMT) == S_IFDIR)
        return addEntry(x, parent, name, "directory", st.st_mode);
    return NULL;
}

File *xar_add(xar_t x, const char *path)
{
    const char *slash = strrchr(path, '/');
    return xar_add_frompath(x, NULL, slash ? slash + 1 : path, path);
}

xar_iter_t xar_iter_new(void)
{
    return calloc(1, sizeof(struct Iterator));
}

void xar_iter_free(xar_iter_t i)
{
    if (!i)
        return;
    free(i->unused0);
    free(i->unused10);
    free(i);
}

File *xar_file_first(xar_t x, xar_iter_t i)
{
    if (!x || !i)
        return NULL;
    i->archive = x;
    i->unused8 = 0;
    i->unused28 = 0;
    i->current = x->fileCount ? x->files[0] : NULL;
    return i->current;
}

// Depth first: the first child, else the next sibling of the file or of the nearest ancestor that has one.
File *xar_file_next(xar_iter_t i)
{
    if (!i || !i->current)
        return NULL;
    File *file = i->current;
    File *next = NULL;
    if (file->childCount) {
        next = file->children[0];
    } else {
        File *up = file;
        do {
            File *parent = up->parent;
            File **list = parent ? parent->children : i->archive->files;
            size_t count = parent ? parent->childCount : i->archive->fileCount;
            for (size_t k = 0; k + 1 < count; k++)
                if (list[k] == up) {
                    next = list[k + 1];
                    goto found;
                }
            up = parent;
        } while (up);
    }
found:
    i->current = next;
    return next;
}

int xar_prop_get(File *f, const char *key, const char **value)
{
    if (!f || !key || !value)
        return -1;
    Node *node = nodeAtPath(f->node, key, 0);
    if (!node)
        return -1;
    *value = node->text;
    if (!*value)
        *value = attributeOf(node, "style");
    return 0;
}

int xar_prop_set(File *f, const char *key, const char *value)
{
    if (!f || !key)
        return -1;
    Node *node = nodeAtPath(f->node, key, 1);
    free(node->text);
    node->text = value ? strdup(value) : NULL;
    free(f->safePath);
    f->safePath = NULL;
    return 0;
}

int xar_prop_create(File *f, const char *key, const char *value)
{
    if (!f || !key)
        return -1;
    Node *node = newNode(key, f->node);
    node->text = value ? strdup(value) : NULL;
    return 0;
}

// Takes a node out of the child list of its parent.
static void unlink_(Node *node)
{
    Node *parent = node->parent;
    for (size_t i = 0; i < parent->childCount; i++)
        if (parent->children[i] == node) {
            memmove(parent->children + i, parent->children + i + 1, (parent->childCount - i) * 8 - 8);
            parent->childCount--;
            break;
        }
}

void xar_prop_unset(File *f, const char *key)
{
    if (!f)
        return;
    Node *node = nodeAtPath(f->node, key, 0);
    if (!node || !node->parent)
        return;
    unlink_(node);
    freeNode(node);
}

const char *xar_attr_get(File *f, const char *prop, const char *key)
{
    if (!f)
        return NULL;
    Node *node = f->node;
    if (prop)
        node = nodeAtPath(node, prop, 0);
    return node ? attributeOf(node, key) : NULL;
}

int xar_attr_set(File *f, const char *prop, const char *key, const char *value)
{
    if (!f)
        return -1;
    Node *node = f->node;
    if (prop)
        node = nodeAtPath(node, prop, 1);
    if (value && key && node) {
        setAttribute(node, key, value);
        return 0;
    }
    return -1;
}

static char *buildPath(File *file, _Bool safe);

char *xar_get_path(File *f)
{
    return f ? buildPath(f, 0) : NULL;
}

// The path of a file: the names of its ancestors joined by slashes. With safe set a slash in a name becomes a colon and "." and ".." become "_".
static char *buildPath(File *file, _Bool safe)
{
    Node *nameNode = childNamed(file->node, "name");
    const char *text = nameNode ? nameNode->text : NULL;
    char *name = strdup(text ? text : "");
    if (safe) {
        char *p = name;
        for (;; p++) {
            if (*p == '/')
                *p = ':';
            else if (*p == 0)
                break;
        }
        if (name[0] == '.' && (name[1] == '.' ? name[2] == 0 : name[1] == 0)) {
            free(name);
            name = strdup("_");
        }
    }
    if (!file->parent)
        return name;
    char *parent = buildPath(file->parent, safe);
    char *path = malloc(strlen(parent) + strlen(name) + 2);
    sprintf(path, "%s/%s", parent, name);
    free(parent);
    free(name);
    return path;
}

const char *xar_get_safe_path(File *f)
{
    if (!f)
        return NULL;
    if (!f->safePath)
        f->safePath = buildPath(f, 1);
    return f->safePath;
}

const char *xar_get_type(xar_t x, File *f)
{
    if (!f)
        return NULL;
    Node *type = childNamed(f->node, "type");
    return type ? type->text : NULL;
}

unsigned long long xar_get_size(xar_t x, File *f)
{
    if (f) {
        Node *size = nodeAtPath(f->node, "data/size", 0);
        if (size && size->text)
            return strtoull(size->text, NULL, 10);
    }
    return 0;
}

unsigned short xar_get_mode(xar_t x, File *f)
{
    if (!f)
        return 0;
    Node *mode = childNamed(f->node, "mode");
    if (mode && mode->text)
        return (unsigned short)strtoul(mode->text, NULL, 8);
    return 0;
}

static int readData(xar_t x, File *f, unsigned char **out, size_t *outLength);

int xar_extract_tobuffersz(xar_t x, File *f, char **buffer, size_t *size)
{
    if (!x || !f || !buffer || !size || x->writable)
        return -1;
    unsigned char *data;
    size_t length;
    if (readData(x, f, &data, &length) != 0)
        return -1;
    *buffer = (char *)data;
    *size = length;
    return 0;
}

static const char *textOf(Node *node)
{
    return node ? node->text : NULL;
}

// The contents of a file's data: read from the heap, checked against the archived checksum, decompressed, checked against the extracted one.
static int readData(xar_t x, File *f, unsigned char **out, size_t *outLength)
{
    Node *data = childNamed(f->node, "data");
    if (!data) {
        *outLength = 0;
        *out = malloc(1);
        return 0;
    }
    *out = NULL;
    *outLength = 0;
    const char *text = textOf(childNamed(data, "offset"));
    unsigned long long offset = strtoull(text ? text : "0", NULL, 10);
    text = textOf(childNamed(data, "length"));
    unsigned long long length = strtoull(text ? text : "0", NULL, 10);
    text = textOf(childNamed(data, "size"));
    unsigned long long size = strtoull(text ? text : "0", NULL, 10);
    if (length > 0x100000000ULL || size > 0x400000000ULL)
        return -1;
    unsigned char *source = malloc(length + 1);
    if ((unsigned long long)pread(x->fd, source, length, offset + x->heapStart) != length) {
        free(source);
        return -1;
    }
    unsigned char md[64];
    char sum[160];
    size_t mdLength;
    Node *archived = childNamed(data, "archived-checksum");
    if (archived && archived->text) {
        const char *style = attributeOf(archived, "style");
        mdLength = 0;
        digest(style ? style : "sha1", source, length, md, &mdLength);
        if (mdLength) {
            hex(md, mdLength, sum);
            if (strcasecmp(sum, archived->text) != 0) {
                free(source);
                return -1;
            }
        }
    }
    const char *encoding = attributeOf(childNamed(data, "encoding"), "style");
    if (!encoding)
        encoding = "application/octet-stream";
    unsigned char *dest;
    if (strcmp(encoding, "application/x-gzip") == 0) {
        unsigned long destLength = size;
        dest = malloc(size + 1);
        if (uncompress(dest, &destLength, source, length) != 0 || destLength != size) {
            free(dest);
            dest = NULL;
        }
        free(source);
    } else if (strcmp(encoding, "application/x-bzip2") == 0) {
        unsigned destLength = (unsigned)size;
        dest = malloc(size + 1);
        if (BZ2_bzBuffToBuffDecompress((char *)dest, &destLength, (char *)source, (unsigned)length, 0, 0) != 0 || destLength != size) {
            free(dest);
            dest = NULL;
        }
        free(source);
    } else if (strcmp(encoding, "application/x-xz") == 0 || strcmp(encoding, "application/x-lzma") == 0) {
        dest = malloc(size + 1);
        if (compression_decode_buffer(dest, size + 1, source, length, NULL, COMPRESSION_LZMA) != size) {
            free(dest);
            free(source);
            return -1;
        }
        free(source);
    } else {
        dest = source;
    }
    if (!dest)
        return -1;
    Node *extracted = childNamed(data, "extracted-checksum");
    if (extracted && extracted->text) {
        const char *style = attributeOf(extracted, "style");
        mdLength = 0;
        digest(style ? style : "sha1", dest, size, md, &mdLength);
        if (mdLength) {
            hex(md, mdLength, sum);
            if (strcasecmp(sum, extracted->text) != 0) {
                free(dest);
                return -1;
            }
        }
    }
    *out = dest;
    *outLength = size;
    return 0;
}

int xar_extract_tobuffer(xar_t x, File *f, char **buffer)
{
    if (!x || !f || !buffer || x->writable)
        return -1;
    unsigned char *data;
    size_t length;
    if (readData(x, f, &data, &length) != 0)
        return -1;
    *buffer = (char *)data;
    return 0;
}

int xar_extract_tofile(xar_t x, File *f, const char *path)
{
    if (!x || !f || !path || x->writable)
        return -1;
    const char *type = "file";
    mode_t mode = 0;
    Node *typeNode = childNamed(f->node, "type");
    if (typeNode)
        type = typeNode->text ? typeNode->text : "file";
    Node *modeNode = childNamed(f->node, "mode");
    if (modeNode && modeNode->text)
        mode = (mode_t)strtoul(modeNode->text, NULL, 8);
    if (strcmp(type, "directory") == 0) {
        if (mkdir(path, mode ? mode : 0755) == 0)
            return 0;
        return errno != EEXIST ? -1 : 0;
    }
    if (strcmp(type, "symlink") == 0) {
        Node *link = childNamed(f->node, "link");
        if (!link)
            return -1;
        if (!link->text)
            return -1;
        return symlink(link->text, path) != 0 ? -1 : 0;
    }
    unsigned char *data;
    size_t length;
    if (readData(x, f, &data, &length) != 0)
        return -1;
    int result = 0;
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, (mode ? mode : 0644) & 0xffff);
    if (fd < 0) {
        result = -1;
    } else {
        if (!writeAll(fd, data, length))
            result = -1;
        if (mode)
            fchmod(fd, mode);
        close(fd);
    }
    free(data);
    return result;
}

int xar_extract(xar_t x, File *f)
{
    if (!f)
        return -1;
    char *path = buildPath(f, 0);
    if (!path)
        return -1;
    char *slash = strchr(path, '/');
    while (slash) {
        *slash = 0;
        mkdir(path, 0755);
        *slash = '/';
        slash = strchr(slash + 1, '/');
    }
    int result = xar_extract_tofile(x, f, path);
    free(path);
    return result;
}

int xar_verify(xar_t x, File *f)
{
    unsigned char *data;
    size_t length;
    int result = readData(x, f, &data, &length);
    free(data);
    return result;
}

Subdoc *xar_subdoc_new(xar_t x, const char *name)
{
    if (!x || !name)
        return NULL;
    Node *xarNode = childNamed(x->document, "xar");
    Node *node = newNode("subdoc", xarNode);
    setAttribute(node, "subdoc_name", name);
    Subdoc *subdoc = calloc(1, sizeof(Subdoc));
    subdoc->node = node;
    subdoc->archive = x;
    x->subdocs = realloc(x->subdocs, x->subdocCount * sizeof(Subdoc *) + sizeof(Subdoc *));
    x->subdocs[x->subdocCount++] = subdoc;
    return subdoc;
}

Subdoc *xar_subdoc_first(xar_t x)
{
    if (!x || !x->subdocCount)
        return NULL;
    return x->subdocs[0];
}

Subdoc *xar_subdoc_next(Subdoc *s)
{
    if (!s)
        return NULL;
    xar_t x = s->archive;
    for (size_t i = 0; i + 1 < x->subdocCount; i++)
        if (x->subdocs[i] == s)
            return x->subdocs[i + 1];
    return NULL;
}

const char *xar_subdoc_name(Subdoc *s)
{
    if (!s || !s->node)
        return NULL;
    return attributeOf(s->node, "subdoc_name");
}

int xar_subdoc_prop_set(Subdoc *s, const char *key, const char *value)
{
    if (!s || !key)
        return -1;
    Node *node = nodeAtPath(s->node, key, 1);
    free(node->text);
    node->text = value ? strdup(value) : NULL;
    return 0;
}

int xar_subdoc_prop_get(Subdoc *s, const char *key, const char **value)
{
    if (!s || !key || !value)
        return -1;
    Node *node = nodeAtPath(s->node, key, 0);
    if (!node)
        return -1;
    *value = node->text;
    return 0;
}

int xar_subdoc_attr_set(Subdoc *s, const char *prop, const char *key, const char *value)
{
    if (!s)
        return -1;
    Node *node = s->node;
    if (prop)
        node = nodeAtPath(node, prop, 1);
    if (value && key && node) {
        setAttribute(node, key, value);
        return 0;
    }
    return -1;
}

const char *xar_subdoc_attr_get(Subdoc *s, const char *prop, const char *key)
{
    if (!s)
        return NULL;
    Node *node = s->node;
    if (prop)
        node = nodeAtPath(node, prop, 0);
    return node ? attributeOf(node, key) : NULL;
}

void xar_subdoc_remove(Subdoc *s)
{
    if (!s)
        return;
    xar_t x = s->archive;
    Node *node = s->node;
    if (node->parent)
        unlink_(node);
    freeNode(s->node);
    for (size_t i = 0; i < x->subdocCount; i++)
        if (x->subdocs[i] == s) {
            memmove(x->subdocs + i, x->subdocs + i + 1, (x->subdocCount - i) * 8 - 8);
            x->subdocCount--;
            break;
        }
    free(s);
}

void xar_register_errhandler(xar_t x, void *callback, void *context)
{
}

// Serialises a node and its children, indented by one space per level.
static void writeNode(Buffer *b, Node *node, int depth)
{
    for (int i = 0; i < depth; i++)
        putByte(b, ' ');
    putByte(b, '<');
    put(b, node->name, strlen(node->name));
    for (size_t i = 0; i < node->attributeCount; i++) {
        putByte(b, ' ');
        put(b, node->attributes[i].name, strlen(node->attributes[i].name));
        put(b, "=\"", 2);
        putEscaped(b, node->attributes[i].value, 1);
        putByte(b, '"');
    }
    if (node->childCount == 0 && (!node->text || !*node->text)) {
        put(b, "/>\n", 3);
        return;
    }
    putByte(b, '>');
    int skipText = 0;
    if (node->text) {
        if (node->childCount) {
            for (const unsigned char *p = (const unsigned char *)node->text;; p++) {
                if (!(*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
                    skipText = *p == 0;
                    break;
                }
            }
        }
        if (!skipText)
            putEscaped(b, node->text, 0);
    }
    if (node->childCount) {
        putByte(b, '\n');
        for (size_t i = 0; i < node->childCount; i++)
            writeNode(b, node->children[i], depth + 1);
        for (int i = 0; i < depth; i++)
            putByte(b, ' ');
    }
    put(b, "</", 2);
    put(b, node->name, strlen(node->name));
    put(b, ">\n", 2);
}

// Text between quotes and tags with the character references resolved.
static char *unescape(const char *s, size_t n)
{
    char *out = malloc(n * 4 + 1);
    size_t j = 0, i = 0;
    while (i < n) {
        if (s[i] != '&') {
            out[j++] = s[i++];
            continue;
        }
        const char *semicolon = memchr(s + i, ';', n - i);
        if (!semicolon) {
            out[j++] = '&';
            i++;
            continue;
        }
        long length = semicolon - (s + i);
        const char *e = s + i;
        char c = 0;
        if (length == 3 && e[1] == 'l' && e[2] == 't')
            c = '<';
        else if (length == 3 && e[1] == 'g' && e[2] == 't')
            c = '>';
        else if (length == 4 && strncmp(e, "&amp", 4) == 0)
            c = '&';
        else if (length == 5 && strncmp(e, "&quot", 5) == 0)
            c = '"';
        else if (length == 5 && strncmp(e, "&apos", 5) == 0)
            c = '\'';
        if (c) {
            out[j++] = c;
            i += length + 1;
        } else if (s[i + 1] == '#') {
            unsigned long cp = e[2] == 'x' ? strtoul(e + 3, NULL, 16) : strtoul(e + 2, NULL, 10);
            if (cp < 0x80) {
                out[j++] = cp;
            } else if (cp < 0x800) {
                out[j++] = 0xc0 | (cp >> 6);
                out[j++] = 0x80 | (cp & 0x3f);
            } else if (cp < 0x10000) {
                out[j++] = 0xe0 | (cp >> 12);
                out[j++] = 0x80 | ((cp >> 6) & 0x3f);
                out[j++] = 0x80 | (cp & 0x3f);
            } else {
                out[j++] = 0xf0 | (cp >> 18);
                out[j++] = 0x80 | ((cp >> 12) & 0x3f);
                out[j++] = 0x80 | ((cp >> 6) & 0x3f);
                out[j++] = 0x80 | (cp & 0x3f);
            }
            i += length + 1;
        } else {
            out[j++] = '&';
            i++;
        }
    }
    out[j] = 0;
    return out;
}

// Text with the five characters of XML written as entities (a quote only inside an attribute).
static void putEscaped(Buffer *b, const char *s, unsigned attribute)
{
    for (; *s; s++) {
        switch (*s) {
        case '"':
            if (attribute)
                put(b, "&quot;", 6);
            else
                put(b, "\"", 1);
            break;
        case '&':
            put(b, "&amp;", 5);
            break;
        case '<':
            put(b, "&lt;", 4);
            break;
        case '>':
            put(b, "&gt;", 4);
            break;
        default:
            putByte(b, *s);
        }
    }
}

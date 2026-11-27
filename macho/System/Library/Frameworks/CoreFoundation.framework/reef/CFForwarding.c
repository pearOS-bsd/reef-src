// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The runtime side of NSMethodSignature and NSInvocation: type encoding parser, argument layout (x86_64), frames, forwarding entry points.
#include "ReefForwarding.h"
#include <objc/runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void *_Block_copy(const void *block);
extern void _Block_release(const void *block);
extern void *_NSConcreteMallocBlock[];
extern void *objc_alloc(void *cls);
extern void *objc_msgSend(void *, void *, ...);
extern void objc_msgSend_stret(void);
extern void *objc_retain(void *);
extern void objc_release(void *);
extern void *objc_autorelease(void *);
extern const char *object_getClassName(id object);
extern const char *sel_getName(SEL selector);
extern SEL sel_registerName(const char *name);
extern Class object_getClass(id object);
extern Class objc_getClass(const char *name);
extern BOOL class_respondsToSelector(Class cls, SEL selector);

#define MSG(type, object, name, ...) ((type (*)(void *, void *, ...))objc_msgSend)((void *)(object), sel_registerName(name), ##__VA_ARGS__)

typedef unsigned char (*LeafVisitor)(void *context, int type, unsigned offset, unsigned size);

// Skips the qualifiers (r n N o O R V and the A of atomic) in front of a type; the bit of each one goes to *flags.
static const char *skipQualifiers(const char *type, unsigned char *flags) {
    for (;; type++) {
        switch (*type) {
        case 'A': continue;
        case 'N': *flags |= 4; continue;
        case 'O': *flags |= 0x10; continue;
        case 'R': *flags |= 0x20; continue;
        case 'V': *flags |= 0x40; continue;
        case 'n': *flags |= 2; continue;
        case 'o': *flags |= 8; continue;
        case 'r': *flags |= 1; continue;
        default: return type;
        }
    }
}

// Skips the "class name" of an object type.
static const char *skipQuoted(const char *type) {
    if (*type == '"') {
        const char *end = strchr(type + 1, '"');
        return end ? end + 1 : type + strlen(type);
    }
    return type;
}

// Walks one type: gives its size and alignment, calls the visitor for every scalar (with its offset); *ok goes to 0 when the encoding is bad.
// Returns the pointer behind the type.
static const char *walkType(const char *type, unsigned offset, LeafVisitor visitor, void *context, unsigned *size, unsigned *alignment, unsigned char *ok) {
    unsigned char qualifiers = 0;
    type = skipQualifiers(type, &qualifiers);
    char c = *type;
    unsigned length, align;
    if (c == 0) goto bad;
    switch (c) {
    case '#': case '*': case ':': case '@': case 'Q': case 'd': case 'q':
        length = 8; align = 8; break;
    case '(': case '{': {
        char close = c == '{' ? '}' : ')';
        const char *p = type;
        for (;;) {
            p++;
            c = *p;
            if (c == 0) { type = p; goto bad; }
            if (c == '=') { p++; break; }
            if (c == close) { *size = 0; *alignment = 1; return p + 1; }
        }
        unsigned total = 0;
        align = 1;
        for (;;) {
            c = *p;
            if (c == 0) { type = p; goto bad; }
            if (c == close) break;
            if (c == '"') {
                const char *end = strchr(p + 1, '"');
                p = end ? end + 1 : p + strlen(p);
                c = *p;
                if (c == close) break;
            }
            unsigned memberSize = 0, memberAlign = 1;
            if (*type == '{') {
                walkType(p, 0, 0, 0, &memberSize, &memberAlign, ok);
                if (memberAlign > 1) { total += memberAlign - 1; total -= total % memberAlign; }
                p = walkType(p, total + offset, visitor, context, &memberSize, &memberAlign, ok);
                total += memberSize;
            } else {
                p = walkType(p, offset, visitor, context, &memberSize, &memberAlign, ok);
                if (total < memberSize) total = memberSize;
            }
            if (align < memberAlign) align = memberAlign;
            if (*ok == 0) return p;
        }
        if (align > 1) { total += align - 1; total -= total % align; }
        *size = total; *alignment = align;
        return p + 1;
    }
    case '?': case 'v':
        length = 0; align = 1; break;
    case 'B': case 'C': case 'c':
        length = 1; align = 1; break;
    case 'D': case 'T': case 't':
        length = 16; align = 16; break;
    case 'I': case 'L': case 'f': case 'i': case 'l':
        length = 4; align = 4; break;
    case 'S': case 's':
        length = 2; align = 2; break;
    case '[': {
        const char *p = type + 1;
        unsigned count = 0;
        while ((unsigned char)(*p - '0') < 10) count = count * 10 + (*p++ - '0');
        unsigned elementSize = 0, elementAlign = 1;
        const char *end = walkType(p, offset, 0, 0, &elementSize, &elementAlign, ok);
        if (visitor && count) {
            for (unsigned i = 0; i < count && *ok == 1; i++) walkType(p, elementSize * i + offset, visitor, context, &elementSize, &elementAlign, ok);
        }
        if (*end == ']') { *size = count * elementSize; *alignment = elementAlign; return end + 1; }
        type = end;
        goto bad;
    }
    case '^': {
        *size = 8; *alignment = 8;
        if (visitor) *ok = visitor(context, '^', offset, 8) ? *ok : 0;
        unsigned pointeeSize = 0, pointeeAlign;
        return walkType(type + 1, 0, 0, 0, &pointeeSize, &pointeeAlign, ok);
    }
    case 'b': {
        const char *p = type + 1;
        unsigned bits = 0, bytes = 0;
        if ((unsigned char)(*p - '0') < 10) {
            while ((unsigned char)(*p - '0') < 10) bits = bits * 10 + (*p++ - '0');
            bytes = (bits + 7) >> 3;
        }
        *size = bytes; *alignment = 1;
        if (visitor && bytes) *ok = visitor(context, 'C', offset, bytes) ? *ok : 0;
        return p;
    }
    case 'j': {
        unsigned partSize = 0, partAlign = 1;
        const char *end = walkType(type + 1, offset, visitor, context, &partSize, &partAlign, ok);
        if (visitor) walkType(type + 1, offset + partSize, visitor, context, &partSize, &partAlign, ok);
        *size = partSize * 2; *alignment = partAlign;
        return end;
    }
    default:
        goto bad;
    }
    *size = length; *alignment = align;
    if (visitor && length) *ok = visitor(context, c, offset, length) ? *ok : 0;
    type++;
    if (c == '@') {
        if (*type != '?') return skipQuoted(type);
        type++;
        if (*type == '<') {
            int depth = 0;
            char d = '<';
            do {
                if (d == '>') depth--; else if (d == '<') depth++;
                d = *++type;
            } while (d != 0 && depth != 0);
        }
    }
    return type;
bad:
    *ok = 0;
    return type;
}

// The registers a value is passed in: for every eight bytes of it the class (1 integer, 2 vector, 0 none yet), and a flag that it must go to memory.
struct Classes {
    unsigned char eightbytes[2];
    unsigned char memory;
};

static unsigned char classifyLeaf(void *context, int type, unsigned offset, unsigned size) {
    struct Classes *classes = context;
    if ((type != 'D' && type != 'T' && type != 't') && offset < 16) {
        unsigned first = offset >> 3, last = (offset + size - 1) >> 3;
        if (first == last) {
            if ((classes->eightbytes[first] & 0xfd) == 0) classes->eightbytes[first] = ((type & 0xfd) == 'd') + 1;
            return 1;
        }
    }
    classes->memory = 1;
    return 1;
}

// Reads the next value of a signature: type, size, alignment, qualifiers; skips the frame offset digits behind it.
static int parseValue(const char **cursor, struct ReefValue *value) {
    unsigned char qualifiers = 0;
    const char *start = skipQualifiers(*cursor, &qualifiers);
    if (*start == 0) return 0;
    unsigned char ok = 1;
    unsigned size = 0, alignment = 1;
    const char *end = walkType(start, 0, 0, 0, &size, &alignment, &ok);
    if (ok != 1) return 0;
    memset(value, 0, sizeof *value);
    value->type = strndup(start, end - start);
    value->size = size;
    value->alignment = alignment + (alignment == 0);
    value->qualifiers = qualifiers;
    unsigned char first = *start;
    value->isSigned = (unsigned)(first - 99) <= 0x11 && ((0x34241u >> (first - 99)) & 1);
    value->isInteger = memchr("cCsSiIlLqQB", (char)first, 12) != 0;
    if (*end == '-') end++;
    while ((unsigned char)(*end - '0') < 10) end++;
    *cursor = end;
    return 1;
}

// Splits a value that does not need memory into its pieces: the register class of each eight bytes, counted separately for integer and vector registers.
static void fillPieces(struct ReefValue *value, const struct Classes *classes, unsigned *integers, unsigned *vectors) {
    unsigned remaining = value->size, count = (value->size + 7) >> 3;
    value->pieceCount = count;
    for (unsigned i = 0; i < count; i++) {
        int vector = classes->eightbytes[i] == 2;
        value->pieces[i].where = vector;
        value->pieces[i].size = remaining < 8 ? remaining : 8;
        value->pieces[i].index = vector ? (*vectors)++ : (*integers)++;
        value->pieces[i].offset = i * 8;
        remaining -= 8;
    }
}

// Works out whether a value goes to memory (more than 16 bytes, unaligned fields, x87) and, if not, the class of its eightbytes.
static int classify(struct ReefValue *value, struct Classes *classes) {
    memset(classes, 0, sizeof *classes);
    if (value->size - 0x11 < 0xfffffff0u) return 1;
    unsigned char ok = 1;
    unsigned size, alignment;
    walkType(value->type, 0, classifyLeaf, classes, &size, &alignment, &ok);
    return ok == 0 || classes->memory;
}

struct ReefSignature *reefSignatureCreate(const char *types) {
    if (types == 0 || *types == 0) return 0;
    struct ReefSignature *signature = calloc(1, sizeof *signature);
    signature->types = strdup(types);
    const char *cursor = types;
    int good = parseValue(&cursor, &signature->result);
    if (good) {
        unsigned capacity = 4;
        signature->arguments = calloc(4, sizeof(struct ReefValue));
        while (*cursor) {
            if (signature->argumentCount == capacity) {
                capacity = signature->argumentCount * 2;
                signature->arguments = realloc(signature->arguments, capacity * sizeof(struct ReefValue));
            }
            if (!parseValue(&cursor, &signature->arguments[signature->argumentCount])) { good = 0; break; }
            signature->argumentCount++;
        }
    }
    if (!good) {
        free(signature->result.type);
        for (unsigned i = 0; i < signature->argumentCount; i++) free(signature->arguments[i].type);
        free(signature->arguments);
        free(signature->types);
        free(signature);
        return 0;
    }
    // The result.
    unsigned integers = 0, vectors = 0, stack = 0;
    struct ReefValue *result = &signature->result;
    if (result->size == 0) signature->resultKind = ReefResultRegisters;
    else if (*result->type == 'D') signature->resultKind = ReefResultX87;
    else {
        struct Classes classes;
        if (classify(result, &classes)) { signature->resultKind = ReefResultMemory; integers = 1; }
        else {
            unsigned resultIntegers = 0, resultVectors = 0;
            fillPieces(result, &classes, &resultIntegers, &resultVectors);
            signature->resultKind = ReefResultRegisters;
        }
    }
    // The arguments.
    for (unsigned i = 0; i < signature->argumentCount; i++) {
        struct ReefValue *argument = &signature->arguments[i];
        if (argument->size == 0) continue;
        struct Classes classes;
        int memory = classify(argument, &classes);
        if (!memory) {
            unsigned needIntegers = 0, needVectors = 0;
            for (unsigned k = 0; k < ((argument->size + 7) >> 3); k++) {
                if (classes.eightbytes[k] == 2) needVectors++; else needIntegers++;
            }
            if (needIntegers + integers > 6 || needVectors + vectors > 8) memory = 1;
            else fillPieces(argument, &classes, &integers, &vectors);
        }
        if (memory) {
            unsigned alignment = argument->alignment < 9 ? 8 : argument->alignment;
            stack = (stack + alignment - 1) - ((stack + alignment - 1) % alignment);
            argument->pieceCount = 1;
            argument->pieces[0].where = ReefPieceStack;
            argument->pieces[0].size = argument->size > 255 ? 0 : argument->size;
            argument->pieces[0].index = stack;
            argument->pieces[0].offset = 0;
            stack += (argument->size + 7) & ~7u;
        }
    }
    signature->frameLength = (stack + 15) & ~15u;
    return signature;
}

void reefSignatureDestroy(struct ReefSignature *signature) {
    if (signature == 0) return;
    free(signature->result.type);
    for (unsigned i = 0; i < signature->argumentCount; i++) free(signature->arguments[i].type);
    free(signature->arguments);
    free(signature->types);
    free(signature);
}

// Where a piece of a value is in the frame (the argument area, or the result area).
static uint8_t *piecePlace(struct ReefFrame *frame, const struct ReefPiece *piece, int result) {
    if (piece->where == ReefPieceVector) return (result ? frame->resultVectors[piece->index] : frame->vectors[piece->index]);
    if (piece->where == ReefPieceInteger) return (uint8_t *)(result ? &frame->resultIntegers[piece->index] : &frame->integers[piece->index]);
    return frame->stack + piece->index;
}

void reefFrameGetValue(const struct ReefFrame *constFrame, const struct ReefValue *value, void *out) {
    struct ReefFrame *frame = (struct ReefFrame *)constFrame;
    if (value->indirect == 1) {
        memcpy(out, *(void **)piecePlace(frame, &value->pieces[0], 0), value->size);
        return;
    }
    if (value->pieceCount == 0) return;
    if (value->pieceCount == 1 && value->pieces[0].where == ReefPieceStack) {
        memcpy(out, frame->stack + value->pieces[0].index, value->size);
        return;
    }
    for (unsigned i = 0; i < value->pieceCount; i++)
        memcpy((char *)out + value->pieces[i].offset, piecePlace(frame, &value->pieces[i], 0), value->pieces[i].size);
}

static void copyIn(struct ReefFrame *frame, const struct ReefValue *value, const void *in, int result) {
    if (value->indirect == 1) {
        memcpy(*(void **)piecePlace(frame, &value->pieces[0], result), in, value->size);
        return;
    }
    if (value->pieceCount == 0) return;
    if (value->pieceCount == 1 && value->pieces[0].where == ReefPieceStack) {
        memcpy(frame->stack + value->pieces[0].index, in, value->size);
        return;
    }
    for (unsigned i = 0; i < value->pieceCount; i++) {
        const struct ReefPiece *piece = &value->pieces[i];
        uint8_t *place = piecePlace(frame, piece, result);
        // A small integer in an integer register is extended to the whole register.
        if (piece->where == ReefPieceInteger && value->isInteger && value->size < 8) {
            uint64_t *word = (uint64_t *)place;
            switch (value->size) {
            case 4: *word = value->isSigned == 1 ? (uint64_t)(int64_t) * (const int32_t *)in : (uint64_t) * (const uint32_t *)in; break;
            case 2: *word = value->isSigned == 1 ? (uint64_t)(int64_t) * (const int16_t *)in : (uint64_t) * (const uint16_t *)in; break;
            case 1: *word = value->isSigned == 1 ? (uint64_t)(int64_t) * (const int8_t *)in : (uint64_t) * (const uint8_t *)in; break;
            default: *word = 0; break;
            }
            continue;
        }
        memcpy(place, (const char *)in + piece->offset, piece->size);
    }
}

void reefFrameSetValue(struct ReefFrame *frame, const struct ReefValue *value, const void *in) {
    copyIn(frame, value, in, 0);
}

void reefFrameGetResult(const struct ReefFrame *constFrame, const struct ReefSignature *signature, void *out) {
    struct ReefFrame *frame = (struct ReefFrame *)constFrame;
    const void *source;
    if (signature->resultKind == ReefResultX87) source = frame->resultX87;
    else if (signature->resultKind == ReefResultMemory) source = (void *)frame->integers[0];
    else if (signature->result.indirect == 1) source = *(void **)piecePlace(frame, &signature->result.pieces[0], 1);
    else {
        if (signature->result.pieceCount == 0) return;
        if (signature->result.pieceCount == 1 && signature->result.pieces[0].where == ReefPieceStack) source = frame->stack + signature->result.pieces[0].index;
        else {
            for (unsigned i = 0; i < signature->result.pieceCount; i++)
                memcpy((char *)out + signature->result.pieces[i].offset, piecePlace(frame, &signature->result.pieces[i], 1), signature->result.pieces[i].size);
            return;
        }
    }
    memcpy(out, source, signature->result.size);
}

void reefFrameSetResult(struct ReefFrame *frame, const struct ReefSignature *signature, const void *in) {
    if (signature->resultKind == ReefResultX87) memcpy(frame->resultX87, in, signature->result.size);
    else if (signature->resultKind == ReefResultMemory) memcpy((void *)frame->integers[0], in, signature->result.size);
    else copyIn(frame, &signature->result, in, 1);
}

// The forwarding handler: the message goes to the object of forwardingTargetForSelector:, or to forwardInvocation: of the receiver with an NSInvocation
// made from the frame. Returns the function to jump to (objc_msgSend with the new receiver), or 0 when the invocation has done everything.
__attribute__((used, noinline)) static void *reefForwarding(struct ReefFrame *frame, int stret) {
    int slot = stret != 0;
    void *receiver = (void *)frame->integers[slot];
    void *selector = (void *)frame->integers[slot + 1];
    SEL forwardingTarget = sel_registerName("forwardingTargetForSelector:");
    if (class_respondsToSelector(object_getClass(receiver), forwardingTarget)) {
        void *target = MSG(void *, receiver, "forwardingTargetForSelector:", selector);
        if (target != receiver && target != 0) {
            frame->integers[slot] = (uint64_t)target;
            return stret ? (void *)objc_msgSend_stret : (void *)objc_msgSend;
        }
    }
    void *signature = 0;
    if (class_respondsToSelector(object_getClass(receiver), sel_registerName("methodSignatureForSelector:")))
        signature = MSG(void *, receiver, "methodSignatureForSelector:", selector);
    if (signature == 0) {
        if (class_respondsToSelector(object_getClass(receiver), sel_registerName("doesNotRecognizeSelector:")))
            MSG(void, receiver, "doesNotRecognizeSelector:", selector);
        fprintf(stderr, "*** NSForwarding: warning: object %p of class '%s' does not implement methodSignatureForSelector: -- trouble ahead\n", receiver, object_getClassName(receiver));
        abort();
    }
    struct ReefSignature *layout = MSG(struct ReefSignature *, signature, "_reefSignature");
    if ((layout->resultKind != ReefResultMemory) == (stret != 0)) {
        fprintf(stderr, "*** NSForwarding: warning: method signature and compiler disagree on struct-return-edness of '%s'.  Signature thinks it does%s return a struct, and compiler thinks it does%s.\n",
                sel_getName(selector), layout->resultKind == ReefResultMemory ? "" : " not", stret ? "" : " not");
        abort();
    }
    void *invocation = MSG(void *, objc_alloc(objc_getClass("NSInvocation")), "_initWithMethodSignature:frame:", signature, frame);
    MSG(void, receiver, "forwardInvocation:", invocation);
    MSG(void, invocation, "_storeResultInFrame:", frame);
    objc_release(invocation);
    return 0;
}

// The forwarding handler of blocks: the block has the signature and the handler block behind its descriptor.
__attribute__((used, noinline)) static void *reefBlockForwarding(struct ReefFrame *frame, int stret) {
    char *block = (char *)frame->integers[stret != 0];
    void *invocation = MSG(void *, objc_alloc(objc_getClass("NSInvocation")), "_initWithMethodSignature:frame:", *(void **)(block + 0x20), frame);
    void *handler = *(void **)(block + 0x28);
    (*(void (**)(void *, void *))((char *)handler + 0x10))(handler, invocation);
    MSG(void, invocation, "_storeResultInFrame:", frame);
    objc_release(invocation);
    return 0;
}

// The entry points have the frame of the call on their stack: registers, the address of the stack arguments; what the handler returns is jumped to
// (with the registers back), or the result of the frame goes to the result registers.
#define REEF_ENTRY(name, handler, stret) \
    __asm__(".text\n.globl _" #name "\n.p2align 4\n_" #name ":\n" \
        "pushq %rbp\n movq %rsp, %rbp\n subq $0x110, %rsp\n" \
        "movq %rdi, (%rsp)\n movq %rsi, 0x8(%rsp)\n movq %rdx, 0x10(%rsp)\n movq %rcx, 0x18(%rsp)\n movq %r8, 0x20(%rsp)\n movq %r9, 0x28(%rsp)\n" \
        "movq %rax, 0x30(%rsp)\n movq $0, 0x38(%rsp)\n" \
        "movdqa %xmm0, 0x40(%rsp)\n movdqa %xmm1, 0x50(%rsp)\n movdqa %xmm2, 0x60(%rsp)\n movdqa %xmm3, 0x70(%rsp)\n" \
        "movdqa %xmm4, 0x80(%rsp)\n movdqa %xmm5, 0x90(%rsp)\n movdqa %xmm6, 0xa0(%rsp)\n movdqa %xmm7, 0xb0(%rsp)\n" \
        "leaq 0x10(%rbp), %r10\n movq %r10, 0xc0(%rsp)\n" \
        "movq %rsp, %rdi\n movl $" #stret ", %esi\n callq " #handler "\n" \
        "testq %rax, %rax\n je 1f\n movq %rax, %r11\n" \
        "movdqa 0x40(%rsp), %xmm0\n movdqa 0x50(%rsp), %xmm1\n movdqa 0x60(%rsp), %xmm2\n movdqa 0x70(%rsp), %xmm3\n" \
        "movdqa 0x80(%rsp), %xmm4\n movdqa 0x90(%rsp), %xmm5\n movdqa 0xa0(%rsp), %xmm6\n movdqa 0xb0(%rsp), %xmm7\n" \
        "movq 0x8(%rsp), %rsi\n movq 0x10(%rsp), %rdx\n movq 0x18(%rsp), %rcx\n movq 0x20(%rsp), %r8\n movq 0x28(%rsp), %r9\n" \
        "movq 0x30(%rsp), %rax\n movq (%rsp), %rdi\n movq %rbp, %rsp\n popq %rbp\n jmpq *%r11\n" \
        "1:\n movq 0xd0(%rsp), %rax\n movq 0xd8(%rsp), %rdx\n movdqa 0xe0(%rsp), %xmm0\n movdqa 0xf0(%rsp), %xmm1\n" \
        "cmpq $0, 0x38(%rsp)\n je 2f\n fldt 0x100(%rsp)\n" \
        "2:\n movq %rbp, %rsp\n popq %rbp\n retq\n")

REEF_ENTRY(reefForwardingEntry, _reefForwarding, 0);
REEF_ENTRY(reefForwardingEntryStret, _reefForwarding, 1);
REEF_ENTRY(reefBlockForwardingEntry, _reefBlockForwarding, 0);
REEF_ENTRY(reefBlockForwardingEntryStret, _reefBlockForwarding, 1);

// Calls function with the registers and the stack arguments of the frame, and stores the result registers in the frame (the x87 one if asked).
__asm__(".text\n.globl _reefFrameCall\n.p2align 4\n_reefFrameCall:\n"
    "pushq %rbp\n movq %rsp, %rbp\n pushq %rbx\n pushq %r12\n pushq %r13\n pushq %r14\n"
    "movq %rdi, %rbx\n movq %rdx, %r12\n movl %ecx, %r13d\n movl %esi, %ecx\n subq %rcx, %rsp\n"
    "movq 0xc0(%rbx), %rsi\n movq %rsp, %rdi\n rep movsb\n"
    "movdqa 0x40(%rbx), %xmm0\n movdqa 0x50(%rbx), %xmm1\n movdqa 0x60(%rbx), %xmm2\n movdqa 0x70(%rbx), %xmm3\n"
    "movdqa 0x80(%rbx), %xmm4\n movdqa 0x90(%rbx), %xmm5\n movdqa 0xa0(%rbx), %xmm6\n movdqa 0xb0(%rbx), %xmm7\n"
    "movq 0x8(%rbx), %rsi\n movq 0x10(%rbx), %rdx\n movq 0x18(%rbx), %rcx\n movq 0x20(%rbx), %r8\n movq 0x28(%rbx), %r9\n"
    "movq 0x30(%rbx), %rax\n movq (%rbx), %rdi\n callq *%r12\n"
    "movq %rax, 0xd0(%rbx)\n movq %rdx, 0xd8(%rbx)\n movdqa %xmm0, 0xe0(%rbx)\n movdqa %xmm1, 0xf0(%rbx)\n"
    "cmpl $2, %r13d\n jne 1f\n fstpt 0x100(%rbx)\n"
    "1:\n leaq -0x20(%rbp), %rsp\n popq %r14\n popq %r13\n popq %r12\n popq %rbx\n popq %rbp\n retq\n");

// The block that forwards what is sent to it to a handler block through an NSInvocation (the signature of the block comes from its type encoding,
// without the offsets between < > and the quoted names).
static void forwardingBlockCopy(void *destination, void *source) {
    *(void **)((char *)destination + 0x20) = objc_retain(*(void **)((char *)source + 0x20));
    *(void **)((char *)destination + 0x28) = _Block_copy(*(void **)((char *)source + 0x28));
}

static void forwardingBlockDispose(void *block) {
    objc_release(*(void **)((char *)block + 0x20));
    _Block_release(*(void **)((char *)block + 0x28));
    free(*(void **)((char *)block + 0x18));
}

void *__NSMakeSpecialForwardingCaptureBlock(const char *types, void *handler) {
    if (types == 0 || handler == 0) return 0;
    char *clean = malloc(strlen(types) + 1);
    size_t length = 0;
    for (;;) {
        char c = *types;
        while (c == '<') {
            int depth = 0;
            c = '<';
            do {
                if (c == '>') depth--; else if (c == '<') depth++;
                c = *++types;
            } while (c != 0 && depth > 0);
        }
        if (c == 0) break;
        types++;
        if (c == '"') {
            char *end = strchr(types, '"');
            if (end) types = end + 1;
        } else clean[length++] = c;
    }
    clean[length] = 0;
    void *signature = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", clean);
    char *descriptor = calloc(1, strlen(clean) + 0x31);
    *(unsigned long *)(descriptor + 8) = 0x30;
    *(void **)(descriptor + 0x10) = (void *)forwardingBlockCopy;
    *(void **)(descriptor + 0x18) = (void *)forwardingBlockDispose;
    strcpy(descriptor + 0x30, clean);
    *(char **)(descriptor + 0x20) = descriptor + 0x30;
    free(clean);
    void **block = calloc(1, 0x30);
    block[0] = _NSConcreteMallocBlock;
    *(unsigned *)(block + 1) = 0x43000002;
    struct ReefSignature *layout = MSG(struct ReefSignature *, signature, "_reefSignature");
    block[2] = layout->resultKind == ReefResultMemory ? (void *)reefBlockForwardingEntryStret : (void *)reefBlockForwardingEntry;
    block[3] = descriptor;
    block[4] = objc_retain(signature);
    block[5] = _Block_copy(handler);
    return objc_autorelease(block);
}

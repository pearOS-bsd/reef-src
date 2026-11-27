// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The method signature and call frame structures of NSMethodSignature, NSInvocation and the forwarding handler (x86_64 calling convention).
#pragma once
#include <stdint.h>

// Where a piece of an argument or of the result lives in a frame.
enum { ReefPieceInteger = 0, ReefPieceVector = 1, ReefPieceStack = 2 };

// A part of a value of at most eight bytes: in an integer register, an SSE register or on the stack (index = stack offset then).
struct ReefPiece {
    uint8_t where;
    uint8_t size;
    uint8_t reserved[2];
    uint32_t index;
    uint32_t offset;
};

// One argument (or the result) of a signature: its type encoding, size, alignment and the pieces it is passed in.
struct ReefValue {
    char *type;
    uint32_t size;
    uint32_t alignment;
    uint8_t qualifiers;
    uint8_t isSigned;
    uint8_t isInteger;
    uint8_t indirect;
    uint8_t pieceCount;
    uint8_t reserved[3];
    struct ReefPiece pieces[2];
    uint8_t spare[24];
};

// The result is returned in registers (0), through a hidden pointer (1) or on the x87 stack (2).
enum { ReefResultRegisters = 0, ReefResultMemory = 1, ReefResultX87 = 2 };

struct ReefSignature {
    char *types;
    struct ReefValue result;
    uint8_t resultKind;
    uint8_t reserved[3];
    uint32_t argumentCount;
    struct ReefValue *arguments;
    uint32_t frameLength;
    uint32_t spare;
};

// The registers and stack of a call: filled by the forwarding entry points, read and written by NSInvocation.
struct ReefFrame {
    uint64_t integers[6];
    uint64_t vectorCount;
    uint64_t x87;
    uint8_t vectors[8][16];
    uint8_t *stack;
    uint64_t reserved;
    uint64_t resultIntegers[2];
    uint8_t resultVectors[2][16];
    uint8_t resultX87[16];
};

extern struct ReefSignature *reefSignatureCreate(const char *types);
extern void reefSignatureDestroy(struct ReefSignature *signature);
extern void reefFrameGetValue(const struct ReefFrame *frame, const struct ReefValue *value, void *out);
extern void reefFrameSetValue(struct ReefFrame *frame, const struct ReefValue *value, const void *in);
extern void reefFrameGetResult(const struct ReefFrame *frame, const struct ReefSignature *signature, void *out);
extern void reefFrameSetResult(struct ReefFrame *frame, const struct ReefSignature *signature, const void *in);
extern void reefFrameCall(struct ReefFrame *frame, unsigned stackLength, void *function, int resultKind);
extern void reefForwardingEntry(void);
extern void reefForwardingEntryStret(void);
extern void reefBlockForwardingEntry(void);
extern void reefBlockForwardingEntryStret(void);

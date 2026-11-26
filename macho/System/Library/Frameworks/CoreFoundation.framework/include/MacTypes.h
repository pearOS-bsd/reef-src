// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// The basic Mac types (the Apple SDK is not used): Boolean is an unsigned char, as the Mac build of CoreFoundation shows.
#pragma once
#define __MACTYPES__ 1
#include <stdint.h>
typedef unsigned char Boolean;
typedef unsigned char UInt8;
typedef signed char SInt8;
typedef unsigned short UInt16;
typedef signed short SInt16;
typedef unsigned int UInt32;
typedef signed int SInt32;
typedef unsigned long long UInt64;
typedef signed long long SInt64;
typedef SInt32 OSStatus;
typedef float Float32;
typedef double Float64;
typedef unsigned short UniChar;
typedef unsigned long UniCharCount;
typedef unsigned char *StringPtr;
typedef const unsigned char *ConstStringPtr;
typedef unsigned char Str255[256];
typedef const unsigned char *ConstStr255Param;
typedef SInt16 OSErr;
typedef SInt16 RegionCode;
typedef SInt16 LangCode;
typedef SInt16 ScriptCode;
typedef UInt32 FourCharCode;
typedef FourCharCode OSType;
typedef UInt8 Byte;
typedef SInt8 SignedByte;
typedef UInt32 UTF32Char;
typedef UInt16 UTF16Char;
typedef UInt8 UTF8Char;

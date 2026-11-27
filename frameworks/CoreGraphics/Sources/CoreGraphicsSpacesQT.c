// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <CoreGraphics/CoreGraphics.h>

// Window server spaces (private CGS API). Reef's window server has one space; created spaces are numbers only.
typedef int CGSConnectionID;
typedef unsigned long long CGSSpaceID;
static CGSSpaceID nextSpace = 100;

CGSSpaceID CGSSpaceCreate(CGSConnectionID cid, int flags, CFDictionaryRef options) { return nextSpace++; }
void CGSSpaceDestroy(CGSConnectionID cid, CGSSpaceID space) {}
void CGSSpaceSetAbsoluteLevel(CGSConnectionID cid, CGSSpaceID space, int level) {}
void CGSShowSpaces(CGSConnectionID cid, CFArrayRef spaces) {}
void CGSHideSpaces(CGSConnectionID cid, CFArrayRef spaces) {}
void CGSAddWindowsToSpaces(CGSConnectionID cid, CFArrayRef windows, CFArrayRef spaces) {}
void CGSRemoveWindowsFromSpaces(CGSConnectionID cid, CFArrayRef windows, CFArrayRef spaces) {}
CFArrayRef CGSCopySpaces(CGSConnectionID cid, int mask) { return CFArrayCreate(NULL, NULL, 0, &kCFTypeArrayCallBacks); }
bool CGSIsZoomed(CGSConnectionID cid, int window) { return false; }
int CGSRegisterConnectionNotifyProc(CGSConnectionID cid, void *proc, int type, void *userInfo) { return 0; }
int CGSRemoveConnectionNotifyProc(CGSConnectionID cid, void *proc, int type, void *userInfo) { return 0; }
// Global hot keys: accepted, never fired.
int CGSSetHotKey(CGSConnectionID cid, int key, unsigned short keyCode, unsigned int modifiers) { return 0; }
int CGSRemoveHotKey(CGSConnectionID cid, int key) { return 0; }
int CGSSetHotKeyEnabled(CGSConnectionID cid, int key, bool enabled) { return 0; }

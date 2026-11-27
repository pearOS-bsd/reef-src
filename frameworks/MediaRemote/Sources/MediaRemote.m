// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

// Now Playing / remote control: nothing else to talk to; the local origin is not available.
void *MRMediaRemoteGetLocalOrigin(void) { return NULL; }
void MRMediaRemoteSetNowPlayingTintColor(id color) {}

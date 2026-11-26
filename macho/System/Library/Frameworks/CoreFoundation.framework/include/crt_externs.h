// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
extern char ***_NSGetArgv(void);
extern int *_NSGetArgc(void);
extern char ***_NSGetEnviron(void);
extern char **_NSGetProgname(void);
extern int _NSGetExecutablePath(char *buf, unsigned int *bufsize);

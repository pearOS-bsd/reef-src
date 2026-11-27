// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <CoreVideo/CoreVideo.h>
#include <string.h>

// H.273 code points of the colour descriptions of image buffers (2 = unspecified).
static int Code(CFStringRef s, const char *const *names, const int *codes, int n) {
    char buf[64];
    if (!s || !CFStringGetCString(s, buf, sizeof buf, kCFStringEncodingUTF8)) return 2;
    for (int i = 0; i < n; i++) if (strcmp(buf, names[i]) == 0) return codes[i];
    return 2;
}
int CVColorPrimariesGetIntegerCodePointForString(CFStringRef s) {
    static const char *const n[] = {"ITU_R_709_2", "EBU_3213", "SMPTE_C", "ITU_R_2020", "P3_D65", "DCI_P3"};
    static const int c[] = {1, 5, 6, 9, 12, 11};
    return Code(s, n, c, 6);
}
int CVTransferFunctionGetIntegerCodePointForString(CFStringRef s) {
    static const char *const n[] = {"ITU_R_709_2", "SMPTE_240M_1995", "UseGamma", "ITU_R_2020", "SMPTE_ST_428_1", "SMPTE_ST_2084_PQ", "ITU_R_2100_HLG", "sRGB", "Linear"};
    static const int c[] = {1, 7, 2, 14, 17, 16, 18, 13, 8};
    return Code(s, n, c, 9);
}
int CVYCbCrMatrixGetIntegerCodePointForString(CFStringRef s) {
    static const char *const n[] = {"ITU_R_709_2", "ITU_R_601_4", "SMPTE_240M_1995", "ITU_R_2020", "P3_D65"};
    static const int c[] = {1, 6, 7, 9, 12};
    return Code(s, n, c, 5);
}

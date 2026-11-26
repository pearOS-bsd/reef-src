// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ucnv_convertEx: the host's library takes the two ICU booleans as unsigned bytes: widen them before the call.

extern void *reef_host_symbol(const char *library, const char *name);

typedef void (*ConvertEx)(void *, void *, char **, const char *, const char **, const char *, unsigned short *, unsigned short **,
                          unsigned short **, const unsigned short *, unsigned char, unsigned char, int *);

void ucnv_convertEx(void *targetCnv, void *sourceCnv, char **target, const char *targetLimit, const char **source,
                    const char *sourceLimit, unsigned short *pivotStart, unsigned short **pivotSource, unsigned short **pivotTarget,
                    const unsigned short *pivotLimit, signed char reset, signed char flush, int *pErrorCode)
{
    static ConvertEx host;
    if (!host)
        host = (ConvertEx)reef_host_symbol("libicuuc.so.76:libicui18n.so.76", "ucnv_convertEx_76");
    host(targetCnv, sourceCnv, target, targetLimit, source, sourceLimit, pivotStart, pivotSource, pivotTarget, pivotLimit, reset, flush, pErrorCode);
}

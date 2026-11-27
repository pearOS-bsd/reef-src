// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libtidy: parse a broken page, clean and repair it, walk the tree, read the options and the diagnostics. Only what the old Tidy of macOS and Tidy 5 do the same
// is printed (the written text differs between the versions). The output must be the same on a Mac and under reef.
#include <stdio.h>
#include <string.h>
#include <tidy.h>
#if __has_include(<tidybuffio.h>)
#include <tidybuffio.h>
#else
#include <buffio.h>
#endif

static int count(TidyNode n, const char *tag)
{
    int k = 0;
    for (TidyNode c = tidyGetChild(n); c; c = tidyGetNext(c)) {
        ctmbstr name = tidyNodeGetName(c);
        if (name && !strcmp(name, tag)) k++;
        k += count(c, tag);
    }
    return k;
}

int main(void)
{
    const char *html = "<title>Reef<p>one<b>bold<p id=x class=y>two <i>italic</b> done<br><table><tr><td>cell</table>";
    TidyDoc doc = tidyCreate();
    tidyOptSetBool(doc, TidyXhtmlOut, yes);
    tidyOptSetBool(doc, TidyMark, no);
    tidyOptSetValue(doc, TidyDoctype, "strict");
    TidyBuffer err = {0}; tidySetErrorBuffer(doc, &err);
    TidyBuffer in = {0}; tidyBufAppend(&in, (void *)html, (uint)strlen(html));
    int rc = tidyParseBuffer(doc, &in);
    printf("parse %d\n", rc);
    rc = tidyCleanAndRepair(doc);
    printf("clean %d\n", rc);
    rc = tidyRunDiagnostics(doc);
    printf("diagnostics %d, warnings %d, errors %u\n", rc, tidyWarningCount(doc) > 0, tidyErrorCount(doc));
    printf("xhtml option %d, mark %d\n", tidyOptGetBool(doc, TidyXhtmlOut), tidyOptGetBool(doc, TidyMark));
    printf("root %s, html %s, head %s, body %s\n", tidyNodeGetName(tidyGetRoot(doc)) ? "named" : "unnamed", tidyNodeGetName(tidyGetHtml(doc)), tidyNodeGetName(tidyGetHead(doc)), tidyNodeGetName(tidyGetBody(doc)));
    const char *tags[] = { "title", "p", "br", "table", "tr", "td" };
    for (int i = 0; i < 6; i++) printf("%s: %d\n", tags[i], count(tidyGetRoot(doc), tags[i]));
    for (TidyAttr a = tidyAttrFirst(tidyGetBody(doc)); a; a = tidyAttrNext(a)) printf("body attribute %s\n", tidyAttrName(a));
    TidyBuffer out = {0};
    rc = tidySaveBuffer(doc, &out);
    printf("save %d, %s\n", rc, out.size > 100 ? "output ok" : "output short");
    printf("contains table %d, contains <p>one %d\n", strstr((char *)out.bp, "<table") != 0, strstr((char *)out.bp, "<p>one") != 0);
    tidyBufFree(&out); tidyBufFree(&in); tidyBufFree(&err);
    tidyRelease(doc);
    return 0;
}

// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libxml2: parsing and serialising a document through the host's library, the global variables copied from the host (the allocator, the
// version, the default SAX handler), the error functions with a message that is formatted here, and a generic error handler.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _xmlDoc xmlDoc;
typedef struct _xmlNode xmlNode;
typedef unsigned char xmlChar;
extern const char *xmlParserVersion;
extern void (*xmlFree)(void *);
extern void *(*xmlMalloc)(size_t);
extern int xmlIndentTreeOutput, xmlKeepBlanksDefaultValue, xmlParserDebugEntities;
extern xmlChar xmlStringText[];
extern unsigned char xmlIsPubidChar_tab[256];

xmlDoc *xmlParseMemory(const char *, int);
xmlNode *xmlDocGetRootElement(xmlDoc *);
void xmlDocDumpMemory(xmlDoc *, xmlChar **, int *);
void xmlFreeDoc(xmlDoc *);
void xmlCleanupParser(void);
void xmlSetGenericErrorFunc(void *, void (*)(void *, const char *, ...));
void xmlParserError(void *, const char *, ...);
void xmlParserWarning(void *, const char *, ...);
void xmlGenericErrorDefaultFunc(void *, const char *, ...);
int xmlMemSetup(void (*)(void *), void *(*)(size_t), void *(*)(void *, size_t), char *(*)(const char *));
int xmlMemGet(void (**)(void *), void *(**)(size_t), void *(**)(void *, size_t), char **);

static int calls;
static void handler(void *context, const char *format, ...)
{
    calls++;
}

static void myFree(void *p) { free(p); }

int main(void)
{
    printf("version %d\n", xmlParserVersion != 0 && xmlParserVersion[0] != 0);
    printf("text %s\n", xmlStringText);
    printf("pubid '%d' ' '%d\n", xmlIsPubidChar_tab['a'], xmlIsPubidChar_tab['<']);
    printf("defaults indent %d keep %d debug %d\n", xmlIndentTreeOutput, xmlKeepBlanksDefaultValue, xmlParserDebugEntities);

    const char *text = "<root a=\"1\"><item>one</item><item>two</item></root>";
    xmlDoc *doc = xmlParseMemory(text, (int)strlen(text));
    printf("parsed %d root %d\n", doc != 0, doc && xmlDocGetRootElement(doc) != 0);
    xmlChar *out = 0;
    int size = 0;
    xmlDocDumpMemory(doc, &out, &size);
    printf("dump %d bytes: %s", size, out);
    xmlFree(out);
    xmlFreeDoc(doc);

    xmlSetGenericErrorFunc(0, handler);
    xmlDoc *bad = xmlParseMemory("<a><b></a>", 10);
    printf("bad %d, handler calls %d\n", bad != 0, calls > 0);
    fflush(stdout);
    xmlGenericErrorDefaultFunc(0, "message %d %s\n", 5, "to stderr");

    xmlGenericErrorDefaultFunc(0, 0);
    printf("alloc %d\n", xmlMalloc != 0);
    printf("memsetup %d\n", xmlMemSetup(myFree, malloc, realloc, strdup));
    printf("free changed %d\n", xmlFree == myFree);
    return 0;
}

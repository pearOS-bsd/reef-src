// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libexpat: parse a document with handlers, an error, the version and the error strings.
#include <stdio.h>
#include <string.h>
typedef void *XML_Parser;
XML_Parser XML_ParserCreate(const char *);
void XML_SetUserData(XML_Parser, void *);
void XML_SetElementHandler(XML_Parser, void (*)(void *, const char *, const char **), void (*)(void *, const char *));
void XML_SetCharacterDataHandler(XML_Parser, void (*)(void *, const char *, int));
int XML_Parse(XML_Parser, const char *, int, int);
int XML_GetErrorCode(XML_Parser);
long XML_GetCurrentLineNumber(XML_Parser);
long XML_GetCurrentColumnNumber(XML_Parser);
const char *XML_ErrorString(int);
const char *XML_ExpatVersion(void);
void XML_ParserFree(XML_Parser);

static void start(void *depth, const char *name, const char **attributes)
{
    printf("%*s<%s", 2 * *(int *)depth, "", name);
    for (; *attributes; attributes += 2) printf(" %s=%s", attributes[0], attributes[1]);
    printf(">\n");
    ++*(int *)depth;
}
static void end(void *depth, const char *name) { --*(int *)depth; printf("%*s</%s>\n", 2 * *(int *)depth, "", name); }
static void text(void *depth, const char *s, int n) { printf("%*s'%.*s'\n", 2 * *(int *)depth, "", n, s); }

static void run(const char *document)
{
    int depth = 0;
    XML_Parser parser = XML_ParserCreate("UTF-8");
    XML_SetUserData(parser, &depth);
    XML_SetElementHandler(parser, start, end);
    XML_SetCharacterDataHandler(parser, text);
    int ok = XML_Parse(parser, document, (int)strlen(document), 1);
    printf("ok %d error %d (%s) line %ld column %ld\n", ok, XML_GetErrorCode(parser), XML_ErrorString(XML_GetErrorCode(parser)),
           XML_GetCurrentLineNumber(parser), XML_GetCurrentColumnNumber(parser));
    XML_ParserFree(parser);
}

int main(void)
{
    printf("%s\n", XML_ExpatVersion());
    run("<?xml version=\"1.0\"?><a x=\"1\" y=\"two\"><b>hello</b><c/>\n<d>&amp;&lt;</d></a>");
    run("<a><b></a>");
    return 0;
}

// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libxslt and libexslt: a stylesheet with parameters, keys, sorting, number formatting, extension functions (EXSLT str, math, set, date, func), the security
// preferences and the version numbers. The output must be the same on a Mac and under reef.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libxslt/security.h>
#include <libexslt/exslt.h>

static const char *sheet =
"<xsl:stylesheet version='1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'\n"
"  xmlns:str='http://exslt.org/strings' xmlns:math='http://exslt.org/math' xmlns:set='http://exslt.org/sets'\n"
"  xmlns:date='http://exslt.org/dates-and-times' xmlns:func='http://exslt.org/functions' xmlns:my='urn:my' extension-element-prefixes='func'>\n"
"<xsl:output method='text'/>\n"
"<xsl:param name='title' select=\"'none'\"/>\n"
"<xsl:key name='by-kind' match='item' use='@kind'/>\n"
"<xsl:decimal-format name='eu' decimal-separator=',' grouping-separator='.'/>\n"
"<func:function name='my:twice'><xsl:param name='n'/><func:result select='$n * 2'/></func:function>\n"
"<xsl:template match='/list'>\n"
"title=<xsl:value-of select='$title'/>\n"
"count=<xsl:value-of select='count(item)'/> sum=<xsl:value-of select='sum(item/@n)'/>\n"
"<xsl:for-each select='item'><xsl:sort select='@n' data-type='number' order='descending'/><xsl:value-of select='@name'/>:<xsl:value-of select='@n'/> </xsl:for-each>\n"
"fruit=<xsl:value-of select=\"count(key('by-kind','fruit'))\"/>\n"
"fmt=<xsl:value-of select=\"format-number(1234567.891,'#.##0,00','eu')\"/> <xsl:value-of select=\"format-number(0.5,'##%')\"/>\n"
"tok=<xsl:for-each select=\"str:tokenize('a,b;c','.,;')\"><xsl:value-of select='.'/>|</xsl:for-each>\n"
"pad=<xsl:value-of select=\"str:padding(5,'ab')\"/> up=<xsl:value-of select=\"translate('abc','abc','ABC')\"/>\n"
"math=<xsl:value-of select='math:max(item/@n)'/> <xsl:value-of select='math:min(item/@n)'/> <xsl:value-of select='math:abs(-4.5)'/> <xsl:value-of select='math:power(2,10)'/>\n"
"distinct=<xsl:value-of select='count(set:distinct(item/@kind))'/>\n"
"date=<xsl:value-of select=\"date:year('2026-11-27T10:20:30')\"/> <xsl:value-of select=\"date:month-name('2026-11-27')\"/> <xsl:value-of select=\"date:day-in-week('2026-11-27')\"/> <xsl:value-of select=\"date:add('2026-01-31','P1M')\"/>\n"
"func=<xsl:value-of select='my:twice(21)'/>\n"
"gen=<xsl:value-of select='string-length(generate-id())&gt;0'/> sys=<xsl:value-of select=\"system-property('xsl:vendor')\"/> v=<xsl:value-of select=\"system-property('xsl:version')\"/>\n"
"avail=<xsl:value-of select=\"function-available('str:tokenize')\"/> <xsl:value-of select=\"element-available('xsl:for-each')\"/> <xsl:value-of select=\"function-available('nonexistent')\"/>\n"
"</xsl:template>\n"
"</xsl:stylesheet>\n";

static const char *data =
"<list><item name='apple' kind='fruit' n='3'/><item name='leek' kind='veg' n='12'/><item name='pear' kind='fruit' n='7'/><item name='kale' kind='veg' n='1'/></list>";

int main(void)
{
    xmlInitParser();
    exsltRegisterAll();
    printf("xslt %d %s, exslt %d\n", xsltLibxsltVersion, xsltEngineVersion, exsltLibexsltVersion);
    xmlDocPtr sdoc = xmlReadMemory(sheet, (int)strlen(sheet), "sheet.xsl", NULL, 0);
    xsltStylesheetPtr style = xsltParseStylesheetDoc(sdoc);
    if (!style) { printf("stylesheet failed\n"); return 1; }
    xmlDocPtr doc = xmlReadMemory(data, (int)strlen(data), "data.xml", NULL, 0);
    const char *params[] = { "title", "'Reef & co'", NULL };
    xsltTransformContextPtr ctxt = xsltNewTransformContext(style, doc);
    xsltSecurityPrefsPtr prefs = xsltNewSecurityPrefs();
    xsltSetSecurityPrefs(prefs, XSLT_SECPREF_WRITE_FILE, xsltSecurityForbid);
    xsltSetCtxtSecurityPrefs(prefs, ctxt);
    printf("write forbidden: %d\n", xsltCheckWrite(prefs, ctxt, (const xmlChar *)"/tmp/x") == 0);
    xsltFreeTransformContext(ctxt);
    xsltFreeSecurityPrefs(prefs);
    xmlDocPtr res = xsltApplyStylesheet(style, doc, params);
    xmlChar *out = NULL; int len = 0;
    if (!res || xsltSaveResultToString(&out, &len, res, style) < 0) { printf("transform failed\n"); return 1; }
    printf("result %d bytes\n%s", len, out);
    xmlFree(out);
    /* an error: a stylesheet that is not one */
    xmlDocPtr bad = xmlReadMemory("<a/>", 4, "bad.xsl", NULL, 0);
    xsltStylesheetPtr b = xsltParseStylesheetDoc(bad);
    printf("not a stylesheet: %s\n", b ? "accepted" : "rejected");
    if (!b) xmlFreeDoc(bad);
    xmlFreeDoc(res); xmlFreeDoc(doc); xsltFreeStylesheet(style);
    xsltCleanupGlobals(); xmlCleanupParser();
    return 0;
}

// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// The name pointers of text and comment nodes as libxml2 sets them (see reef-xml.h).
#include <libxml/tree.h>
#include <libxml/parserInternals.h>
#undef xmlStringText
#undef xmlStringComment
__attribute__((visibility("hidden"))) const xmlChar *reef_xml_string_text(void)
{
    static const xmlChar *name;
    if (!name) { xmlNodePtr n = xmlNewText((const xmlChar *)""); name = n->name; xmlFreeNode(n); }
    return name;
}
__attribute__((visibility("hidden"))) const xmlChar *reef_xml_string_comment(void)
{
    static const xmlChar *name;
    if (!name) { xmlNodePtr n = xmlNewComment((const xmlChar *)""); name = n->name; xmlFreeNode(n); }
    return name;
}

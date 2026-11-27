// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Included before the sources of libxslt and libexslt (-include). Reef's libxml2.2 hands the code a copy of the data of the host's libxml2, and the
// libxml2 functions set `name` of the nodes they create to the host's strings: libxslt compares those pointers, so it must ask the library for them.
#include <libxml/tree.h>
#include <libxml/parserInternals.h>
extern const xmlChar *reef_xml_string_text(void);
extern const xmlChar *reef_xml_string_comment(void);
#define xmlStringText reef_xml_string_text()
#define xmlStringComment reef_xml_string_comment()

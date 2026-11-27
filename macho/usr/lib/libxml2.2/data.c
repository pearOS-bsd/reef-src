// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libxml2: the global variables of the library. They live in this image (programs bind to them), so they are copied from the host's
// library when it is loaded and again whenever a function changes them (reef_xml2_refresh_data).

#include <string.h>

extern void *reef_host_symbol(const char *library, const char *name);

#define HOST_LIBRARY "libxml2.so.2"

// Shapes of the host's variables, only as large as they are.
typedef struct { void *words[28]; } SAXHandler;         // xmlSAXHandler, 224 bytes
typedef struct { void *words[4]; } SAXLocator;
typedef struct { int shortCount, longCount; void *shortRanges, *longRanges; } CharGroup;
typedef struct { char bytes[88]; } Error;               // xmlError
typedef struct { char bytes[5]; } Text;
typedef struct { char bytes[10]; } TextNoenc;
typedef struct { char bytes[8]; } Comment;

#define ALIGNED __attribute__((aligned(16)))

int __xmlRegisterCallbacks ALIGNED;
SAXHandler docbDefaultSAXHandler ALIGNED;
SAXHandler htmlDefaultSAXHandler ALIGNED;
int oldXMLWDcompatibility ALIGNED;
int xmlBufferAllocScheme ALIGNED;
int xmlDefaultBufferSize ALIGNED;
SAXHandler xmlDefaultSAXHandler ALIGNED;
SAXLocator xmlDefaultSAXLocator ALIGNED;
void *xmlDeregisterNodeDefaultValue ALIGNED;
int xmlDoValidityCheckingDefaultValue ALIGNED;
void *xmlFree ALIGNED;
void *xmlGenericError ALIGNED;
void *xmlGenericErrorContext ALIGNED;
int xmlGetWarningsDefaultValue ALIGNED;
int xmlIndentTreeOutput ALIGNED;
CharGroup xmlIsBaseCharGroup ALIGNED;
CharGroup xmlIsCharGroup ALIGNED;
CharGroup xmlIsCombiningGroup ALIGNED;
CharGroup xmlIsDigitGroup ALIGNED;
CharGroup xmlIsExtenderGroup ALIGNED;
CharGroup xmlIsIdeographicGroup ALIGNED;
unsigned char xmlIsPubidChar_tab[256] ALIGNED;
int xmlKeepBlanksDefaultValue ALIGNED;
Error xmlLastError ALIGNED;
int xmlLineNumbersDefaultValue ALIGNED;
int xmlLoadExtDtdDefaultValue ALIGNED;
void *xmlMalloc ALIGNED;
void *xmlMallocAtomic ALIGNED;
void *xmlMemStrdup ALIGNED;
void *xmlOutputBufferCreateFilenameValue ALIGNED;
int xmlParserDebugEntities ALIGNED;
void *xmlParserInputBufferCreateFilenameValue ALIGNED;
int xmlParserMaxDepth ALIGNED;
void *xmlParserVersion ALIGNED;
int xmlPedanticParserDefaultValue ALIGNED;
void *xmlRealloc ALIGNED;
void *xmlRegisterNodeDefaultValue ALIGNED;
int xmlSaveNoEmptyTags ALIGNED;
Comment xmlStringComment ALIGNED;
Text xmlStringText ALIGNED;
TextNoenc xmlStringTextNoenc ALIGNED;
void *xmlStructuredError ALIGNED;
void *xmlStructuredErrorContext ALIGNED;
int xmlSubstituteEntitiesDefaultValue ALIGNED;
void *xmlTreeIndentString ALIGNED;
unsigned long xmlXPathNAN ALIGNED;
unsigned long xmlXPathNINF ALIGNED;
unsigned long xmlXPathPINF ALIGNED;

#define COPY_VALUE(variable, name) \
    do { \
        void *host = reef_host_symbol(HOST_LIBRARY, name); \
        if (host) \
            variable = *(__typeof__(variable) *)host; \
    } while (0)

#define COPY_BYTES(variable, name, size) \
    do { \
        void *host = reef_host_symbol(HOST_LIBRARY, name); \
        if (host) \
            memcpy(&variable, host, size); \
    } while (0)

void reef_xml2_refresh_data(void)
{
    COPY_VALUE(__xmlRegisterCallbacks, "__xmlRegisterCallbacks");
    COPY_BYTES(docbDefaultSAXHandler, "docbDefaultSAXHandler", 0xe0);
    COPY_BYTES(htmlDefaultSAXHandler, "htmlDefaultSAXHandler", 0xe0);
    COPY_VALUE(oldXMLWDcompatibility, "oldXMLWDcompatibility");
    COPY_VALUE(xmlBufferAllocScheme, "xmlBufferAllocScheme");
    COPY_VALUE(xmlDefaultBufferSize, "xmlDefaultBufferSize");
    COPY_BYTES(xmlDefaultSAXHandler, "xmlDefaultSAXHandler", 0xe0);
    COPY_VALUE(xmlDefaultSAXLocator, "xmlDefaultSAXLocator");
    COPY_VALUE(xmlDeregisterNodeDefaultValue, "xmlDeregisterNodeDefaultValue");
    COPY_VALUE(xmlDoValidityCheckingDefaultValue, "xmlDoValidityCheckingDefaultValue");
    COPY_VALUE(xmlFree, "xmlFree");
    COPY_VALUE(xmlGenericError, "xmlGenericError");
    COPY_VALUE(xmlGenericErrorContext, "xmlGenericErrorContext");
    COPY_VALUE(xmlGetWarningsDefaultValue, "xmlGetWarningsDefaultValue");
    COPY_VALUE(xmlIndentTreeOutput, "xmlIndentTreeOutput");
    COPY_VALUE(xmlIsBaseCharGroup, "xmlIsBaseCharGroup");
    COPY_VALUE(xmlIsCharGroup, "xmlIsCharGroup");
    COPY_VALUE(xmlIsCombiningGroup, "xmlIsCombiningGroup");
    COPY_VALUE(xmlIsDigitGroup, "xmlIsDigitGroup");
    COPY_VALUE(xmlIsExtenderGroup, "xmlIsExtenderGroup");
    COPY_VALUE(xmlIsIdeographicGroup, "xmlIsIdeographicGroup");
    COPY_BYTES(xmlIsPubidChar_tab, "xmlIsPubidChar_tab", 0x100);
    COPY_VALUE(xmlKeepBlanksDefaultValue, "xmlKeepBlanksDefaultValue");
    COPY_VALUE(xmlLastError, "xmlLastError");
    COPY_VALUE(xmlLineNumbersDefaultValue, "xmlLineNumbersDefaultValue");
    COPY_VALUE(xmlLoadExtDtdDefaultValue, "xmlLoadExtDtdDefaultValue");
    COPY_VALUE(xmlMalloc, "xmlMalloc");
    COPY_VALUE(xmlMallocAtomic, "xmlMallocAtomic");
    COPY_VALUE(xmlMemStrdup, "xmlMemStrdup");
    COPY_VALUE(xmlOutputBufferCreateFilenameValue, "xmlOutputBufferCreateFilenameValue");
    COPY_VALUE(xmlParserDebugEntities, "xmlParserDebugEntities");
    COPY_VALUE(xmlParserInputBufferCreateFilenameValue, "xmlParserInputBufferCreateFilenameValue");
    COPY_VALUE(xmlParserMaxDepth, "xmlParserMaxDepth");
    COPY_VALUE(xmlParserVersion, "xmlParserVersion");
    COPY_VALUE(xmlPedanticParserDefaultValue, "xmlPedanticParserDefaultValue");
    COPY_VALUE(xmlRealloc, "xmlRealloc");
    COPY_VALUE(xmlRegisterNodeDefaultValue, "xmlRegisterNodeDefaultValue");
    COPY_VALUE(xmlSaveNoEmptyTags, "xmlSaveNoEmptyTags");
    COPY_VALUE(xmlStringComment, "xmlStringComment");
    COPY_VALUE(xmlStringText, "xmlStringText");
    COPY_VALUE(xmlStringTextNoenc, "xmlStringTextNoenc");
    COPY_VALUE(xmlStructuredError, "xmlStructuredError");
    COPY_VALUE(xmlStructuredErrorContext, "xmlStructuredErrorContext");
    COPY_VALUE(xmlSubstituteEntitiesDefaultValue, "xmlSubstituteEntitiesDefaultValue");
    COPY_VALUE(xmlTreeIndentString, "xmlTreeIndentString");
    COPY_VALUE(xmlXPathNAN, "xmlXPathNAN");
    COPY_VALUE(xmlXPathNINF, "xmlXPathNINF");
    COPY_VALUE(xmlXPathPINF, "xmlXPathPINF");
}

__attribute__((constructor)) static void fill(void)
{
    reef_xml2_refresh_data();
}

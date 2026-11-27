// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Foundation functions GarageBand imports and Reef's Foundation lacks.
#include <stdlib.h>
#include <stddef.h>

// Collectable memory of the garbage collector: without it, ordinary (zeroed) memory
void *NSAllocateCollectable(unsigned long size, unsigned long options) { return calloc(1, size ? size : 1); }
void *NSReallocateCollectable(void *pointer, unsigned long size, unsigned long options) { return realloc(pointer, size); }

// Apple event descriptors for scripting objects: no conversion
void *_NSAEDescriptorByConvertingObjectOfTypeInSuite(void *object, void *type, void *suite) { return NULL; }

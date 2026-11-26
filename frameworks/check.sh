#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# usage: check.sh Name : symbols the app imports from Name (from missing/<Name>.txt) that out/lib/Name.dylib does not export.
here=$(cd "$(dirname "$0")" && pwd)
n=$1
nm -gU "$here/out/lib/$n.dylib" 2>/dev/null || llvm-nm -g --defined-only "$here/out/lib/$n.dylib"

#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# Build Sources/ with tests/SelfTest.swift into one test library on the Mac, run its entry point and compare the output
# with tests/selftest.expected (layout, environment and state of the rewritten core, worked out by hand from the binary).
set -e
here=$(cd "$(dirname "$0")/.." && pwd)
KEY=${MAC_KEY:-$HOME/fbsd-vm/id_ed25519}
SSH="ssh -p ${MAC_PORT:-2229} -i $KEY -o IdentitiesOnly=yes -o BatchMode=yes -o StrictHostKeyChecking=no ${MAC_USER:-alex}@localhost"
SDK=${MAC_SDK:-/Library/Developer/CommandLineTools/SDKs/MacOSX15.sdk}
R=suire/selftest
$SSH "rm -rf $R && mkdir -p $R"
tar -C "$here" -cf - Sources tests/SelfTest.swift | $SSH "cd $R && tar xf -"
$SSH "cd $R && find Sources -name '*.swift' | sort > files.txt && echo tests/SelfTest.swift >> files.txt && \
 swiftc -target x86_64-apple-macos14.0 -sdk $SDK -enable-library-evolution -module-name SwiftUI -parse-as-library -swift-version 5 \
 -Xfrontend -disable-availability-checking -suppress-warnings -enable-upcoming-feature MemberImportVisibility -Onone -wmo \
 -emit-library -o libSwiftUITest.dylib @files.txt > err.txt 2>&1 || { grep error: err.txt | head; exit 1; }
 printf '#include <dlfcn.h>\n#include <stdio.h>\nint main(){void*h=dlopen(\"./libSwiftUITest.dylib\",RTLD_NOW);if(!h){puts(dlerror());return 1;}((void(*)(void))dlsym(h,\"swiftui_selftest\"))();return 0;}\n' > run.c && \
 clang run.c -o run && ./run 2>&1 | grep -v '^objc\['" > /tmp/swiftui-selftest.out
diff -u "$here/tests/selftest.expected" /tmp/swiftui-selftest.out && echo "selftest: ok"

// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
// Demangle Swift type names that contain symbolic references (as they are stored in the Swift metadata of a binary).
// Runs on the Mac against the toolchain's libswiftDemangle.  Input: one hex-encoded mangled name per line, where a
// symbolic reference is the byte 0x01 followed by a 4-byte index chosen by the caller: k >= 0 a type, -k-1 a protocol,
// 0x1000000 + k an opaque type descriptor.
// Output: the demangler's node tree (as `swift demangle --tree-only`), the reference shown as
// TypeSymbolicReference / ProtocolSymbolicReference with that index.
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace llvm { class StringRef { public: const char *Data; size_t Length; }; }
namespace swift { namespace Demangle {
class Node { public: enum class Kind : uint16_t {}; };
enum class SymbolicReferenceKind : uint8_t {};
enum class Directness : bool {};
class NodeFactory {
public:
  Node *createNode(Node::Kind K);
  Node *createNode(Node::Kind K, unsigned long long Index);
  Node *createNode(Node::Kind K, const char *Text);
};
class Demangler : public NodeFactory {
public:
  Node *createType(Node *Child);
  Node *demangleType(llvm::StringRef MangledName,
                     std::function<Node *(SymbolicReferenceKind, Directness, int, const void *)> Resolver);
};
class Context { public: Context(); ~Context(); Demangler *D; };
std::string getNodeTreeAsString(Node *Root);
}}
using namespace swift::Demangle;

int main() {
  Context ctx;
  Demangler *D = ctx.D;
  // find the numbers of the node kinds we create
  int typeSym = -1, protoSym = -1, opaqueSym = -1;
  for (int k = 0; k < 2000; k++) {
    std::string s = getNodeTreeAsString(D->createNode((Node::Kind)k, 0ULL));
    if (s.find("kind=TypeSymbolicReference,") != std::string::npos) typeSym = k;
    if (s.find("kind=ProtocolSymbolicReference,") != std::string::npos) protoSym = k;
    if (s.find("kind=OpaqueTypeDescriptorSymbolicReference,") != std::string::npos) opaqueSym = k;
    if (typeSym >= 0 && protoSym >= 0 && opaqueSym >= 0) break;
  }
  if (typeSym < 0 || protoSym < 0 || opaqueSym < 0) { fprintf(stderr, "node kinds not found\n"); return 1; }
  std::string line;
  while (std::getline(std::cin, line)) {
    std::string bytes;
    for (size_t i = 0; i + 1 < line.size(); i += 2) bytes.push_back((char)std::stoi(line.substr(i, 2), nullptr, 16));
    llvm::StringRef ref{bytes.data(), bytes.size()};
    // the demangler stores pointers into the text; keep it alive until printed
    Node *n = D->demangleType(ref, [&](SymbolicReferenceKind kind, Directness, int value, const void *) -> Node * {
      if ((uint8_t)kind == 0) {
        if (value >= 0x1000000) return D->createNode((Node::Kind)opaqueSym, (unsigned long long)(value - 0x1000000));
        if (value < 0) return D->createNode((Node::Kind)protoSym, (unsigned long long)(-value - 1));
        return D->createType(D->createNode((Node::Kind)typeSym, (unsigned long long)value));
      }
      return nullptr;
    });
    std::cout << "Demangling for " << line << "\n";
    if (n) std::cout << getNodeTreeAsString(n);
    else std::cout << "<<NULL>>\n";
  }
  return 0;
}

// drv LIB FILE... : compile each Metal file with LIB's compileMsl, print the module or the error
#include <dlfcn.h>
#include <unistd.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

struct MslOptions { int languageVersion = 0; bool fastMath = true; std::vector<std::pair<std::string, std::string>> macros; };
using Fn = std::unique_ptr<llvm::Module> (*)(llvm::LLVMContext &, const std::string &, const MslOptions &, std::string &);

int main(int argc, char **argv)
{
    void *lib = dlopen(argv[1], RTLD_NOW);
    if (!lib) { fprintf(stderr, "%s\n", dlerror()); return 2; }
    Fn *fn = (Fn *)dlsym(lib, "reef_mtl_msl_compiler");
    if (!fn) { fprintf(stderr, "%s\n", dlerror()); return 2; }
    MslOptions base;
    int i = 2;
    for (; i < argc; i++) {
        std::string a = argv[i];
        if (a == "-v") base.languageVersion = (int)strtol(argv[++i], nullptr, 0);
        else if (a == "-nofast") base.fastMath = false;
        else if (a.rfind("-D", 0) == 0) { std::string s = a.substr(2); size_t e = s.find('='); base.macros.push_back({s.substr(0, e), e == std::string::npos ? "" : s.substr(e + 1)}); }
        else break;
    }
    bool bc = getenv("BC") != nullptr;
    for (; i < argc; i++) {
        std::ifstream f(argv[i], std::ios::binary); std::stringstream ss; ss << f.rdbuf();
        llvm::LLVMContext ctx; std::string error = getenv("ERR") ? getenv("ERR") : "";
        MslOptions o = base;
        // options in the file: first lines "//opt -v N -nofast -Dx=y"
        std::string src = ss.str();
        if (src.rfind("//opt ", 0) == 0) {
            std::istringstream is(src.substr(6, src.find('\n') - 6)); std::string a;
            while (is >> a) {
                if (a == "-v") { is >> a; o.languageVersion = (int)strtol(a.c_str(), nullptr, 0); }
                else if (a == "-nofast") o.fastMath = false;
                else if (a.rfind("-D", 0) == 0) { std::string s = a.substr(2); size_t e = s.find('='); o.macros.push_back({s.substr(0, e), e == std::string::npos ? "" : s.substr(e + 1)}); }
            }
        }
        std::unique_ptr<llvm::Module> m = (*fn)(ctx, src, o, error);
        std::string out; llvm::raw_string_ostream os(out);
        os << "=== " << argv[i] << "\n";
        os << "error: [" << error << "]\n";
        if (m) {
            m->print(os, nullptr);
            os << "verify: " << (llvm::verifyModule(*m, &os) ? "broken" : "ok") << "\n";
            if (bc) { std::string b; llvm::raw_string_ostream bs(b); llvm::WriteBitcodeToFile(*m, bs); bs.flush();
                uint64_t h = 1469598103934665603ull; for (unsigned char c : b) { h ^= c; h *= 1099511628211ull; }
                os << "bitcode: " << b.size() << " " << h << "\n"; }
        } else os << "no module\n";
        os.flush();
        fwrite(out.data(), 1, out.size(), stdout);
    }
    fflush(stdout);
    _exit(0);
}

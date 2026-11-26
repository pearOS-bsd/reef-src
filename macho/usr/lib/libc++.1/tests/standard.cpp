// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libc++: strings, containers, streams, number formatting, regex, filesystem, chrono and threads.
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <memory>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <charconv>

int main()
{
    std::string s = "Reef";
    s += " runs Mac apps";
    std::cout << s << " " << s.size() << " " << s.find("Mac") << std::endl;
    std::vector<int> v{5, 3, 9, 1, 7};
    std::sort(v.begin(), v.end());
    std::map<std::string, int> m{{"b", 2}, {"a", 1}, {"c", 3}};
    for (auto &[k, x] : m) std::cout << k << "=" << x << " ";
    for (int x : v) std::cout << x << ",";
    std::cout << "\n" << std::fixed << std::setprecision(3) << 3.14159265 << " " << std::scientific << 12345.678 << " " << std::hex << 255 << std::dec << "\n";
    std::ostringstream o; o << 1.5 << ' ' << 42 << ' ' << true; std::cout << o.str() << "\n";
    std::istringstream in("17 2.25 word"); int i; double d; std::string w; in >> i >> d >> w; std::cout << i << " " << d << " " << w << "\n";
    std::regex re("(\\d+)-(\\w+)"); std::smatch mt; std::string text = "id 123-abc end";
    if (std::regex_search(text, mt, re)) std::cout << mt[1] << " " << mt[2] << "\n";
    char buf[32]; auto r = std::to_chars(buf, buf + 32, 3.5); std::cout << std::string(buf, r.ptr) << "\n";
    std::filesystem::path p = std::filesystem::current_path() / "t" / "file.txt";
    std::filesystem::create_directories(p.parent_path());
    { std::ofstream f(p); f << "hello\nworld\n"; }
    { std::ifstream f(p); std::string line; int n = 0; while (std::getline(f, line)) n++; std::cout << n << " lines, size " << std::filesystem::file_size(p) << ", " << p.filename().string() << "\n"; }
    std::filesystem::remove_all(p.parent_path());
    std::mutex mu; int counter = 0; std::vector<std::thread> threads;
    for (int t = 0; t < 4; t++) threads.emplace_back([&] { for (int k = 0; k < 1000; k++) { std::lock_guard<std::mutex> g(mu); counter++; } });
    for (auto &t : threads) t.join();
    auto t0 = std::chrono::steady_clock::now(); std::this_thread::sleep_for(std::chrono::milliseconds(5));
    std::cout << counter << " " << (std::chrono::steady_clock::now() - t0 >= std::chrono::milliseconds(5)) << "\n";
    auto sp = std::make_shared<std::string>("shared"); std::cout << *sp << " " << sp.use_count() << "\n";
    try { std::string().at(3); } catch (const std::out_of_range &e) { std::cout << "caught out_of_range\n"; }
    return 0;
}

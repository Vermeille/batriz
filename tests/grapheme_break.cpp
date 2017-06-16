#include <fstream>
#include <iostream>

#include <str/str.h>

template <class T>
auto make_string(const T& toks) {
    std::string s;
    for (unsigned int i = 1; i < toks.size(); i += 2) {
        str::append_code_point(s, std::stoi(toks[i], nullptr, 16));
    }
    return s;
}

int main(int argc, char** argv) {
    std::ifstream f(argv[1]);
    for (std::string l; std::getline(f, l);) {
        auto comment = l.substr(std::min(l.find('\t'), l.find('#')));
        l = l.substr(0, std::min(l.find('\t'), l.find('#')));
        if (l.empty()) {
            continue;
        }
        auto ts = str::split(l);
        auto c = std::count(ts.begin(), ts.end(), "÷") - 1;
        auto s = make_string(ts);
        if (str::countchars(s) != c) {
            for (auto& t : ts) {
                std::cout << "." << t;
            }
            std::cout << ".";
            std::cout << "--VS--";
            for (auto g : str::make_graphemes(s)) {
                std::cout << ":" << std::hex;
                for (; g.first != g.second; ++g.first) {
                    std::cout << "." << std::hex << *g.first << "("
                              << read_grapheme_cluster_break(*g.first) << ")";
                }
            }
            std::cout << ":\t\t" << str::countchars(s) << " " << c << "  "
                      << comment << "\n ";
        }
    }
    return 0;
}

#include <sstream>
#include <string>
#include <vector>

namespace str {

template <class T>
T split(const std::string& s, char c) {
    T ss;
    int start = 0;
    while (true) {
        auto found = s.find(c, start);
        if (found == std::string::npos) {
            ss.emplace_back(s.substr(start));
            return ss;
        }
        ss.emplace_back(s.substr(start, found - start));
        start = found + 1;
    }
    return ss;
}

template <class Ts>
std::string join(const std::string& joiner, Ts xs) {
    if (xs.empty()) {
        return "";
    }
    auto it = std::begin(xs);
    std::ostringstream oss;
    oss << *it;
    ++it;
    while (it != std::end(xs)) {
        oss << joiner << *it;
        ++it;
    }
    return oss.str();
}

}  // namespace str

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

}  // namespace str

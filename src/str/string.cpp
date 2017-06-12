#include <cctype>
#include <codecvt>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/locale.hpp>

#include "grapheme_iterator.h"

int read_combining(int);
int read_upper(int);
int read_lower(int);

namespace str {

static const std::string ascii_lowercase = "abcdefghijklmopqrstuvwxyz";
static const std::string ascii_uppercase = "ABCDEFGHIJKLMOPQRSTUVWXYZ";
static const std::string ascii_letters = ascii_lowercase + ascii_uppercase;
static const std::string digits = "0123456789";
static const std::string hexdigits = "0123456789abcdefABCDEF";
static const std::string octdigits = "01234567";
static const std::string punctuation = R"(!"#$%&'()*+,-./:;<=>?@[]^_`{|}~)";
static const std::string whitespace = " \t\n\r\f\v";
static const std::string printable =
    digits + ascii_letters + punctuation + whitespace;

bool is_combining(int cp) { return read_combining(cp) != 0; }

void append_code_point(std::string& s, int cp) {
    if (cp < 0x80) {
        s.push_back(cp);
    } else if (cp < 0x800) {
        s.push_back(0b1100'0000 | (cp >> 6));
        s.push_back(0b1000'0000 | (cp & 0b0011'1111));
    } else if (cp < 0x10000) {
        s.push_back(0b1110'0000 | (cp >> 12));
        s.push_back(0b1000'0000 | ((cp >> 6) & 0b0011'1111));
        s.push_back(0b1000'0000 | (cp & 0b0011'1111));
    } else if (cp < 0x110000) {
        s.push_back(0b1111'0000 | (cp >> 18));
        s.push_back(0b1000'0000 | ((cp >> 12) & 0b0011'1111));
        s.push_back(0b1000'0000 | ((cp >> 6) & 0b0011'1111));
        s.push_back(0b1000'0000 | (cp & 0b0011'1111));
    } else {
        throw std::runtime_error("invalid code_point");
    }
}

std::string upper(const std::string& s) {
    std::string res;
    for (int cp : make_code_points(s)) {
        append_code_point(res, cp + read_upper(cp));
    }
    return res;
}

std::string lower(const std::string& s) {
    std::string res;
    for (int cp : make_code_points(s)) {
        append_code_point(res, cp + read_lower(cp));
    }
    return res;
}

int countchars(const std::string& s) { return make_graphemes(s).size(); }

std::string capitalize(const std::string& text) {
    if (text.empty()) {
        return "";
    }
    using namespace boost::locale::boundary;
    ssegment_index map(character, text.begin(), text.end());
    std::string res;
    res.reserve(text.size());
    ssegment_index::iterator it = map.begin(), e = map.end();
    res += boost::locale::to_upper(std::string(*it));
    ++it;
    for (; it != e; ++it) {
        res += *it;
    }
    return res;
}

std::string casefold(const std::string& text) {
    return boost::locale::fold_case(text);
}

int countcodepoints(const std::string& s) {
    return std::accumulate(s.begin(), s.end(), 0, [](int x, uint8_t c) {
        return x + ((c >> 7 == 0 || c >> 6 == 0b11) ? 1 : 0);
    });
}

std::string center(const std::string& s,
                   int width,
                   const std::string& fill = " ") {
    assert(countchars(fill) == 1);
    auto len = width - countchars(s);
    if (len <= 0) {
        return s;
    }
    auto res = s;
    if (len % 2 == 1) {
        res += fill;
        --len;
    }

    while (len > 0) {
        res = fill + res + fill;
        len -= 2;
    }
    return res;
}

int count(const std::string& s,
          const std::string& sub,
          int start = 0,
          std::string::size_type end = -1) {
    int count = 0;
    while (true) {
        auto pos = s.find(sub, start);
        if (pos == std::string::npos || pos >= end) {
            return count;
        }
        ++count;
        if (end > pos) {
            end -= pos;
        }
        start = pos + sub.size();
    }
}

bool endswith(const std::string& s,
              const std::string& suffix,
              int start = 0,
              std::string::size_type end = -1) {
    end = (end == std::string::npos) ? s.size() : end;
    if (suffix.size() > end - start) {
        return false;
    }
    return std::equal(
        suffix.rbegin(), suffix.rend(), s.rbegin() + (s.size() - end));
}

std::string expandtabs(const std::string& s, int tabsize = 8) {
    std::string res;
    res.reserve(s.size());
    std::string::size_type start = 0;
    while (start < s.size()) {
        auto found = s.find('\t', start);
        res += s.substr(start, found - start);
        if (found == std::string::npos) {
            return res;
        }
        res += ' ';
        while (res.size() % tabsize) {
            res += ' ';
        }
        start = found + 1;
    }
    return res;
}

int find(const std::string& s,
         const std::string& sub,
         int start = 0,
         std::string::size_type end = -1) {
    auto found = s.find(sub, start);
    if (found == std::string::npos) {
        return -1;
    }
    if (end != std::string::npos && found >= end) {
        return -1;
    }
    return found;
}

bool in(const std::string& c, const std::string& s) {
    return s.find(c) != std::string::npos;
}

bool in(char c, const std::string& s) { return s.find(c) != std::string::npos; }

template <class T>
std::string str(T x) {
    return boost::lexical_cast<std::string>(x);
}

template <class... Ts>
std::string format(const std::string& fmt, Ts... args) {
    // FIXME: properly handle format strings
    // https://docs.python.org/3/library/string.html#formatstrings
    std::array<std::string, sizeof...(args)> strs{{str(args)...}};
    std::ostringstream oss;
    int start = 0;
    int auto_nbr = -1;
    while (true) {
        auto bracket = fmt.find('{', start);
        oss << fmt.substr(start, bracket - start);
        if (bracket == std::string::npos) {
            return oss.str();
        }
        if (fmt[bracket + 1] != '}') {
            throw std::invalid_argument("invalid fmt string");
        }
        start = bracket + 2;
        ++auto_nbr;
        oss << strs[auto_nbr];
    }
}

int index(const std::string& s,
          const std::string& pattern,
          int start = 0,
          std::string::size_type end = -1) {
    end = (end == std::string::npos) ? s.size() : end;
    auto pos = s.find(pattern, start);
    if (pos == std::string::npos || pos >= end) {
        throw std::out_of_range(pattern + " not in " +
                                s.substr(start, end - start));
    }
    return pos;
}

#if 0
bool isalnum(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    return std::all_of(s.begin(), s.end(), [](char c) {
            return isalnum(c);
            });
}
#endif

template <class T = std::vector<std::string>>
T split(const std::string& s, char c = ' ') {
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

}  // namespace string

int main() {
    /*
    assert(str::capitalize("toto") == "Toto");
    assert(str::capitalize("çaça") == "Çaça");
    assert(str::capitalize("ßtoto") == "SStoto");
    assert(str::casefold("Éviscérer") == "éviscérer");
    */
    std::cout << str::upper("ßtoto") << "\n";
    assert(str::countchars("ßtoto") == 5);
    assert(str::center("e\u0301", 3) == " e\u0301 ");
    assert(str::center("e\u0301", 4) == " e\u0301  ");
    assert(str::center("+", 8) == "   +    ");
    assert(str::center("-e\u0301e\u0301-", 8) == "  -e\u0301e\u0301-  ");
    assert(!str::endswith("le caca", "cac"));
    assert(str::endswith("le caca", "cac", 0, 6));
    assert(str::expandtabs("01\t012\t0123\t01234") ==
           "01      012     0123    01234");
    assert(str::expandtabs("01\t012\t0123\t01234", 4) ==
           "01  012 0123    01234");
    assert(str::count("ddddcacacccaddd", "ca") == 3);
    assert((str::split("le caca est beau") ==
            std::vector<std::string>{"le", "caca", "est", "beau"}));
    assert(str::make_code_points("$¢€𐍈").size() == 4);
    std::cout << str::upper("le caca e\u0301toile\u0301 σ") << "\n";
    assert(str::lower(str::upper("le caca e\u0301toile\u0301 σ")) ==
           "le caca e\u0301toile\u0301 σ");

    std::string a = "aßtotolae\u0301e\u0301 \r\nu";
    for (auto g : str::make_graphemes(a)) {
        std::cout << "-"
                  << std::string(g.first.str_begin(), g.second.str_begin())
                  << "-\n";
    }

    return 0;
}

#include "str.h"

#include <cctype>
#include <codecvt>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>

#include <boost/lexical_cast.hpp>

#include "grapheme_iterator.h"

int read_combining(int);
int read_upper(int);
int read_lower(int);

namespace str {

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

const std::string ascii_lowercase = "abcdefghijklmopqrstuvwxyz";
const std::string ascii_uppercase = "ABCDEFGHIJKLMOPQRSTUVWXYZ";
const std::string ascii_letters = ascii_lowercase + ascii_uppercase;
const std::string digits = "0123456789";
const std::string hexdigits = "0123456789abcdefABCDEF";
const std::string octdigits = "01234567";
const std::string punctuation = R"(!"#$%&'()*+,-./:;<=>?@[]^_`{|}~)";
const std::string whitespace = " \t\n\r\f\v";
const std::string printable = digits + ascii_letters + punctuation + whitespace;

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

int countcodepoints(const std::string& s) {
    return std::accumulate(s.begin(), s.end(), 0, [](int x, uint8_t c) {
        return x + ((c >> 7 == 0 || c >> 6 == 0b11) ? 1 : 0);
    });
}

std::string center(const std::string& s, int width, const std::string& fill) {
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
          int start,
          std::string::size_type end) {
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
              int start,
              std::string::size_type end) {
    end = (end == std::string::npos) ? s.size() : end;
    if (suffix.size() > end - start) {
        return false;
    }
    return std::equal(
        suffix.rbegin(), suffix.rend(), s.rbegin() + (s.size() - end));
}

std::string expandtabs(const std::string& s, int tabsize) {
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
         int start,
         std::string::size_type end) {
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
          int start,
          std::string::size_type end) {
    end = (end == std::string::npos) ? s.size() : end;
    auto pos = s.find(pattern, start);
    if (pos == std::string::npos || pos >= end) {
        throw std::out_of_range(pattern + " not in " +
                                s.substr(start, end - start));
    }
    return pos;
}

}  // namespace string

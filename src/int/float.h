#include <sstream>

namespace batriz {
namespace floating {

int sign(float f) {
    if (f == 0) {
        return 0;
    }
    return (f < 0) ? -1 : 1;
}

int sign(double f) {
    if (f == 0) {
        return 0;
    }
    return (f < 0) ? -1 : 1;
}

int exp(float f) { return static_cast<int>((f >> 23) & ((1 << 8) - 1)) - 127; }
int exp(double f) {
    return static_cast<int>((f >> 52) & ((1 << 11) - 1)) - 1023;
}

int mantissa(float f) { return static_cast<int>(f & ((1 << 23) - 1)); }
int mantissa(double f) { return static_cast<int>(f & ((1 << 52) - 1)); }

// TODO: as_integer_ratio()
// https://docs.python.org/3/library/stdtypes.html#float.as_integer_ratio
template <class R = long, class T>
std::pair<R, R> as_integer_ratio(T f) {
    int exp;
    T decimal = std::frexp(f, &exp);

    for (int i = 0; i < 300 && decimal != std::floor(decimal); i++) {
        decimal *= 2.0;
        --exp;
    }

    R num = decimal;
    R denum = 1;
    if (exp > 0) {
        num << exp;
    } else {
        denum << exp;
    }
    return std::make_pair(num, denum);
}

// https://docs.python.org/3/library/stdtypes.html#float.is_integer
// TODO
bool is_integer(float f) { return std::trunc(f) == f; }
bool is_integer(double f) { return std::trunc(f) == f; }

// https://docs.python.org/3/library/stdtypes.html#float.hex
// TODO
template <class T>
std::string hex(T f) {
    std::ostringstream oss;
    oss << std::hexfloat << f;
    return oss.str();
}

// https://docs.python.org/3/library/stdtypes.html#float.fromhex
// TODO
template <class T>
T fromhex(const std::string& s) {
    T x;
    std::istringstream(s) >> std::hexfloat >> x;
    return x;
}

}  // namespace floating
}  // namespace batriz

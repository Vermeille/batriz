#include <cassert>
#include <exception>
#include <string>

namespace batriz {
namespace integer {

// http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
template <class T>
T next_power_of_2(T x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    ++x;
    return x;
}

template <class T>
int bit_length(T x) {
    if (x < 0) {
        x = -x;
    }
    int len = 0;
    if (x >= (1 << 16) - 1) {
        x = x >> 16;
        len += 16;
    }
    if (x >= (1 << 8) - 1) {
        x = x >> 8;
        len += 8;
    }
    if (x >= (1 << 4) - 1) {
        x = x >> 4;
        len += 4;
    }
    if (x >= (1 << 2) - 1) {
        x = x >> 2;
        len += 2;
    }
    if (x >= (1 << 1) - 1) {
        x = x >> 1;
        len += 1;
    }
    if (x >= 1) {
        ++len;
    }
    return len;
}

struct OverflowError : public std::exception {};

enum class ByteOrder { Big, Little };

template <class R = std::string, class T>
R to_bytes(T x, int len, ByteOrder order, bool sign = false) {
    int min_bytes = (bit_length(x) + 7) / 8;
    if (x < 0 && !sign) {
        throw OverflowError{};
    }

    if (min_bytes > len) {
        throw OverflowError{};
    }
    min_bytes = std::min(min_bytes, len);
    char fill = (sign && x < 0) ? '\xff' : '\0';
    R ret(len, fill);
    if (order == ByteOrder::Big) {
        std::copy(reinterpret_cast<unsigned char*>(&x),
                  reinterpret_cast<unsigned char*>(&x) + min_bytes,
                  ret.rbegin());
        return ret;
    } else if (order == ByteOrder::Little) {
        std::copy(reinterpret_cast<unsigned char*>(&x),
                  reinterpret_cast<unsigned char*>(&x) + min_bytes,
                  ret.begin());
        return ret;
    }
    assert(false);
}

template <class R = int, class T>
R from_bytes(const T& x, ByteOrder order, bool sign = false) {
    R ret = sign ? -1 : 0;
    auto len = std::min(x.size(), sizeof(ret));
    if (order == ByteOrder::Big) {
        std::copy(std::rbegin(x),
                  std::rbegin(x) + len,
                  reinterpret_cast<unsigned char*>(&ret));
    } else if (order == ByteOrder::Little) {
        std::copy(std::begin(x),
                  std::begin(x) + len,
                  reinterpret_cast<unsigned char*>(&ret));
    }
    return ret;
}

}  // namespace integer
}  // namespace batriz

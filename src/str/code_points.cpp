#include "code_points.h"

#include <algorithm>
#include <stdexcept>

namespace str {
namespace impl {

int code_point_iterator::decode_cpt() const {
    if ((*pos_ & 0b1000'0000) == 0) {
        return *pos_;
    } else if ((*pos_ & 0b1110'0000) == 0b1100'0000) {
        return ((to_uc(pos_[0]) & 0b0001'1111) << 6) |
               (to_uc(pos_[1]) & 0b0011'1111);
    } else if ((*pos_ & 0b1111'0000) == 0b1110'0000) {
        return ((to_uc(pos_[0]) & 0b0000'1111) << 12) |
               ((to_uc(pos_[1]) & 0b0011'1111) << 6) |
               ((to_uc(pos_[2]) & 0b0011'1111));
    } else if ((*pos_ & 0b1111'1000) == 0b1111'0000) {
        return ((pos_[0] & 0b0000'0111) << 18) |
               ((pos_[1] & 0b0011'1111) << 12) |
               ((pos_[2] & 0b0011'1111) << 6) | (pos_[3] & 0b0011'1111);
    }
    throw std::runtime_error("invalid UTF-8 encoding");
}

code_point_iterator code_point_iterator::operator++() {
    if ((*pos_ & 0b1000'0000) == 0) {
        ++pos_;
    } else if ((*pos_ & 0b1110'0000) == 0b1100'0000) {
        pos_ += 2;
    } else if ((*pos_ & 0b1111'0000) == 0b1110'0000) {
        pos_ += 3;
    } else if ((*pos_ & 0b1111'1000) == 0b1111'0000) {
        pos_ += 4;
    }
    return *this;
}

std::string::const_iterator code_point_iterator::str_end() const {
    if ((*pos_ & 0b1000'0000) == 0) {
        return pos_ + 1;
    } else if ((*pos_ & 0b1110'0000) == 0b1100'0000) {
        return pos_ + 2;
    } else if ((*pos_ & 0b1111'0000) == 0b1110'0000) {
        return pos_ + 3;
    } else if ((*pos_ & 0b1111'1000) == 0b1111'0000) {
        return pos_ + 4;
    }
    return pos_;
}

}  // namespace impl
}  // namespace str

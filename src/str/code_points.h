#pragma once

#include <algorithm>
#include <string>

namespace str {
namespace impl {

class code_point_iterator {
    std::string::const_iterator pos_;

    unsigned char to_uc(char x) const { return x; }

    int decode_cpt() const {
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

   public:
    using iterator_category = std::forward_iterator_tag;

    code_point_iterator() = default;
    code_point_iterator(std::string::const_iterator p) : pos_(p) {}
    code_point_iterator(const code_point_iterator&) = default;

    code_point_iterator operator=(const code_point_iterator& it) {
        pos_ = it.pos_;
        return *this;
    }
    ~code_point_iterator() = default;

    int operator*() const { return decode_cpt(); }

    code_point_iterator operator++() {
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

    bool operator==(code_point_iterator o) const { return pos_ == o.pos_; }
    bool operator!=(code_point_iterator o) const { return pos_ != o.pos_; }

    code_point_iterator operator++(int) {
        auto p = *this;
        ++pos_;
        return p;
    }
};

}  // namespace impl

}  // namespace str

namespace std {
template <>
struct iterator_traits<str::impl::code_point_iterator> {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::string::iterator::difference_type;
    using value_type = std::string::iterator::value_type;
    using pointer = std::string::iterator::pointer;
    using reference = std::string::iterator::reference;
};
}

namespace str {
struct const_code_points {
   private:
    const std::string& s_;

   public:
    using const_iterator = impl::code_point_iterator;

    const_code_points(const std::string& s) : s_(s) {}

    const_iterator begin() const { return const_iterator(std::cbegin(s_)); }
    const_iterator end() const { return const_iterator(std::cend(s_)); }

    int size() const { return std::distance(begin(), end()); }
};

const_code_points make_code_points(const std::string& s) {
    return const_code_points(s);
}

}  // namespace str

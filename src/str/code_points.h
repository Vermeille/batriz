#pragma once

#include <algorithm>
#include <string>

namespace str {
namespace impl {

template <class StrIt>
class code_point_iterator {
    StrIt pos_;

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
    code_point_iterator(StrIt p) : pos_(p) {}

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
        operator++();
        return p;
    }
};

}  // namespace impl

template <class Str>
struct code_points {
   private:
    Str& s_;

   public:
    using iterator = impl::code_point_iterator<std::string::iterator>;
    using const_iterator =
        impl::code_point_iterator<std::string::const_iterator>;

    code_points(Str& s) : s_(s) {}

    const_iterator cbegin() const { return const_iterator(std::cbegin(s_)); }
    const_iterator cend() const { return const_iterator(std::cend(s_)); }

    iterator begin() { return iterator(std::begin(s_)); }
    iterator end() { return iterator(std::end(s_)); }

    int size() const { return std::distance(cbegin(), cend()); }
};

code_points<const std::string> make_code_points(const std::string& s) {
    return code_points<const std::string>(s);
}

code_points<std::string> make_code_points(std::string& s) {
    return code_points<std::string>(s);
}

}  // namespace str

namespace std {
template <class Str>
struct iterator_traits<str::impl::code_point_iterator<Str>>
    : public iterator_traits<Str> {
    using iterator_category = std::forward_iterator_tag;
};
}

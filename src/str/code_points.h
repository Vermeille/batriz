#pragma once

#include <string>

namespace str {
namespace impl {

class code_point_iterator {
    std::string::const_iterator pos_;

    unsigned char to_uc(char x) const { return x; }

    int decode_cpt() const;

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

    code_point_iterator operator++();

    bool operator==(code_point_iterator o) const { return pos_ == o.pos_; }
    bool operator!=(code_point_iterator o) const { return pos_ != o.pos_; }

    code_point_iterator operator++(int) {
        auto p = *this;
        ++pos_;
        return p;
    }

    std::string::const_iterator str_begin() const { return pos_; }
    std::string::const_iterator str_end() const;
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

inline const_code_points make_code_points(const std::string& s) {
    return const_code_points(s);
}

}  // namespace str

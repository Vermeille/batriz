#pragma once

#include <algorithm>
#include <string>

#include "code_points.h"

int read_grapheme_cluster_break(int cp);

#include "grapheme_cluster_break_automaton.h"

namespace str {
namespace impl {

class grapheme_iterator {
    std::pair<code_point_iterator, code_point_iterator> pos_;
    code_point_iterator end_;

    bool belongs_to_emoji_sequence(int);

   public:
    grapheme_iterator(const code_point_iterator& end)
        : pos_{end, end}, end_(end) {}

    grapheme_iterator(const code_point_iterator& start,
                      const code_point_iterator& end)
        : pos_({start, start}), end_(end) {
        ++*this;
    }

    grapheme_iterator operator++() {
        pos_.first = pos_.second;
        STATE s = STATE_sot;
        while (true) {
            if (pos_.second == end_) {
                return *this;
            }
            auto prop = read_grapheme_cluster_break(*pos_.second);
            s = next_state(s, prop);
            if (s == STATE_BREAK) {
                return *this;
            }
            ++pos_.second;
        }
    }

    bool operator==(const grapheme_iterator& o) const {
        return pos_.first == o.pos_.first;
    }
    bool operator!=(const grapheme_iterator& o) const {
        return pos_.first != o.pos_.first;
    }

    grapheme_iterator operator++(int) {
        auto p = *this;
        ++*this;
        return p;
    }

    std::pair<code_point_iterator, code_point_iterator> operator*() const {
        return pos_;
    }

    std::string::const_iterator str_begin() const {
        return pos_.first.str_begin();
    }

    std::string::const_iterator str_end() const {
        return pos_.second.str_begin();
    }
};

}  // namespace impl
}  // namespace str

namespace std {
template <>
struct iterator_traits<str::impl::grapheme_iterator> {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::string::iterator::difference_type;
    using value_type = std::string::iterator::value_type;
    using pointer = std::string::iterator::pointer;
    using reference = std::string::iterator::reference;
};
}

namespace str {
struct graphemes {
   private:
    const std::string& s_;

   public:
    using const_iterator = impl::grapheme_iterator;

    graphemes(const std::string& s) : s_(s) {}

    const_iterator begin() const {
        return impl::grapheme_iterator(
            const_iterator(std::cbegin(s_), std::cend(s_)));
    }
    const_iterator end() const {
        return impl::grapheme_iterator(const_iterator(std::cend(s_)));
    }

    int size() const { return std::distance(begin(), end()); }
};

inline graphemes make_graphemes(const std::string& s) { return graphemes(s); }

}  // namespace str

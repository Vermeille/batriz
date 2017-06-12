#pragma once

#include <algorithm>
#include <string>

#include "code_points.h"

int read_grapheme_cluster_break(int cp);

namespace str {
namespace impl {

class GRAPHEME_CLUSTER_BREAK {
   public:
    enum {
        CR,
        LF,
        Control,
        Extend,
        ZWJ,
        Regional_Indicator,
        Prepend,
        SpacingMark,
        L,
        V,
        T,
        LV,
        LVT,
        E_Base,
        E_Modifier,
        Glue_After_Zwj,
        E_Base_GAZ,
        Any
    };
};

class grapheme_iterator {
    std::pair<code_point_iterator, code_point_iterator> pos_;
    code_point_iterator end_;
    bool emoji_;     // used for GB10
    int reg_indic_;  // For GB12/13

    bool belongs_to_emoji_sequence(int cp) {
        // helper for GB10
        if (cp == GRAPHEME_CLUSTER_BREAK::E_Base ||
            cp == GRAPHEME_CLUSTER_BREAK::E_Base_GAZ) {
            return true;
        } else if (cp == GRAPHEME_CLUSTER_BREAK::Extend) {
            return true;
        } else {
            return false;
        }
    }

   public:
    grapheme_iterator(const code_point_iterator& end)
        : pos_{end, end}, end_(end) {}

    grapheme_iterator(const code_point_iterator& start,
                      const code_point_iterator& end)
        : pos_({start, start}),
          end_(end),
          emoji_(belongs_to_emoji_sequence(*pos_.first)),
          reg_indic_(0) {
        if (read_grapheme_cluster_break(*pos_.first) ==
            GRAPHEME_CLUSTER_BREAK::Regional_Indicator) {
            reg_indic_ = 1;
        }
        ++*this;
    }

    grapheme_iterator operator++() {
        // http://www.unicode.org/reports/tr29/tr29-29.html#GB1
        pos_.first = pos_.second;
        int prev_type = read_grapheme_cluster_break(*pos_.first);
        while (true) {
            //////////// TR 29 RULES ///////////////////////
            // GB1 GB2
            if (pos_.second == end_) {
                return *this;
            }
            // helpers
            emoji_ = emoji_ && belongs_to_emoji_sequence(*pos_.second);

            if (read_grapheme_cluster_break(*pos_.second) ==
                GRAPHEME_CLUSTER_BREAK::Regional_Indicator) {
                ++reg_indic_;
            } else {
                reg_indic_ = 0;
            }

            ++pos_.second;
            int c2 = read_grapheme_cluster_break(*pos_.second);

            // GB3
            if (prev_type == GRAPHEME_CLUSTER_BREAK::CR &&
                c2 == GRAPHEME_CLUSTER_BREAK::LF) {
                goto next;
                // GB4
            } else if (prev_type == GRAPHEME_CLUSTER_BREAK::CR ||
                       prev_type == GRAPHEME_CLUSTER_BREAK::LF ||
                       prev_type == GRAPHEME_CLUSTER_BREAK::Control) {
                return *this;
                // GB5
            } else if (c2 == GRAPHEME_CLUSTER_BREAK::CR ||
                       c2 == GRAPHEME_CLUSTER_BREAK::LF ||
                       c2 == GRAPHEME_CLUSTER_BREAK::Control) {
                return *this;
                // TODO: GB6,7,8
                // GB9
            } else if (c2 == GRAPHEME_CLUSTER_BREAK::Extend ||
                       c2 == GRAPHEME_CLUSTER_BREAK::ZWJ) {
                goto next;
                // GB9a
            } else if (c2 == GRAPHEME_CLUSTER_BREAK::SpacingMark) {
                goto next;
                // GB9b
            } else if (prev_type == GRAPHEME_CLUSTER_BREAK::Prepend) {
                goto next;
                // GB10
            } else if (emoji_ && c2 == GRAPHEME_CLUSTER_BREAK::E_Modifier) {
                goto next;
                // GB11
            } else if (prev_type == GRAPHEME_CLUSTER_BREAK::ZWJ &&
                       (c2 == GRAPHEME_CLUSTER_BREAK::Glue_After_Zwj ||
                        c2 == GRAPHEME_CLUSTER_BREAK::E_Base_GAZ)) {
                goto next;
                // GB12 / 13
            }
            if (reg_indic_ && reg_indic_ % 2 == 1 &&
                c2 == GRAPHEME_CLUSTER_BREAK::Regional_Indicator) {
                goto next;
                // GB999
            } else {
                return *this;
            }

        next:
            prev_type = c2;
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

graphemes make_graphemes(const std::string& s) { return graphemes(s); }

}  // namespace str

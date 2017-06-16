#include "grapheme_iterator.h"

namespace str {
namespace impl {

bool grapheme_iterator::belongs_to_emoji_sequence(int cp) {
    // helper for GB10
    if (cp == GRAPHEME_CLUSTER_BREAK::E_Base ||
        cp == GRAPHEME_CLUSTER_BREAK::E_Base_GAZ) {
        return true;
    } else {
        return false;
    }
}

}  // namespace impl
}  // namespace str

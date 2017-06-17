#pragma once

namespace str {
namespace impl {

enum GRAPHEME_CLUSTER_BREAK {
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
    Any,
    eot
};

enum STATE {
    STATE_BREAK,
    STATE_sot,
    STATE_eot,
    STATE_CR,
    STATE_LF,
    STATE_Control,
    STATE_L,
    STATE_V_or_LV,
    STATE_T_or_LVT,
    STATE_Prepend,
    STATE_ZWJ,
    STATE_Emoji,
    STATE_RI_1,
    STATE_RI_2,
    STATE_Any,
};

STATE next_state(STATE s, int prop);
}
}

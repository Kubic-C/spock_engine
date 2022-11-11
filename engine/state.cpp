#include "state.hpp"

namespace spk {
    state_t state;

    double state_t::_get_target_fps(bool divide_by_second) { 
        if(divide_by_second) {
            return 1.0 / target_fps_;
        } else {
            return target_fps_;
        }
    }

    double state_t::_get_target_tps(bool divide_by_second) { 
        if(divide_by_second) {
            return 1.0 / target_tps_;
        } else {
            return target_tps_;
        }
    }
}
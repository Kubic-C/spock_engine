#include "state.hpp"

namespace spk {
    state_t state;
    stats_t stats;
 
    const char* vsync_name(vsync_setting_e opt) {
        switch(opt) {
            case VSYNC_ADAPATIVE:
                return "VSYNC_ADAPTIVE";
            
            case VSYNC_DISABLED:
                return "VSYNC_DISABLED";
            
            case VSYNC_ENABLED:
                return "VSYNC_ENABLED";
        
            default:
                return "VSYNC_UKNOWN";
        }
    } 

    double state_t::get_target_fps(bool divide_by_second) const { 
        if(divide_by_second) {
            return 1.0 / target_fps_;
        } else {
            return target_fps_;
        }
    }

    double state_t::get_target_tps(bool divide_by_second) const { 
        if(divide_by_second) {
            return 1.0 / target_tps_;
        } else {
            return target_tps_;
        }
    }
}
#include "internal.hpp"

namespace spk {
    internal_t* internal = nullptr;
 
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
}
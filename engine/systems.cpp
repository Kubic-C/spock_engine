#include "systems.hpp"

namespace spk {    
    bool systems_ascending(system_tt* _1, system_tt* _2) {
        return _1->sorting_order < _2->sorting_order;
    }

    void system_manager_tt::push_system(system_tt* system) {
        systems.push_back(system);
        std::sort(systems.begin(), systems.end(), &systems_ascending);
    }

    void system_manager_tt::update(scene_tt& scene, float deltatime) {
        for(system_tt* sys : systems) {
            sys->update(scene, deltatime);
        }
    }

    void system_manager_tt::tick(scene_tt& scene, float deltatime) {
        for(system_tt* sys : systems) {
            sys->tick(scene, deltatime);
        }
    }

    void system_manager_tt::free(scene_tt& scene) {
        for(system_tt* sys : systems) {
            sys->free(scene);
        }        
    }

    void system_manager_tt::free_user_systems(spk::scene_tt& scene) {
        int32_t i = -1;

        for(system_tt* sys : systems) {
            if(sys->sorting_order > USER_SORTING_ORDER4) {
                break;
            } else {
                sys->free(scene);
                i++;
            }
        }    

        if(i > -1)
            systems.erase(systems.begin() + i);
    }

    void render_system_manager_tt::push_system(render_system_tt* system) {
        systems.push_back(system);
    }
    
    void render_system_manager_tt::free() {
        for(render_system_tt* sys : systems) {
            sys->free();
        } 
    }
}
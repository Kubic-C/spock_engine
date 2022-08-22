#include "systems.hpp"

namespace spk {    
    bool systems_ascending(system_t* _1, system_t* _2) {
        return _1->sorting_order < _2->sorting_order;
    }

    void system_manager_t::push_system(system_t* system) {
        systems.push_back(system);
        std::sort(systems.begin(), systems.end(), &systems_ascending);
    }

    void system_manager_t::update(scene_t& scene, float deltatime) {
        for(system_t* sys : systems) {
            sys->update(scene, deltatime);
        }
    }

    void system_manager_t::tick(scene_t& scene, float deltatime) {
        for(system_t* sys : systems) {
            sys->tick(scene, deltatime);
        }
    }

    void system_manager_t::free(scene_t& scene) {
        for(system_t* sys : systems) {
            sys->free(scene);
        }        
    }

    void system_manager_t::free_user_systems(spk::scene_t& scene) {
        int32_t i = -1;

        for(system_t* sys : systems) {
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

    void render_system_manager_t::push_system(render_system_t* system) {
        systems.push_back(system);
    }
    
    void render_system_manager_t::free() {
        for(render_system_t* sys : systems) {
            sys->free();
        } 
    }
}
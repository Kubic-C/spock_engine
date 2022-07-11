#include "systems.hpp"

namespace spk {    
    bool systems_ascending(system_tt* _1, system_tt* _2) {
        return _1->sorting_order < _2->sorting_order;
    }

    void system_manager_tt::push_system(system_tt* system) {
        systems.push_back(system);
        std::sort(systems.begin(), systems.end(), &systems_ascending);
        system->bus = &messages;;
    }

    void system_manager_tt::msg_update(scene_tt& scene, float deltatime) {
        if(messages.empty())
            return; 

        message_tt& msg = messages.front();
        for(system_tt* sys : systems) {
            sys->handle_message(scene, deltatime, msg);
        }

        messages.pop();        
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

    void system_manager_tt::free() {
        for(system_tt* sys : systems) {
            sys->free();
        }        
    }

    void system_manager_tt::free_user_systems() {
        int32_t i = -1;

        for(system_tt* sys : systems) {
            if(sys->sorting_order > USER_SORTING_ORDER4) {
                break;
            } else {
                sys->free();
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
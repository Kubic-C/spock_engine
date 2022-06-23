#include "systems.hpp"

namespace spk {    
    void system_manager_tt::push_system(system_tt* system) {
        systems.push_back(system);
        system->bus = &messages;;
    }

    void system_manager_tt::msg_update() {
        if(messages.empty())
            return; 

        message_tt& msg = messages.front();
        for(system_tt* sys : systems) {
            sys->handle_message(msg);
        }

        messages.pop();        
    }

    void system_manager_tt::update(flecs::world& world) {
        for(system_tt* sys : systems) {
            sys->update(world);
        }
    }

    void system_manager_tt::tick(flecs::world& world) {
        for(system_tt* sys : systems) {
            sys->tick(world);
        }
    }

    void system_manager_tt::free() {
        for(system_tt* sys : systems) {
            sys->free();
        }        
    }
}
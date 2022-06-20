#include "systems.hpp"

namespace spk {
    void console::handle_message() {
        message message;
        if(!bus->get_last_message(message))
            return;

#ifndef NDEBUG
            std::cout << "| message.code: " << message.code << "\n| message.str: " << message.str << std::endl;
#endif

        switch(message.code) {
        case 0:
            std::cout << "console echo" << std::endl;
            break;
        default:
            break;
        }
    }
        
    void console::user_prompt() {
        std::string str;
        std::cout << "game>";
        std::getline(std::cin, str);

        bus->post_message({std::stoul(str), "User Message"});
    }
    
    void system_manager::push_system(system* system) {
        systems.push_back(system);
    }

    void system_manager::free_systems() {
        systems.clear();
    }

    void system_manager::update() {
        if(bus->empty())
            return; 

        for(int i = 0; i < systems.size(); i++) {
            systems[i]->handle_message();
        }

        bus->pop_message();
    }
}
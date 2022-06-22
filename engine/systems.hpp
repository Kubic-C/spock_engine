#pragma once
#include "framework.hpp"

namespace spk {
    struct message_tt {
        uint32_t code; // message code
        std::string str; // may be empty
    };

    struct system_tt {
        std::string name;
        std::queue<message_tt>* bus;

        // called by system manager (part of game engine)
        virtual void handle_message(message_tt& message) {};
        virtual void init(void* data) {};
        virtual void update() {};
        virtual void tick() {};
        virtual void free() {};
    };

    /* handles updating and keeping track of the active systems*/
    struct system_manager_tt {
        std::vector<system_tt*> systems;
        std::queue<message_tt> messages;

        void push_system(system_tt* system);

        void msg_update();
        void update();
        void tick();
        void free();
    };
}
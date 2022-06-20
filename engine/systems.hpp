#pragma once
#include "messages.hpp"

namespace spk {
    class system {
    protected:
        std::string name;
        message_bus* bus;

    public:
        virtual void handle_message() {
            assert(!("The use of the base system class is not allowed!"));
        }

        inline void set_bus(message_bus* bus) {
            this->bus = bus;
        }
    };

    class console : public system  {
    public:
        void handle_message();
        
        void user_prompt();
    };

    /* handles updating and keeping track of the active systems*/
    class system_manager {
    protected:
        std::vector<system*> systems;
        message_bus* bus;

        void free_systems();
    public:
        void push_system(system* system);

        void update();

        inline void set_buses(message_bus* bus) {
            for(int i = 0; i < systems.size(); i++) {
                systems[i]->set_bus(bus);
            }

            this->bus = bus;
        }
    };
}
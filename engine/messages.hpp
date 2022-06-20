#pragma once
#include "framework.hpp"

namespace spk {
    struct message {
        uint32_t code; // message code
        std::string str; // may be empty
    };

    class message_bus {
    private:
        std::queue<message> messages;
    
    public:
        inline bool get_last_message(message& message) {
            if(!messages.empty()) {
                message = messages.front();
                return true;
            } else {
                return false;
            }

        }

        inline void pop_message() {
            if(!messages.empty())
                messages.pop();
        }
        
        inline void post_message(message message) {
            messages.push(message);
        }

        inline bool empty() {
            return messages.empty();
        }
    };
}
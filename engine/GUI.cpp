#include "GUI.hpp"

namespace spk {
    void GUI::handle_message() {
        message message;
        if(!bus->get_last_message(message))
            return;

        switch(message.code) {
        case 0:
            break;
        }

    }
}
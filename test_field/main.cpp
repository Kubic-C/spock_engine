#include "state.hpp"
#include "spock.hpp"
#include "utility/image_loader.hpp"

enum current_state_e: uint32_t {
    STATE_MENU,
    STATE_LOAD,
    STATE_PLAY,
    STATE_EXIT_PLAY,
    STATE_EXIT
};

MAIN {
    spk_trace();
    
    int           exit_code = 0;
    spk::engine_t engine;

    engine.init();

    exit_code = engine.run();
    engine.free();

    return exit_code;
}
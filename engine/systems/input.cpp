#include "input.hpp"

namespace spk {

    void char_callback(void* self, uint32_t codepoint) {
        input_tt* input = (input_tt*)self;
    
        input->bus->push(spk::message_tt(codepoint, "char callback"));
    }

    input_tt::input_tt() {

    }

    void input_tt::init(scene_tt& scene, void* data) {
        scene.window->char_callback.data = this;
        scene.window->char_callback.fp_callback = char_callback;
    }

    void input_tt::update(scene_tt& scene, float deltatime) {

    }

    void input_tt::tick(scene_tt& scene, float deltatime) {

    }

    void input_tt::free() {

    }
}
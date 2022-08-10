#include "ui_.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    void ui_canvas_tt::init() {
        DEBUG_VALUE(bool, ret =) texts.init(sfk::xor_int_hash<sfk::key_tt>);
        assert(ret);
        DEBUG_EXPR(ret =) buttons.init(sfk::xor_int_hash<sfk::key_tt>);
        assert(ret);

        font = UINT32_MAX;
        // vp = glm::identity<glm::mat4>(); this will be set in a resize
    }

    void ui_canvas_tt::resize(int width, int height) {
        glm::mat4 view, proj;

        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        proj = glm::ortho(0.0f, (float)width, 0.0f, (float)height, 0.01f, 100.0f);

        vp = proj * view;
    }

    void ui_canvas_tt::free() {
        texts.free();
    }

}
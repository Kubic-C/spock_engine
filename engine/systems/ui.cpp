#include "ui.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    inline GLFWwindow* window;

    void ui_t::init(scene_t& scene, void* data) {
        glm::mat4 view, proj;

        window = scene.window->c_window;

        scene.canvas.init();
        this->scene = &scene;
    }

    void ui_t::update(scene_t& scene, float deltatime) {
    }

    void ui_t::tick(scene_t& scene, float deltatime) {
    }

    void ui_t::free(scene_t& scene) {
        scene.canvas.free();
    }

    void ui_t::mouse_button_callback(sfk::window_t* window, void* vself, int button, int action, int mods) {
        ui_t* self = (ui_t*)vself;
        int width, height;
        double x, y;

        glfwGetCursorPos(window->c_window, &x, &y);
        glfwGetWindowSize(window->c_window, &width, &height);
        
        y = height - y; // GLFW XY -> CANVAS XY

        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            auto check_button = [&](ui_button_t& btn) {
                glm::vec2 corners[2] = {
                    btn.abs_pos - btn.abs_size,
                    btn.abs_pos + btn.abs_size
                };

                if(!(btn.flags & spk::UI_ELEMENT_FLAGS_ENABLED))
                    return false;

                if(corners[0].x < x && x < corners[1].x &&
                   corners[0].y < y && y < corners[1].y) {
                    
                    btn.time_when_clicked = glfwGetTime();

                    if(btn.callback) {
                        btn.callback(*self->scene, &btn);
                    }

                    return true;
                }

                return false;
            };

            self->scene->canvas.btns.get_valid_blocks(nullptr, UINT32_MAX, check_button);
        }
    }
}
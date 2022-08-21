#include "ui.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    inline GLFWwindow* window;

    void ui_tt::init(scene_tt& scene, void* data) {
        glm::mat4 view, proj;

        window = scene.window->c_window;

        scene.canvas.init();
        this->scene = &scene;
    }

    void ui_tt::update(scene_tt& scene, float deltatime) {
    }

    void ui_tt::tick(scene_tt& scene, float deltatime) {
    }

    void ui_tt::free(scene_tt& scene) {
        scene.canvas.free();
    }

    void ui_tt::mouse_button_callback(sfk::window_tt* window, void* vself, int button, int action, int mods) {
        ui_tt* self = (ui_tt*)vself;
        int width, height;
        double x, y;

        glfwGetCursorPos(window->c_window, &x, &y);
        glfwGetWindowSize(window->c_window, &width, &height);
        
        y = height - y; // GLFW XY -> CANVAS XY

        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            auto check_button = [&](ui_button_tt& btn) {
                glm::vec2 corners[2] = {
                    btn.abs_pos - btn.abs_size,
                    btn.abs_pos + btn.abs_size
                };

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
#include "ui.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    inline GLFWwindow* window;

    void func(scene_tt& scene, ui_button_tt* btn) {
        glfwSetWindowShouldClose(window, true);

        printf("they are??? %p \n", btn);
    }

    void ui_tt::init(scene_tt& scene, void* data) {
        glm::mat4 view, proj;

        window = scene.window->c_window;

        scene.canvas.init();
        this->scene = &scene;

        ui_button_tt* btn = this->scene->canvas.init_button(0, (ui_axises_tt){.position = { 10.0f, 10.0f }, .size = { 200.0f, 30.0f}}, 
            (glm::vec3){ 1.0f, 1.0f, 1.0f }, func);
        btn->axises.set({10.0f, 10.0f}, { 400.0f, 100.0f });
    
        this->scene->canvas.init_text(0, (ui_axises_tt){.position = { 0.0f, 0.0f }}, (glm::vec3){0.0f, 0.0f, 0.0f}, 
            "width relavance is cool!", &btn->axises);
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
            ui_button_tt* ui_buttons[24];
            uint32_t size;

            size = self->scene->canvas.buttons.get_all_data(ui_buttons, 24);

            // not very optimized menu Cursor Button checking but it doesnt need such a big optimization
            for(uint32_t i = 0; i < size; i++) {
                ui_button_tt* button = ui_buttons[i];

                if(button->axises.position.x < x && x < button->axises.position.x + button->axises.size.x &&
                   button->axises.position.y < y && y < button->axises.position.y + button->axises.size.y) {
                    
                    button->time_when_clicked = glfwGetTime();
                    
                    if(button->callback) {
                        button->callback(*self->scene, button);
                    }
                }
            }
        }
    }
}
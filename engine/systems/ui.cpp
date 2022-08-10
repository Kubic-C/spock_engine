#include "ui.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    void func() {
        printf("BUTTON PRESSED\n");

        printf("gay peni\n");
    }

    void ui_tt::init(scene_tt& scene, void* data) {
        glm::mat4 view, proj;

        scene.canvas.init();
        canvas = &scene.canvas;

        scene.canvas.texts.register_key(text_key);
        ui_text_tt* text = &scene.canvas.texts[text_key];
        text->str = ";p gay penis";
        text->position = {0.0f, 0.0f};
        text->color = {1.0f, 0.0f, 0.05f};

        scene.canvas.buttons.register_key(button_key);
        ui_button_tt* button = &scene.canvas.buttons[button_key];
        button->position = {100.0f, 100.0f};
        button->size     = {200.0f, 40.0f};
        button->color    = {1.0f, 1.0f, 1.0f};
        button->callback = func;
        button->text     = text;
        text->button     = button;

        scene.canvas.buttons.register_key(button_key + 1);
        button = &scene.canvas.buttons[button_key + 1];
        button->position = {500.0f, 100.0f};
        button->size     = {200.0f, 40.0f};
        button->color    = {1.0f, 1.0f, 1.0f};
        button->callback = func;
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

            size = self->canvas->buttons.get_all_data(ui_buttons, 24);

            // not very optimized menu Cursor Button checking but it doesnt need such a big optimization
            for(uint32_t i = 0; i < size; i++) {
                ui_button_tt* button = ui_buttons[i];

                if(button->position.x < x && x < button->position.x + button->size.x &&
                   button->position.y < y && y < button->position.y + button->size.y) {
                    if(button->callback) {
                        button->time_when_clicked = glfwGetTime();
                        button->callback();
                    }
                }
            }
        }
    }
}
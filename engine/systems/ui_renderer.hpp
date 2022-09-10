#pragma once

#include "../systems.hpp"


namespace spk {
    struct font_render_t {
        struct vertex_t {
            float x, y;
            glm::vec2 uv;
            glm::vec3 rgb;
        };
        
        sfk::vertex_layout_t layout;
        sfk::vertex_array_t vao;
        sfk::vertex_buffer_t vbo;
        sfk::program_t program;

        std::vector<vertex_t> buffer;

        /* letters */
        uint32_t indexes;
        uint32_t vertices;
        const uint32_t indexes_per_letter = 4;
        const uint32_t vertices_per_letter = 6;

        void init();
        void render(sfk::static_index_buffer_t& ibo, font_t* font, ui_canvas_t* canvas);
        void free();

        void add_ui_text(font_t* font, ui_text_t* text);
    };

    struct button_render_t {
        struct vertex_t {
            float x, y;
            float r, g, b;
        };

        sfk::vertex_layout_t layout;
        sfk::vertex_array_t vao;
        sfk::vertex_buffer_t vbo;
        sfk::program_t program;

        std::vector<vertex_t> buffer;

        /* letters */
        uint32_t indexes;
        uint32_t vertices;
        const uint32_t indexes_per_button = 4;
        const uint32_t vertices_per_button = 6;

        void init();
        void render(sfk::static_index_buffer_t& ibo, ui_canvas_t* canvas);
        void free();

        void add_ui_button(ui_button_t* button);
    };

    class ui_renderer_t : public render_system_t {
    public:

        void init(scene_t& scene);
        void render(scene_t& scene);
        void resize(int width, int height);
        void free(scene_t& scene);

    private:    

        // since font_render, and button render are both rendering quads they share the same IBO    
        sfk::static_index_buffer_t ibo;
        font_render_t font_render;
        button_render_t button_render;
        ui_canvas_t* canvas;
    };
}
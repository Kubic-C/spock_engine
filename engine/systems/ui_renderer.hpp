#pragma once

#include "../systems.hpp"

namespace spk {
    struct font_render_tt {
        struct vertex_tt {
            float x, y;
            float u, v;
            float r, g, b;
        };
        
        sfk::vertex_layout_tt layout;
        sfk::vertex_array_tt vao;
        sfk::vertex_buffer_tt vbo;
        sfk::program_tt program;

        std::vector<vertex_tt> buffer;

        /* letters */
        uint32_t indexes;
        uint32_t vertices;
        const uint32_t indexes_per_letter = 4;
        const uint32_t vertices_per_letter = 6;

        void init();
        void render(sfk::static_index_buffer_tt& ibo, font_tt* font, ui_canvas_tt* canvas);
        void free();

        void add_ui_text(vertex_tt* vtx, font_tt* font, ui_text_tt* text);
    };

    struct button_render_tt {
        struct vertex_tt {
            float x, y;
            float r, g, b;
        };

        sfk::vertex_layout_tt layout;
        sfk::vertex_array_tt vao;
        sfk::vertex_buffer_tt vbo;
        sfk::program_tt program;

        std::vector<vertex_tt> buffer;

        /* letters */
        uint32_t indexes;
        uint32_t vertices;
        const uint32_t indexes_per_button = 4;
        const uint32_t vertices_per_button = 6;

        void init();
        void render(sfk::static_index_buffer_tt& ibo, ui_canvas_tt* canvas);
        void free();

        void add_ui_button(vertex_tt* vtx, ui_button_tt* button);
    };

    class ui_renderer_tt : public render_system_tt {
    public:

        void init(scene_tt& scene);
        void render(scene_tt& scene);
        void resize(int width, int height);
        void free();

    private:    

        // since font_render, and button render are both rendering quads they share the same IBO    
        sfk::static_index_buffer_tt ibo;
        font_render_tt font_render;
        button_render_tt button_render;
        ui_canvas_tt* canvas;
    };
}
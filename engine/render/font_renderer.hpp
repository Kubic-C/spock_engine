#pragma once

#include "base_renderer.hpp"
#include "components/ui.hpp"

#define SPK_MAX_LETTERS 0xfff

namespace spk {
    struct font_vertex_t {
        float x, y;
        glm::vec2 uv;
        glm::vec3 rgb;
    };
    
    struct font_batch_mesh_t : public mesh_t {
        std::vector<font_vertex_t> buffer;

        font_t*   font;
        glm::mat4 vp;

        uint32_t       indexes;
        uint32_t       vertices;
        const uint32_t indexes_per_letter  = 4;
        const uint32_t vertices_per_letter = 6;

        void init();
        void free();       
        void subdata();
        void zero();
        void add_ui_text(ui_text_t* text);
    };

    class font_renderer_t : public base_renderer_t {  
    public:      
        font_renderer_t();
        ~font_renderer_t();
        
        void draw() override;
    
        font_batch_mesh_t mesh;

    private:
        render_ctx_t font_ctx;
    };
}
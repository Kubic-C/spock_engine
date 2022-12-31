#pragma once

#include "base_renderer.hpp"
#include "components/ui.hpp"

namespace spk {
    struct button_vertex_t {
        glm::vec2 pos;
        glm::vec3 color;
    };

    struct button_batch_mesh_t : public mesh_t {        
        std::vector<button_vertex_t> buffer;

        glm::mat4 vp;

        uint32_t indexes;
        uint32_t vertices;
        const uint32_t indexes_per_button = 4;
        const uint32_t vertices_per_button = 6;

        void init();
        void free();
        void subdata();
        void zero();
        void add_ui_button(ui_button_t* button);
    };

    class button_renderer_t : public base_renderer_t {
    public:
        void init() override;
        void draw() override;
        void free() override;

        button_batch_mesh_t mesh;
    };
}
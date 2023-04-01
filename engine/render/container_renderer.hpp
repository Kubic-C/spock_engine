#pragma once

#include "common.hpp"
#include "core/ui.hpp"

namespace spk {
    struct container_renderer_t : base_renderer_t {
        struct vertex_t {
            glm::vec3 pos;
            glm::vec3 tex;
            glm::vec4 color;
        };

        std::map<uint32_t, mesh_t<vertex_t, 6, 4>> meshes;

        void button_mesh(const button_t& button);
        void container_mesh(const container_t& container);
        void render() override;
    };
}
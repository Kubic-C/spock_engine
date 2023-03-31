#pragma once

#include "common.hpp"
#include "core/ui.hpp"

namespace spk {
    struct container_renderer_t : base_renderer_t {
        struct vertex_t {
            glm::vec3 pos;
            glm::vec3 tex;
            glm::vec3 color;
        };

        std::map<uint32_t, mesh_t<vertex_t, 6, 4>> meshes;

        void container_mesh(glm::vec3 pos, glm::vec2 size, const container_t& container);
        void render() override;
    };
}
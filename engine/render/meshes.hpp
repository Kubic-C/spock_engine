#pragma once

#include "opengl.hpp"
#include "utility/static_list.hpp"

namespace spk {
    struct mesh_t {
        vertex_buffer_t vertex_buffer;
        uint32_t        max_vertexes;

        void m_init();
        void resize(size_t v_size, size_t new_v, void* data = nullptr);
        void m_free();
    };
}
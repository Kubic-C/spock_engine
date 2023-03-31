/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "render/common.hpp"
#include "core/ui.hpp"

namespace spk {
    struct text_renderer_t : base_renderer_t {
    public:
        struct mesh_t {
            vertex_buffer_t buffer;
            size_t          vertexes_on_buffer = 0;
            size_t          vertexes_to_render = 0;

            mesh_t();
            ~mesh_t();
        };

        std::map<uint32_t, mesh_t> meshes; 

        void text_mesh(const text_t& text);
        void render() override;

    private: // utility functions
        void add_mesh(uint32_t font_id, void* vertices);
    };
}
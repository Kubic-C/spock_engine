/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

// common utilities for rendering

#include "opengl.hpp"
#include "utility/common.hpp"

namespace spk {
    enum shader_type_e {
        SHADER_TYPE_BASIC     = 0,
        SHADER_TYPE_SPRITE    = 1,
        SHADER_TYPE_TEXT      = 2,
        SHADER_TYPE_TEXTURE   = 3,
        SHADER_TYPE_BUTTON    = 4,
        SHADER_TYPE_CONTAINER = 5,
        SHADER_TYPE_TILEMAP   = 6,
        SHADER_TYPE_END       = 0xFF // 255
    };

    enum renderer_type_e {
        RENDERER_TYPE_BODY      = 0,
        RENDERER_TYPE_SPRITE    = 1,
        RENDERER_TYPE_CANVAS    = 2,
        RENDERER_TYPE_TEXT      = 3,
        RENDERER_TYPE_BUTTON    = 4,
        RENDERER_TYPE_CONTAINER = 5,
        RENDERER_TYPE_TILEMAP   = 6,
        RENDERER_TYPE_END       = 0xFF // 255
    };

    enum framebuffer_type_e {
        FRAMEBUFFER_TYPE_DEFAULT = 0,
        FRAMEBUFFER_TYPE_CANVAS = 1
    };

    struct base_renderer_t {
        virtual void pre_render() {}
        virtual void render() = 0;
        virtual void post_render() {}
    };

    // a renderer that meshes and renders by steppign through a tree
    struct tree_renderer_t : base_renderer_t {};

    // a renderer responsible for rendering components in the ECS system
    struct component_renderer_t : base_renderer_t {
        std::vector<flecs::system> mesh_systems;

        void pre_render() override {
            for(auto& mesh_system : mesh_systems) {
                mesh_system.run();
            }
        }

        flecs::system mesh_system_add(flecs::entity system) {
            // adding the tag_render_system, stops it from being called during ticks
            mesh_systems.push_back(flecs::system(system.world(), system.id()));
            system.add<tag_mesh_t>();

            return mesh_systems.back();
        }
    };

    template<typename vertex_type, size_t vertices_per_render_T, size_t vertices_per_subdata_T>
    struct mesh_t {
        vertex_buffer_t buffer;
        uint32_t        vertices_to_render = 0;
        uint32_t        vertices_on_buffer = 0;

        static constexpr size_t vertices_per_render  = vertices_per_render_T;
        static constexpr size_t vertices_per_subdata = vertices_per_subdata_T;
    
        mesh_t() {
            buffer.init(GL_ARRAY_BUFFER);
        }

        mesh_t(const mesh_t<vertex_type, vertices_per_render_T, vertices_per_subdata_T>& other) {
            buffer             = other.buffer;
            vertices_to_render = other.vertices_to_render;
            vertices_on_buffer = other.vertices_on_buffer;
        }

        ~mesh_t() {
            printf("freeing %lu\n", (uint32_t)buffer);
            buffer.free();
        }

        void add_mesh(ptm::ptr_t<vertex_type> mesh) {
            if(buffer.size() < (vertices_on_buffer + vertices_per_subdata) * sizeof(vertex_type)) 
                buffer.resize((vertices_on_buffer + vertices_per_subdata) * sizeof(vertex_type));

            buffer.buffer_sub_data(vertices_on_buffer * sizeof(vertex_type), sizeof(vertex_type) * vertices_per_subdata, mesh);
            vertices_on_buffer += vertices_per_subdata;
            vertices_to_render += vertices_per_render;
        }
    };
}

/* this is on purpose, some of them need access to the above listed elements ^^^ */
#include "components/common.hpp"
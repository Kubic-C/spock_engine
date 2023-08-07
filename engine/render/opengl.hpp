/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "base.hpp"
#include "texture_array.hpp"
#include "external/stb_image.h"

#define SFK_MAX_VERTEX_LAYOUT_ATTRIBUTES 8

namespace spk {
    uint32_t create_shader_from_src(uint32_t shader_type, const char* src, int* size);
    uint32_t create_shader(uint32_t shader_type, const char* file_path);

    struct vertex_attribute_t {
        size_t index;
        size_t size;
        size_t type;
        bool normalized;
        size_t stride;
        size_t offset;
   
        uint32_t buffer;
    };

    class vertex_layout_t {
    public:

        vertex_layout_t& add(size_t index, size_t size, size_t type, bool normalized, 
            size_t stride, size_t offset, uint32_t buffer = UINT32_MAX);
        void set_buffer(uint32_t index, uint32_t buffer);

        /* avoid using the heap on small data */
        vertex_attribute_t attributes[SFK_MAX_VERTEX_LAYOUT_ATTRIBUTES];
        std::bitset<SFK_MAX_VERTEX_LAYOUT_ATTRIBUTES> attributes_active;
    };

    class vertex_array_t {
    public:
        void init();
        void free();

        void bind();
        void bind_layout(vertex_layout_t& layout);

        operator uint32_t() {
            return id;
        }

    private:
        uint32_t id; // OpenGL ID
    };

    class vertex_buffer_t {
    public:
        void init(uint32_t type);
        void free();
    
        int size();
        void bind();
        void resize(size_t new_size);
        void buffer_data(size_t size, void* data, size_t usage);
        void buffer_sub_data(size_t offset, size_t size, const void* data);

        operator uint32_t() {
            return id;
        }

    private:
        uint32_t id; // OpenGL ID
        uint32_t type;
    };

    /* predefined never changing index buffer */
    class static_index_buffer_t : public vertex_buffer_t {
    public:
    
        // cap is the maximum amount of quads that the index buffer can 
        // generate for
        void generate_quad_indexes(uint32_t cap);
    };

    class texture2D_t {
    public:
        bool init();
        void free();
        void allocate(uint32_t type, uint32_t internal_format, uint32_t format, int width, int height, void* pixels);
        void subdata(uint32_t type, uint32_t xoffset, uint32_t yoffset, uint32_t format, int width, int height, void* pixels);
        bool load_image(const char* path, int desired_channels, bool flip, bool gen_mipmap = false);
        void active_texture(uint32_t slot);
        void bind();

        glm::ivec2 get_size() {
            int width, height;

            bind();
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

            return { width, height };
        }

        int get_channels() {
            return channels;
        }

        operator uint32_t() {
            return id;
        }

    private:
        int channels;
        uint32_t id;
    };

    class program_t {
    public:
        program_t();
        ~program_t();

        bool load_shader_files(const char* vsh, const char* fsh);
        bool load_shader_str(const char* vs, const char* fs);
        bool load_shader_modules(uint32_t vsh, uint32_t fsh, bool delete_shaders = true);
        void use();

        void set_bool(const std::string &name, bool value) const
        {         
            glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value); 
        }

        void set_int(const std::string &name, int value) const
        { 
            glUniform1i(glGetUniformLocation(id, name.c_str()), value); 
        }

        void set_float(const std::string &name, float value) const
        { 
            glUniform1f(glGetUniformLocation(id, name.c_str()), value); 
        }

        void set_vec2(const std::string &name, const glm::vec2 &value) const
        { 
            glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]); 
        }

        void set_vec2(const std::string &name, float x, float y) const
        { 
            glUniform2f(glGetUniformLocation(id, name.c_str()), x, y); 
        }

        void set_vec3(const std::string &name, const glm::vec3 &value) const
        { 
            glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]); 
        }

        void set_vec3(const std::string &name, float x, float y, float z) const
        { 
            glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z); 
        }

        void set_vec4(const std::string &name, const glm::vec4 &value) const
        { 
            glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]); 
        }

        void set_vec4(const std::string &name, float x, float y, float z, float w) const
        { 
            glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w); 
        }

        void set_mat2(const std::string &name, const glm::mat2 &mat) const
        {
            glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        void set_mat3(const std::string &name, const glm::mat3 &mat) const
        {
            glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        void set_mat4(const std::string &name, const glm::mat4 &mat) const
        {
            glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

    private:
        uint32_t id;
    };

    void tex_param_nearest(uint32_t target);
    void tex_param_linear(uint32_t target);
    void tex_param_wrap_repeat(uint32_t target);
    void tex_param_wrap_clamp_to_edge(uint32_t target);
}
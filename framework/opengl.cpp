#include "opengl.hpp"
#include "external/stb_image.h"

namespace sfk {
    uint32_t create_shader(uint32_t shader_type, const char* file_path) {
        int file_size;
        char* file_data;
        uint32_t shaderid;

        {
            FILE* file;

            file = fopen(file_path, "r");
            if(!file) {
                return -1;
            }

            fseek(file, 0, SEEK_END);
            file_size = ftell(file);
            fseek(file, 0, SEEK_SET);
            file_data = (char*)malloc(file_size * sizeof(char));
            fread(file_data, sizeof(char), file_size, file);
            fclose(file);

        }

        shaderid = glCreateShader(shader_type);
        glShaderSource(shaderid, 1, &file_data, &file_size);
        glCompileShader(shaderid);

        {
            int success;
            char info_log[512];
            glGetShaderiv(shaderid, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(shaderid, 512, NULL, info_log);
                printf("shader compilation failed: %s\n", info_log);
                glDeleteShader(shaderid);
                return -1;
            }
        }

        return shaderid;
    }

    vertex_layout_tt& vertex_layout_tt::add(size_t index, size_t size, size_t type, 
        bool normalized, size_t stride, size_t offset, uint32_t buffer) {
        assert(!attributes_active[index]);
        
        attributes_active[index].flip();

        {
            vertex_attribute_tt* pva = attributes + index;
            
            pva->index = index;
            pva->size = size;
            pva->type = type;
            pva->normalized = normalized;
            pva->stride = stride;
            pva->offset = offset;
            pva->buffer = buffer;

        }

        return *this;    
    }

    void vertex_layout_tt::set_buffer(uint32_t index, uint32_t buffer) {
        assert(attributes_active[index]);

        attributes[index].buffer = buffer;
    }

    void vertex_array_tt::init() {
        glGenVertexArrays(1, &id);
    }

    void vertex_array_tt::free() {
        glDeleteVertexArrays(1, &id);
    }
    
    void vertex_array_tt::bind() {
        glBindVertexArray(id);
    }

    void vertex_array_tt::bind_layout(vertex_layout_tt& layout) {
        bind();

        for(uint32_t i = 0; i < SFK_MAX_VERTEX_LAYOUT_ATTRIBUTES; i++) {
            if(layout.attributes_active[i]) {
                vertex_attribute_tt* pva = &layout.attributes[i];

                glBindBuffer(GL_ARRAY_BUFFER, pva->buffer);
                glVertexAttribPointer(pva->index, pva->size, pva->type, pva->normalized, pva->stride, (void*)pva->offset);
                glEnableVertexAttribArray(pva->index);
            }
        }
    }

    void vertex_buffer_tt::init(uint32_t type) {
        this->type = type;

        glGenBuffers(1, &id);
    }

    void vertex_buffer_tt::buffer_data(size_t size, void* data, size_t usage) {
        bind();
        glBufferData(type, size, data, usage);
    }

    void vertex_buffer_tt::buffer_sub_data(size_t offset, size_t size, void* data) {
        bind();
        glBufferSubData(type, offset, size, data);
    }

    void vertex_buffer_tt::free() {
        glDeleteBuffers(1, &id);
    }
    
    void vertex_buffer_tt::bind() {
        glBindBuffer(type, id);
    }

    void static_index_buffer_tt::generate_quad_indexes(uint32_t cap) {
        std::vector<uint32_t> indexes;
        indexes.resize(cap * 6);

        uint32_t offset = 0;
        for(uint32_t i = 0; i < cap * 6; i += 6) {
			indexes[i + 0] = 0 + offset;
			indexes[i + 1] = 1 + offset;
			indexes[i + 2] = 2 + offset;
			
			indexes[i + 3] = 0 + offset;
			indexes[i + 4] = 3 + offset;
			indexes[i + 5] = 2 + offset;   
            offset += 4;
        }

        buffer_data(indexes.size() * sizeof(uint32_t), indexes.data(), GL_STATIC_DRAW);
    }
}
#include "opengl.hpp"

namespace spk {
    uint32_t create_shader_from_src(uint32_t shader_type, const char* src, int* size) {
        uint32_t shaderid;

        shaderid = glCreateShader(shader_type);
        glShaderSource(shaderid, 1, &src, size);
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

    uint32_t create_shader(uint32_t shader_type, const char* file_path) {
        int file_size;
        char* file_data;

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

        return create_shader_from_src(shader_type, file_data, &file_size);;
    }

    vertex_layout_t& vertex_layout_t::add(size_t index, size_t size, size_t type, 
        bool normalized, size_t stride, size_t offset, uint32_t buffer) {
        spk_assert(!attributes_active[index]);
        
        attributes_active[index].flip();

        {
            vertex_attribute_t* pva = attributes + index;
            
            pva->index      = index;
            pva->size       = size;
            pva->type       = type;
            pva->normalized = normalized;
            pva->stride     = stride;
            pva->offset     = offset;
            pva->buffer     = buffer;

        }

        return *this;    
    }

    void vertex_layout_t::set_buffer(uint32_t index, uint32_t buffer) {
        spk_assert(attributes_active[index]);

        attributes[index].buffer = buffer;
    }

    void vertex_array_t::init() {
        glGenVertexArrays(1, &id);
    }

    void vertex_array_t::free() {
        glDeleteVertexArrays(1, &id);
    }
    
    void vertex_array_t::bind() {
        glBindVertexArray(id);
    }

    void vertex_array_t::bind_layout(vertex_layout_t& layout) {
        bind();

        for(uint32_t i = 0; i < SFK_MAX_VERTEX_LAYOUT_ATTRIBUTES; i++) {
            if(layout.attributes_active[i]) {
                vertex_attribute_t* pva = &layout.attributes[i];

                glBindBuffer(GL_ARRAY_BUFFER, pva->buffer);
                glVertexAttribPointer(pva->index, pva->size, pva->type, pva->normalized, pva->stride, (void*)pva->offset);
                glEnableVertexAttribArray(pva->index);
            }
        }
    }

    void vertex_buffer_t::init(uint32_t type) {
        this->type = type;

        glGenBuffers(1, &id);
    }

    int vertex_buffer_t::size() {
        int      size;

        // 1) copy data over to the copy buffer
        glGetNamedBufferParameteriv(id, GL_BUFFER_SIZE, &size);

        return size;
    }

    void vertex_buffer_t::resize(size_t new_size) {
        uint32_t copy_buffer;
        int      old_size;

        // 1) copy data over to the copy buffer
        glBindBuffer(GL_COPY_READ_BUFFER, id);
        glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &old_size);

        glCreateBuffers(1, &copy_buffer);

        glBindBuffer(GL_COPY_WRITE_BUFFER, copy_buffer);
        glBufferData(GL_COPY_WRITE_BUFFER, old_size, nullptr, GL_STATIC_COPY);

        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, old_size);

        // 1) resize the main buffer
        glBindBuffer(GL_COPY_WRITE_BUFFER, id);
        glBindBuffer(GL_COPY_READ_BUFFER, copy_buffer);
        glBufferData(GL_COPY_WRITE_BUFFER, new_size, nullptr, GL_DYNAMIC_DRAW);

        // 2) then write back to the main buffer and cleanup
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, old_size);
        glDeleteBuffers(1, &copy_buffer);
    }

    void vertex_buffer_t::buffer_data(size_t size, void* data, size_t usage) {
        bind();
        glBufferData(type, size, data, usage);
    }

    void vertex_buffer_t::buffer_sub_data(size_t offset, size_t size, const void* data){
        bind();
        glBufferSubData(type, offset, size, data);
    }

    void vertex_buffer_t::free() {
        glDeleteBuffers(1, &id);
    }
    
    void vertex_buffer_t::bind() {
        glBindBuffer(type, id);
    }

    void static_index_buffer_t::generate_quad_indexes(uint32_t cap) {
        std::vector<uint32_t> indexes;
        indexes.resize(cap * 6);

        uint32_t offset = 0;
        for(uint32_t i = 0; i < cap * 6; i += 6) {
			indexes[i + 0] = 0 + offset;
			indexes[i + 1] = 2 + offset;
			indexes[i + 2] = 1 + offset;
			
			indexes[i + 3] = 0 + offset;
			indexes[i + 4] = 3 + offset;
			indexes[i + 5] = 2 + offset;   
            offset += 4;
        }

        buffer_data(indexes.size() * sizeof(uint32_t), indexes.data(), GL_STATIC_DRAW);
    }

    bool texture2D_t::init() {
        glGenTextures(1, &id);
    
        return true;
    }

    void texture2D_t::allocate(uint32_t type, uint32_t internal_format, uint32_t format, int width, int height, void* pixels) {
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, pixels);
    }

    void texture2D_t::subdata(uint32_t type, uint32_t xoffset, uint32_t yoffset, uint32_t format, int width, int height, void* pixels) {
        bind();
        glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, width, height, format, type, pixels);
    }

    bool texture2D_t::load_image(const char* path, int desired_channels, bool flip, bool gen_mipmap) {
        uint8_t* pixels;
        int width, height;

        //TODO: desired_channels parameter does not work as expected
        spk_assert(desired_channels == 0);

        if(desired_channels == 0) {
            desired_channels = STBI_rgb_alpha;
        }

        stbi_set_flip_vertically_on_load(flip);
        pixels = stbi_load(path, &width, &height, &channels, desired_channels);
        if(!pixels)
            return false;

        bind();
        allocate(GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA, width, height, pixels); 
        
        if(gen_mipmap) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        
        stbi_image_free(pixels);

        return true;
    }

    void texture2D_t::active_texture(uint32_t slot) {
        glActiveTexture(slot);
        bind();
    }

    void texture2D_t::bind() {
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void texture2D_t::free() {
        glDeleteTextures(1, &id);
    }

    program_t::program_t() {
        id = glCreateProgram();
    }

    program_t::~program_t() {
        glDeleteProgram(id);
    }

    bool program_t::load_shader_files(const char* vsh_path, const char* fsh_path) {
        uint32_t vsh, fsh;
        bool vsh_invalid, fsh_invalid, leave;
    
        vsh = spk::create_shader(GL_VERTEX_SHADER, vsh_path);
        fsh = spk::create_shader(GL_FRAGMENT_SHADER, fsh_path);

        vsh_invalid = vsh == UINT32_MAX;
        fsh_invalid = fsh == UINT32_MAX;
        leave       = vsh_invalid || fsh_invalid;

        if(fsh_invalid) {
            glDeleteShader(vsh);
        }

        if(vsh_invalid) {
            glDeleteShader(fsh);
        }

        if(leave) {
            return false;
        }

        if(!load_shader_modules(vsh, fsh, true)) {
            return false;
        }

        return true;
    }

    bool program_t::load_shader_str(const char* vs, const char* fs) {
        uint32_t vs_shader = spk::create_shader_from_src(GL_VERTEX_SHADER, vs, nullptr);
        uint32_t fs_shader = spk::create_shader_from_src(GL_FRAGMENT_SHADER, fs, nullptr);
        
        if(vs_shader == UINT32_MAX ||
           fs_shader == UINT32_MAX)
            return false;

        return load_shader_modules(vs_shader, fs_shader);
    }

    bool program_t::load_shader_modules(uint32_t vsh, uint32_t fsh, bool delete_shaders) {    
        bool ret = true;
        
        glAttachShader(id, vsh);
        glAttachShader(id, fsh);
        glLinkProgram(id);
            
        {
            int success;
            char info_log[512];
            glGetProgramiv(id, GL_LINK_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(id, 512, NULL, info_log);
                log.log(spk::LOG_TYPE_INFO, "program failure to compile %s", info_log);
                ret = false;
            }
        }

        if(delete_shaders) {
            glDeleteShader(vsh);
            glDeleteShader(fsh);
        }

        return ret;
    }

    void program_t::use() {
        glUseProgram(id);    
    }

    void tex_param_nearest(uint32_t target) {
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    void tex_param_linear(uint32_t target) {
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void tex_param_wrap_repeat(uint32_t target) {
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void tex_param_wrap_clamp_to_edge(uint32_t target) {
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}
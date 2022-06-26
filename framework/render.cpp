#include "render.hpp"
#include "external/stb_image.h"

namespace sfk {
    bgfx::ShaderHandle load_shader(const std::string& file_name) {
        char* data = new char[2048];
        std::ifstream file;
        size_t fileSize;

        file.open(file_name);
        if(file.bad() || !file.is_open()) {
            logger.add_log(LOG_TYPE_WARNING, "could not load shader: ", file_name);
            return bgfx::ShaderHandle{UINT16_MAX};
        }

        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        file.read(data, fileSize);
        file.close();

        const bgfx::Memory* mem = bgfx::copy(data,fileSize+1);

        mem->data[mem->size-1] = '\0';
        bgfx::ShaderHandle handle = bgfx::createShader(mem);

        return handle;
    }

    bgfx::ProgramHandle load_program(const std::string& vs, const std::string& fs) {
        bgfx::ShaderHandle vs_handle, fs_handle;
        vs_handle = load_shader(vs);
        assert(BGFX_HANDLE_VALID(vs_handle));
        fs_handle = load_shader(fs);
        assert(BGFX_HANDLE_VALID(fs_handle));

        return bgfx::createProgram(vs_handle, fs_handle, true);
    }

    bool load_texture2D(const std::string& file_path, bgfx::TextureHandle& hdl) {
        int x, y, channels;
        void* data = stbi_load(file_path.c_str(), &x, &y, &channels, STBI_rgb_alpha);
        const bgfx::Memory* tex_mem;

        if(null) {
            logger.add_log(LOG_TYPE_WARNING, "could not load texture: ", file_path);
            return false;
        }

        tex_mem = bgfx::copy(data, x * y * channels);
        hdl = bgfx::createTexture2D(x, y, false, 1, bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_UVW_BORDER, tex_mem);
        stbi_image_free(data);

        return true;
    }
}
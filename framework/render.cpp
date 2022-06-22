#include "render.hpp"

namespace sfk {
    bgfx::ShaderHandle load_shader(debug_logger& logger, const std::string& file_name) {
        std::string file_path = "[NOT_LOADED]";
        std::string file_data = "";
        std::string line;
        bgfx::ShaderHandle shader_handle = {};
        const bgfx::Memory* mem; 

        switch(bgfx::getRendererType())  {
            case bgfx::RendererType::Noop:
            case bgfx::RendererType::Direct3D9:  file_path = "shaders/dx9/";   break;
            case bgfx::RendererType::Direct3D11:
            case bgfx::RendererType::Direct3D12: file_path = "shaders/dx11/";  break;
            case bgfx::RendererType::Gnm:        file_path = "shaders/pssl/";  break;
            case bgfx::RendererType::Metal:      file_path = "shaders/metal/"; break;
            case bgfx::RendererType::OpenGL:     file_path = "shaders/glsl/";  break;
            case bgfx::RendererType::OpenGLES:   file_path = "shaders/essl/";  break;
            case bgfx::RendererType::Vulkan:     file_path = "shaders/spirv/"; break;
        } 
        
        file_path.append(file_name);

        std::ifstream file(file_path, std::ios::binary);
        if(file.bad() || !file.is_open()) {
            logger.add_log(LOG_TYPE_WARNING, "could not load shader");
            return shader_handle;
        }

        while(std::getline(file, line)) {
            file_data += line + '\n';
        }
        file.close();

        mem = bgfx::copy(file_data.data(), file_data.size() + 1);
        shader_handle = bgfx::createShader(mem);

        return shader_handle;
    }

    bgfx::ShaderHandle load_custom_shader(debug_logger& logger, const std::string& file_name) {
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
}
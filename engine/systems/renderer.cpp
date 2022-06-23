#include "renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>

static spk::renderer2D_tt::vertex s_cubeVertices[] =
{
    {  0.5f,  0.5f, 0.0f, 0xff0000ff },
    {  0.5f, -0.5f, 0.0f, 0xff0000ff },
    { -0.5f, -0.5f, 0.0f, 0xff00ff00 },
    { -0.5f,  0.5f, 0.0f, 0xff00ff00 }
};

static const uint16_t s_cubeTriList[] =
{
    0,1,3,
    1,2,3
};

namespace spk {
    renderer2D_tt::renderer2D_tt() {}

    void renderer2D_tt::init(void* data) {
        init_data* init_info = (init_data*)data;
        bgfx::PlatformData pd;
        int width, height;

        this->window = init_info->window;
        window->set_platform_data(&pd);
        window->get_size(&width, &height);

        bgfx::renderFrame();

        bgfx::Init bgfx_init;
        bgfx_init.type = bgfx::RendererType::OpenGL;
        bgfx_init.resolution.width = width;   
        bgfx_init.resolution.height = height;
        bgfx_init.resolution.reset = BGFX_RESET_VSYNC;
        bgfx::init(bgfx_init);

        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, width, height);

        // weee woooo ///
        triangle.layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

        triangle_buffer = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), triangle.layout);
        index_buffer = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList)));

        bgfx::ShaderHandle vsh = sfk::load_custom_shader(*init_info->logger, "vs_cube.bin");
        assert(vsh.idx != bgfx::kInvalidHandle);
        bgfx::ShaderHandle fsh = sfk::load_custom_shader(*init_info->logger, "fs_cube.bin");
        assert(fsh.idx != bgfx::kInvalidHandle);
        triangle.program = bgfx::createProgram(vsh, fsh, true);
        assert(triangle.program.idx != bgfx::kInvalidHandle);
    }

    void renderer2D_tt::render() { 
        glm::mat4 model;

        set_window_state();

        model = glm::identity<glm::mat4>();
        model = glm::translate(model, glm::vec3(100.0f + cos(glfwGetTime()), 100.0f, 0.0f));
        model = glm::scale(model, glm::vec3(50.0f));
        model = glm::rotate(model, sinf((float)glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));
        bgfx::setTransform(&model[0][0]);

        bgfx::setState((0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z));
        bgfx::setVertexBuffer(0, triangle_buffer);
        bgfx::setIndexBuffer(index_buffer);

        bgfx::submit(0, triangle.program);
        bgfx::frame();
    }

    void renderer2D_tt::set_window_state() {
        glm::mat4 proj;
        glm::mat4 view;
        int width, height;

        window->get_size(&width, &height);        
        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
        proj = glm::ortho(0.0f, 500.0f, 500.0f, 0.0f);

        bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);

        bgfx::setViewRect(0, 0, 0, width, height);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    }

    void renderer2D_tt::update(flecs::world& world) {

        render();
    }
    
    void renderer2D_tt::free() {
        bgfx::shutdown();
    }
}
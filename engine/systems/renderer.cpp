#include "renderer.hpp"

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

        bgfx::Init bgfx_init;
        bgfx_init.type = bgfx::RendererType::OpenGL;
        bgfx_init.debug = true;
        bgfx_init.resolution.width = width;   
        bgfx_init.resolution.height = height;
        bgfx_init.resolution.reset = BGFX_RESET_VSYNC;
        bgfx::init(bgfx_init);

        bgfx::setDebug(BGFX_DEBUG_TEXT);

        bgfx::setViewClear(0, BGFX_CLEAR_COLOR, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, width, height);

        // weee woooo ///
        triangle.layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

        triangle_buffer = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), triangle.layout);
        index_buffer = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList)));
    
        bgfx::ShaderHandle vsh = sfk::load_custom_shader(*init_info->logger, "vs_cube.bin");
        bgfx::ShaderHandle fsh = sfk::load_custom_shader(*init_info->logger, "fs_cube.bin");
        triangle.program = bgfx::createProgram(vsh, fsh, true);
    }

    void renderer2D_tt::render() { 
        float mtx[16];
        bx::mtxRotateY(mtx, 0.0f);
        
        // position x,y,z
        mtx[12] = 0.0f;
        mtx[13] = 0.0f;
        mtx[14] = 0.0f;
        
        // Set model matrix for rendering.
        bgfx::setTransform(mtx);

        bgfx::setVertexBuffer(0, triangle_buffer);
        bgfx::setIndexBuffer(index_buffer);

        bgfx::setState(BGFX_STATE_DEFAULT);
        
        bgfx::submit(0, triangle.program);
        bgfx::frame();
    }

    void renderer2D_tt::update() {
        render();
    }
    
    void renderer2D_tt::free() {
        bgfx::shutdown();
    }
}
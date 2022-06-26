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

        tile_renderer.init(nullptr);
        renderer_manager.push_system(&tile_renderer);
    }

    void renderer2D_tt::set_window_state() {
        int width, height;

        window->get_size(&width, &height);        
        bgfx::setViewRect(0, 0, 0, width, height);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    }

    void renderer2D_tt::update(flecs::world& world) {
        const int view_id = 0;

        bgfx::frame(); // this allows copy, and move ops to happen before More commands are submmited via bgfx::submit

        for(auto sys : renderer_manager.systems) {
            set_window_state();
            sys->render(view_id, world);
        }
    }   
    
    void renderer2D_tt::free() {
        renderer_manager.free();
        bgfx::shutdown();
    }
}
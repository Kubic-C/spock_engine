#include "render.hpp"
#include "../state.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    void opengl_debug_callback(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam) {
        switch(type) {
        case GL_DEBUG_TYPE_ERROR:
            sfk::log.log(sfk::LOG_TYPE_ERROR, message);
            break;

        default:
            sfk::log.flags &= ~sfk::LOG_FLAGS_ENABLE_STD_PIPE;
            sfk::log.log(sfk::LOG_TYPE_INFO, message); 
            sfk::log.flags |= sfk::LOG_FLAGS_ENABLE_STD_PIPE;
        }
    }

    void render_system_on_add(flecs::entity e, render_system_ctx_t& ctx) {
#ifndef NDEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(opengl_debug_callback, nullptr);
#endif

        state._set_current_renderer(e);
    }

    void render_system_on_remove(flecs::entity e, render_system_ctx_t& ctx) {
    }

    void render_system_pre_update(flecs::entity e, comp_window_t& window, tag_current_window_t) {
        sfk_assert(e == state._get_current_window());

        glClear(GL_COLOR_BUFFER_BIT);

    }

    void render_system_resize(flecs::iter& iter) {
        auto ctx = SPK_GET_CTX(iter, render_system_ctx_t); 
        
        comp_window_size_t* resize = iter.param<comp_window_size_t>();
        float half_width  = (float)resize->width / 4;
        float half_height = (float)resize->height / 4;
        glm::mat4 view, proj;

        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        proj = glm::ortho(-(float)half_width / sfk::ppm, (float)half_width / sfk::ppm, -half_height / sfk::ppm, half_height / sfk::ppm);
        ctx->vp = proj * view;

        glViewport(0, 0, resize->width, resize->height);

        sfk::log.log(sfk::LOG_TYPE_INFO, "resize");
    }

    void render_system_post_update(flecs::entity e, comp_window_t& window, tag_current_window_t) {
        sfk_assert(e == state._get_current_window());

        SDL_GL_SwapWindow(window.win);
    }

    void render_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        flecs::entity* ctx;

        world.component<render_system_ctx_t>();
        world.observer<render_system_ctx_t>().event(flecs::OnAdd).each(render_system_on_add);
        world.observer<render_system_ctx_t>().event(flecs::OnRemove).each(render_system_on_remove);

        ctx = ctx_alloc.allocate_ctx<render_system_ctx_t>();

        world.system<comp_window_t, tag_current_window_t>().ctx(ctx) // PRE
            .kind(flecs::PreUpdate).each(render_system_pre_update);
        world.system<comp_window_t, tag_current_window_t>().ctx(ctx) // POST
            .kind(flecs::PostUpdate).each(render_system_post_update);
        world.observer().event<comp_window_size_t>().term<tag_events_t>()
            .ctx(ctx).iter(render_system_resize);
    }
}
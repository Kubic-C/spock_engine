/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "camera.hpp"
#include "core/internal.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk { 
    void comp_camera_t::init(flecs::entity entity) {
        z = 10.0f;
        pos  = { 0.0f, 0.0f };
        size = { 0.0f, 0.0f };
        scale = 1.0f; 
    }

    void comp_camera_t::free(flecs::entity entity) {

    }

    glm::vec2 comp_camera_t::get_world_position(const glm::vec2& screen_coords) {
        const float total_scale = internal->settings.ppm * scale;

        // screen camera size -> world camera size
        // SDL Window's positive Y is oppisote of Box2D's postive Y
        float world_y = size.y - screen_coords.y; // flip the y

        // divide by the PPM to get the screen coordinates world position
        // (everything is multiplied by PPM in rendering), and subtract by 
        // 1/2 of the world size to correctly offset the qoutient. 
        // Simply put we are doing the reverse of VP
        glm::vec2 world_coords = 
            { (screen_coords.x - (size.x / 2.0f)), 
              (        world_y - (size.y / 2.0f)) };
 
        world_coords /= total_scale;
        world_coords += pos; // position is already in world coordinates

        return world_coords;
    }

    void comp_camera_t::recalculate() {
        glm::mat4 view, proj;

        const float width  = size.x / 2.0f;
        const float height = size.y / 2.0f;

        view = glm::identity<glm::mat4>();
        view = glm::scale(view, glm::vec3(internal->settings.ppm * scale, internal->settings.ppm * scale, 1.0f));
        // flipping the coordinates must be done because Ortho flips everything
        view = glm::translate(view, glm::vec3(-pos, -z)); 

        proj = glm::ortho(-width, width, -height, height, 0.001f, 300.0f);

        render_context().world_camera = proj * view;
        render_context().ui_camera    = glm::ortho(0.0f, (float)size.x, (float)size.y, 0.0f, 0.00001f, 200.0f);
        render_context().ui_camera    = glm::translate(render_context().ui_camera, glm::vec3(0.0f, 0.0f, -100.0f));
    }

    void tag_current_camera_on_add(flecs::entity e, comp_camera_t& camera, tag_current_camera_t) {
        if(internal->scene.camera.id() != UINT64_MAX) {
            internal->scene.camera.remove<tag_current_camera_t>();
        }

        internal->scene.camera = e;
        
        // likely that the camera does not have its size set yet,
        // so emiting a resize event seems reasonable here
        // The size may change when changing between different cameras
        window_force_resize_event();
    } 

    void camera_comp_init(flecs::world& world) {
        spk_trace();
        
        spk_register_component(world, comp_camera_t);
    
        world.observer<comp_camera_t, tag_current_camera_t>()
            .event(flecs::OnAdd).each(tag_current_camera_on_add);
    }
}
#include "camera.hpp"
#include "state.hpp"
#include "spock.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk { 
    void comp_camera_t::init() {
        z = -1.0f;
        pos  = { 0.0f, 0.0f };
        size = { 0.0f, 0.0f };
        scale = 1.0f;
        view = glm::identity<glm::mat4>();
        proj = glm::identity<glm::mat4>();        
    }

    void comp_camera_t::free() {

    }

    glm::vec2 comp_camera_t::get_world_position(const glm::vec2& screen_coords) {
        // screen camera size -> world camera size
        // SDL Window's positive Y is oppisote of Box2D's postive Y
        float world_y = size.y - screen_coords.y; // flip the y

        // divide by the PPM to get the screen coordinates world position
        // (everything is multiplied by PPM in rendering), and subtract by 
        // 1/2 of the world size to correctly offset the qoutient. 
        // Simply put we are doing the reverse of VP
        glm::vec2 world_coords = 
            { (screen_coords.x - size.x / 2.0f - (pos.x * state.get_ppm() * scale)), 
              (        world_y - size.y / 2.0f - (pos.y * state.get_ppm() * scale))};
 
        return world_coords / (state.get_ppm() * scale);
    }

    void comp_camera_t::recalculate() {
        float half_width  = (float)size.x / 2;
        float half_height = (float)size.y / 2;
        
        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(pos * state.get_ppm() * scale, z));
        view = glm::scale(view, glm::vec3(state.get_ppm() * scale));
 
        proj = glm::ortho(-half_width, 
                           half_width, 
                          -half_height, 
                           half_height);
 
        vp = proj * view;
    }

    void tag_current_camera_on_add(flecs::entity e, comp_camera_t& camera, tag_current_camera_t) {
        if(state.get_current_camera() != UINT64_MAX) {
            state.get_current_camera().remove<tag_current_camera_t>();
        }

        state.set_current_camera(e);
        
        // likely that the camera does not have its size set yet,
        // so emiting a resize event seems reasonable here
        // The size may change when changing between different cameras
        state.get_current_window().get_ref<comp_window_t>()->force_resize_event();
    } 

    void camera_comp_init(flecs::world& world) {
        spk_register_component(world, comp_camera_t);
    
        world.observer<comp_camera_t, tag_current_camera_t>()
            .event(flecs::OnAdd).each(tag_current_camera_on_add);
    }
}
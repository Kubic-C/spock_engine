#include "camera.hpp"
#include "state.hpp"
#include "spock.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk { 
    void comp_camera_t::init() {
        z = 10.0f;
        pos  = { 0.0f, 0.0f };
        size = { 0.0f, 0.0f };
        scale = 1.0f;
        view = glm::identity<glm::mat4>();
        proj = glm::identity<glm::mat4>();        
    }

    void comp_camera_t::free() {

    }

    glm::vec2 comp_camera_t::get_world_position(const glm::vec2& screen_coords) {
        const float total_scale = state.get_ppm() * scale;

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
        const float width  = size.x / 2.0f;
        const float height = size.y / 2.0f;

        view = glm::identity<glm::mat4>();
        view = glm::scale(view, glm::vec3(state.get_ppm() * scale, state.get_ppm() * scale, 1.0f));
        // flipping the coordinates must be done because Ortho flips everything
        view = glm::translate(view, glm::vec3(-pos, -z)); 

        proj = glm::ortho(-width, width, -height, height, 0.001f, 300.0f);

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
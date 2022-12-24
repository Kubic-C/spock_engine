#include "camera.hpp"
#include "../state.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk { 
    void comp_camera_t::init() {
        z = -1.0f;
        pos  = { 0.0f, 0.0f };
        size = { 0.0f, 0.0f };
        view = glm::identity<glm::mat4>();
        proj = glm::identity<glm::mat4>();        
    }

    void comp_camera_t::free() {

    }

    void comp_camera_t::recalculate() {
        float half_width  = (float)size.x / 2;
        float half_height = (float)size.y / 2;
        
        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(pos, z));
        view = glm::scale(view, glm::vec3(state.get_ppm()));
 
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
    } 

    void camera_comp_init(flecs::world& world) {
        sfk_register_component(world, comp_camera_t);
    
        world.observer<comp_camera_t, tag_current_camera_t>()
            .event(flecs::OnAdd).each(tag_current_camera_on_add);
    }
}
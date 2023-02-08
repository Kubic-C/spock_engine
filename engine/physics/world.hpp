#pragma once

#include "body.hpp"
#include "quad_tree.hpp"
#include "utility/structures.hpp"
#include "systems.hpp"

namespace spk {
    struct physics_world_t : public system_t {
        physics_world_t() 
            : tree(glm::vec2(10.0f, 10.0f)), gravity(0.0f, 0.0f) {
        }

        rigid_body_t* add(glm::vec2 pos);
        fixture_t*    add_fixture(rigid_body_t* rigid_body, const material_t& material, glm::vec2 pos);
        void          add_box_fixture(rigid_body_t* rigid_body, const material_t& material, glm::vec2 pos,float hw, float hh);

        void step(float delta_time);

        void set_gravity(float x, float y) {
            gravity = {x, y};
        }

        world_tree_t tree;

    protected:
        glm::vec2 origin;
        glm::vec2 gravity;

        memory_pool_t<quad_tree_body_t> body_allocater;
        memory_pool_t<fixture_t>    fixture_allocater;
    };
}
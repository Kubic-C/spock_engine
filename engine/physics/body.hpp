#pragma once

#include "transform.hpp"
#include "material.hpp"
#include "aabb.hpp"
#include "utility/structures.hpp"

namespace spk {
    struct fixture_t;

    struct rigid_body_t {
        transform_t transform;
        glm::vec2   velocity;
        float       angular_velocity;
        aabb_pos_t      max;
        
        size_t     fixtures_size;
        fixture_t* fixtures;

        rigid_body_t() {}

        aabb_pos_t get_aabb_pos() const;
        void calculate_max();
    };

    struct comp_rigid_body_t {
        rigid_body_t* body;

        rigid_body_t* operator->() {
            return body;
        }

        rigid_body_t& operator*() {
            return *body;
        }

        operator rigid_body_t*() {
            return body;
        }

        void operator=(rigid_body_t* other) {
            body = other;
        }
    };

    enum {
        BOTTOM_LEFT = 0,
        BOTTOM_RIGHT = 1,
        TOP_RIGHT = 2,
        TOP_LEFT = 3
    };

    enum fixture_type_e {
        FIXTURE_TYPE_NONE = 0,
        FIXTURE_TYPE_BOX = 1
    };

    struct fixture_t {
        rigid_body_t* rb;
        fixture_t* next;
        fixture_t* prev;

        glm::vec2  pos;
        material_t material;
        uint32_t   type;
    
        glm::vec2 get_world_pos() {
            return rb->transform * pos;
        }

        union def_t {
            aabb_t box;
        
            def_t() {

            }
        } def;

        fixture_t() {
            rb       = nullptr;
            next     = nullptr;
            prev     = nullptr;
            pos      = {0.0f, 0.0f};
            material = {0.0f, 0.0f, 0.0f};
            type     = FIXTURE_TYPE_NONE;
        }
    };
}
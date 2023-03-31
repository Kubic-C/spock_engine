/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "ui.hpp"
#include "internal.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    constraint_t constraint_pixel(float pixels) {
        auto constraint = [=](float parent) -> float {
            return pixels;
        };

        return constraint;
    }

    constraint_t constraint_relative(float relative) {
        auto constraint = [=](float parent) -> float {
            return parent * relative;
        };

        return constraint;
        
    }

    constraint_t constraint_center() {
        auto constraint = [](float parent) -> float {
            return parent / 2.0f;
        };

        return constraint;
    }

    void canvas_make_current(canvas_t& canvas) {
        internal->scene.canvas = &canvas;        
    }

    void container_t::visible_set(bool value) { 
        flags[VISIBLE] = value; 
    }

    bool container_t::visible_get() const { 
        return flags[VISIBLE]; 
    };

    glm::vec3 container_t::canvas_position() {
        glm::vec3 current_pos = dimensions.pos;

        while(!parent.is_null()) {
            current_pos.x += parent->dimensions.pos.x;
            current_pos.y += parent->dimensions.pos.y;
        }

        return current_pos;
    }

    b2AABB container_t::aabb() {
        b2AABB bounds;
        glm::vec2 pos = (glm::vec2)canvas_position();

        bounds.lowerBound = pos - dimensions.size;
        bounds.upperBound = pos + dimensions.size;

        return bounds;
    }

    bool container_t::contains(glm::vec2 pos_) {
        b2AABB bounds = aabb();

        return bounds.Contains(pos_);
    }

    void container_t::x_set(constraint_t constraint) {
        constraints.xpos_constraint = constraint;
    }

    void container_t::y_set(constraint_t constraint) {
        constraints.ypos_constraint = constraint;
    }

    void container_t::z_set(constraint_t constraint) {
        constraints.zpos_constraint = constraint;
    }

    void container_t::width_set(constraint_t constraint) {
        constraints.width_constraint = constraint;
    }

    void container_t::height_set(constraint_t constraint) {
        constraints.height_constraint = constraint;
    }


    void container_t::dimensions_calculate() {
        if(parent.is_null()) { // only in the case if its a canvas
            glm::ivec2 size = window().size();

            dimensions.size = size;
            dimensions.pos  = {0.0f, 0.0f, 0.0f};
            return;
        } else {
            dimensions_t& parent_dimensions = parent->dimensions_get();

            dimensions.pos = { 
                constraints.xpos_constraint(parent_dimensions.size.x), 
                constraints.ypos_constraint(parent_dimensions.size.y), 
                constraints.zpos_constraint(parent_dimensions.pos.z)
            };

            dimensions.size = {
                constraints.width_constraint(parent_dimensions.size.x), 
                constraints.height_constraint(parent_dimensions.size.y)             
            };
        }
    }

    canvas_t::canvas_t() {
        parent = nullptr;
        canvas = this;
    }
}
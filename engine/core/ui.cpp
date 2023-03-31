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
        glm::vec3 current_pos = pos;

        while(!parent.is_null()) {
            current_pos.x += parent->pos.x;
            current_pos.y += parent->pos.y;
        }

        return current_pos;
    }

    b2AABB container_t::aabb() {
        b2AABB bounds;
        glm::vec2 pos = (glm::vec2)canvas_position();

        bounds.lowerBound = pos - size;
        bounds.upperBound = pos + size;

        return bounds;
    }

    bool container_t::contains(glm::vec2 pos_) {
        b2AABB bounds = aabb();

        return bounds.Contains(pos_);
    }

    canvas_t::canvas_t() {
        parent = nullptr;
        canvas = this;
    }
}
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

    glm::vec2 container_t::bottom_left() const {
        if(parent.is_null())
            return {0.0f, 0.0f};
        
        return (glm::vec2)dimensions.pos - dimensions.size;
    }

    glm::vec2 container_t::top_left() const {
        glm::vec2 top_left_pos = (glm::vec2)dimensions.pos;

        if(parent.is_null())
            return {0.0f, dimensions.size.y};

        top_left_pos.x -= dimensions.size.x;
        top_left_pos.y += dimensions.size.y;

        return top_left_pos;
    }

    glm::vec2 container_t::top_right() const {
        if(parent.is_null())
            return {0.0f, dimensions.size.y};

        return (glm::vec2)dimensions.pos + dimensions.size;
    }

    glm::vec3 container_t::canvas_position() const {
        if(parent.is_null())
            return {0.0f, 0.0f, 0.0f};

        glm::vec3 current_pos = {0.0f, 0.0f, 0.0f};
        ptr_t     cur_parent  = this->parent;

        while(!cur_parent.is_null()) {
            current_pos.x += cur_parent->dimensions.pos.x;
            current_pos.y += cur_parent->dimensions.pos.y;
        
            cur_parent = cur_parent->parent;
        }

        return current_pos;
    }

    b2AABB container_t::aabb() const {
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
            const dimensions_t& parent_dimensions = parent->dimensions_get();
            glm::vec2           bottom_left_pos   = parent->bottom_left();

            dimensions.pos = { 
                constraints.xpos_constraint(parent_dimensions.size.x) + bottom_left_pos.x, 
                constraints.ypos_constraint(parent_dimensions.size.y) + bottom_left_pos.y, 
                parent_dimensions.pos.z + 0.1f
            };

            dimensions.size = {
                constraints.width_constraint(parent_dimensions.size.x), 
                constraints.height_constraint(parent_dimensions.size.y)             
            };

            if(type() == ui_types_t::TEXT) 
                ((text_t*)this)->text_dimensions_calculate();
        }
    }

    void text_t::text_dimensions_calculate() {
        font_t&     font            = font_get();
        glm::vec2   bottom_left_pos = bottom_left();
        glm::vec2   top_left_pos    = top_left();
        glm::vec2   top_right_pos   = top_right();
        text_info_t text_info;
        float       scale = 1.0f;

    start:
        text_info.calculate(text, font, scale);

        if(alignment == text_align_t::LEFT) {
            text_dimensions.cursor    = top_left_pos;
            text_dimensions.cursor.y -= text_info.height;

            text_dimensions.xreset  = top_left_pos.x;
            text_dimensions.ysub    = text_info.height * 1.1;
            text_dimensions.xright  = top_right_pos.x;
            text_dimensions.ybottom = bottom_left_pos.y;
            text_dimensions.scale   = scale;

            // make a lower scale so it can fit
            if(text_dimensions.cursor.y < text_dimensions.ybottom) {
                float amount_over = text_dimensions.ybottom - text_dimensions.cursor.y;
                float height_fit  = text_info.height - amount_over; 

                scale = height_fit / text_info.height;
                goto start;
            }
        }
    }

    font_t& text_t::font_get() const {
        if(font == UINT32_MAX) {
            return fonts().get(canvas->font);
        }

        return fonts().get(font);
    }

    canvas_t::canvas_t() {
        parent = nullptr;
        canvas = this;
    }
}
/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "text.hpp"
#include "utility/ui.hpp"
#include "utility/stack_allocator.hpp"

/**
 * @brief A UI with tree repersentation
 * 
 */

namespace spk {
    // idea: use constraints with lambdas

    class canvas_t;

    struct dimensions_t {
        glm::vec3 pos  = {0.0f, 0.0f, 0.0f};
        glm::vec2 size = {0.0f, 0.0f};
    };

    struct text_dimensions_t {
        glm::vec2 cursor  = {0.0f, 0.0f};
        float     xreset  = 0.0f;
        float     ysub    = 0.0f;

        float     xright  = 0.0f;
        float     ybottom = 0.0f;
        float     scale   = 1.0f;
    };

    enum class ui_types_t {
        CANVAS,
        CONTAINER,
        TEXT,
        BUTTON
    };

    enum class text_align_t {
        LEFT,
        CENTER,
        RIGHT
    };

    typedef std::function<float(float parent)> constraint_t;

    constraint_t constraint_pixel(float pixels);
    constraint_t constraint_relative(float relative);
    constraint_t constraint_center();

    struct constraints_t {
        constraint_t xpos_constraint   = constraint_pixel(0.0f);
        constraint_t ypos_constraint   = constraint_pixel(0.0f);

        constraint_t width_constraint  = constraint_pixel(0.0);
        constraint_t height_constraint = constraint_pixel(0.0);
    };

    class container_t {
    public:
        container_t() {}

        virtual ui_types_t type() { return ui_types_t::CONTAINER; }

        void visible_set(bool value);
        bool visible_get() const;
        
        // get the bottom left position in screen coordinates
        glm::vec2 bottom_left() const;

        // get the top let position in screen coordinates
        glm::vec2 top_left() const;

        // get the top right position in screen coodrinates
        glm::vec2 top_right() const;

        // the position of the element within the canvas
        glm::vec3 canvas_position() const;

        // aabb of the container in screen coordinates
        b2AABB aabb() const;

        // checks if the position is inside of the container
        bool contains(glm::vec2 pos);

        // adds a new child
        template<typename T, bool is_base = std::is_base_of_v<container_t, T>>
        ptr_t<T> element();

        void dimensions_calculate();
        const dimensions_t& dimensions_get() const { return dimensions; }

        // constraints

        void x_set(constraint_t constraint);
        void y_set(constraint_t constraint);
        void width_set(constraint_t constraint);
        void height_set(constraint_t constraint);

    public:
        uint32_t sprite_array_id = UINT32_MAX;
        int32_t  sprite_index    = -1;

        glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

        std::list<ptr_t<container_t>> children;
        ptr_t<container_t>            parent;
        ptr_t<canvas_t>               canvas;

    protected:
        enum {
            VISIBLE = 0,  
            BASE_FLAGS_END = 4,
        };

        dimensions_t   dimensions;
        constraints_t  constraints;
        std::bitset<8> flags;
    };

    class text_t : public container_t {
    public:
        virtual ui_types_t type() override { return ui_types_t::TEXT; }

        void text_alignment_set(text_align_t align) { alignment = align; }
        text_align_t text_alignment_get() const { return alignment; }

        void text_dimensions_calculate();
        const text_dimensions_t& text_dimensions_get() const { return text_dimensions; }

        font_t& font_get() const;

        glm::vec3   text_color = {0.0f, 0.0f, 0.0f};
        uint32_t    font = UINT32_MAX;
        std::string text = "";

    private:
        text_dimensions_t text_dimensions;
        text_align_t      alignment = text_align_t::LEFT;
    };

    class button_t : public container_t {
    public:
        virtual ui_types_t type() override { return ui_types_t::BUTTON; }

        glm::vec3 click_color      = {0.0f, 0.0f, 0.f};

    private:
        bool  is_clicked;
        float time_since_click = 0.0f;
    };

    class canvas_t : public container_t {
        friend class container_t;

    public:
        virtual ui_types_t type() override { return ui_types_t::CANVAS; }

        canvas_t();

        // font to use for texts, set to your desired font ID
        uint32_t font = UINT32_MAX;

    private:
        object_pool_t<std::variant<container_t, text_t, button_t>> element_pool;
    };

    /**
     * @brief sets the passed canvas as the current canvas
     * 
     * @param canvas the UI canvas that you want to make active
     */
    void canvas_make_current(canvas_t& canvas);
}

namespace spk {
    template<typename T, bool is_base>
    ptr_t<T> container_t::element() {
        spk_assert(is_base == true);

        auto     variant = canvas->element_pool.create(1);
        ptr_t<T> element = &variant->emplace<T>();

        element->parent = ptr_t(this);
        element->canvas = canvas;

        children.push_back(ptr_t<container_t>(element));

        return element;
    }
}
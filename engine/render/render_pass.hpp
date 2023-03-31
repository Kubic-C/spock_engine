#pragma once

#include "common.hpp"

namespace spk {
    typedef std::function<void(uint32_t src, uint32_t dst)> framebuffer_op_t;

    // renders SRC's color attachment onto DST's color attachment
    void framebuffer_op_overlay(uint32_t src, uint32_t dst);

    struct attachment_t {
        attachment_t(uint32_t internal_format, uint32_t format, uint32_t data_type, uint32_t type);
        ~attachment_t();

        void resize(uint32_t width, uint32_t height);

        uint32_t texture         = UINT32_MAX;
        uint32_t atch_type       = UINT32_MAX;
        uint32_t internal_format = UINT32_MAX;
        uint32_t format          = UINT32_MAX;
        uint32_t data_type       = UINT32_MAX;
    };

    struct framebuffer_t {
        framebuffer_t(bool is_default = false);
        ~framebuffer_t();

        void attach(const attachment_t& attachment);
        bool check_status();

        operator uint32_t() {
            return framebuffer;
        }

        bool     is_default;
        uint32_t framebuffer = 0;
    };

    struct render_pass_t {
        // must be a framebuffer ID
        uint32_t dst_framebuffer = UINT32_MAX; // may be UINT32_MAX to not use
        uint32_t framebuffer     = 0;

        uint32_t  clear_mask    = 0;
        glm::vec4 clear_color   = {0.0f, 0.0f, 0.0f, 1.0f};
        double    clear_depth   = 1.0f;
        bool      clear_stencil = false;

        // a list renderers that will be called during the render pass
        std::vector<renderer_type_e> renderers;

        // called at the end of the render pass
        framebuffer_op_t framebuffer_op = nullptr;
    };

    struct render_info_t {
        std::vector<render_pass_t> render_passes;
    };
}
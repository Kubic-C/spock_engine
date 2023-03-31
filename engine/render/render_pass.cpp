#include "render_pass.hpp"
#include "core/internal.hpp"

namespace spk {
    attachment_t::attachment_t(uint32_t internal_format, uint32_t format, uint32_t data_type, uint32_t atch_type)
        : internal_format(internal_format), format(format), data_type(data_type), atch_type(atch_type) {
        glm::ivec2 size = window().size();
    
        glGenTextures(1, &texture);
        resize(size.x, size.y);
    }

    attachment_t::~attachment_t() {
        if(texture != UINT32_MAX)
            glDeleteTextures(1, &texture);
    }

    void attachment_t::resize(uint32_t width, uint32_t height) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, data_type, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    framebuffer_t::framebuffer_t(bool is_default) 
        : is_default(is_default) {
            
        if(!is_default)
            glCreateFramebuffers(1, &framebuffer);
    }

    framebuffer_t::~framebuffer_t() {
        if(!is_default)
            glDeleteFramebuffers(1, &framebuffer);
    }

    void framebuffer_t::attach(const attachment_t& attachment) {
        spk_assert(!is_default);

        glBindTexture(GL_TEXTURE_2D, attachment.texture);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment.atch_type, GL_TEXTURE_2D, attachment.texture, 0);  
    }

    bool framebuffer_t::check_status() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }

    void framebuffer_op_overlay(uint32_t src, uint32_t dst) {
        auto& shader = render_context().shaders[SHADER_TYPE_TEXTURE];
        GLboolean enable_depth_test = false;
        int color_attachment = INT_MAX;

        glGetBooleanv(GL_DEPTH_TEST, &enable_depth_test);
        glDisable(GL_DEPTH_TEST);

        glGetNamedFramebufferAttachmentParameteriv(src, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &color_attachment);

        if(color_attachment == INT_MAX)
            return;

        glBindFramebuffer(GL_FRAMEBUFFER, dst);

        shader.use();
        shader.set_mat4("u_vp", render_context().framebuffer_ops.one_to_one_vp);
        shader.set_int("tex", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, color_attachment);
        render_context().framebuffer_ops.overlay.array.bind();
        render_context().common.quad_indexes.bind();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glDisable(GL_BLEND);

        if(enable_depth_test)
            glEnable(GL_DEPTH_TEST);
    } 
}
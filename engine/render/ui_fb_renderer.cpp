#include "ui_fb_renderer.hpp"
#include "render_system.hpp"

const char* vs_fb_shader = R"###(
#version 330 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

out vec2 v_uv;

void main() {
    gl_Position = vec4(a_pos, 0.0f, 1.0f);
    v_uv = a_uv;
})###";

const char* fs_fb_shader = R"###(
#version 330 core

in vec2 v_uv;
uniform sampler2D color;

void main() {
    gl_FragColor = texture(color, v_uv);
})###";

float pos[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, // triangle 1 
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f,
     1.0f,  1.0f, 1.0f, 1.0f, // triangle 2
    -1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f
};

namespace spk {
    void ui_framebuffer_renderer_t::init() {
        vertex_array.init();

        vertex_buffer.init(GL_ARRAY_BUFFER);
        vertex_buffer.buffer_data(sizeof(pos), pos, GL_STATIC_DRAW);

        vertex_layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0, vertex_buffer);
        vertex_layout.add(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, sizeof(float) * 2, vertex_buffer);
        vertex_array.bind_layout(vertex_layout);

        program.init();
        bool ret = program.load_shader_str(vs_fb_shader, fs_fb_shader);
        spk_assert(ret);
    }
    
    void ui_framebuffer_renderer_t::free() {
        program.free();
        vertex_buffer.free();
        vertex_array.free();
    }

    void ui_framebuffer_renderer_t::draw(render_system_t* rs, key_t current) {
        framebuffer_t& fb = rs->fb_get(current);
        attachment_t&  color = rs->atch_get(fb.attachments[0]);

        spk_assert(color.attachment == GL_COLOR_ATTACHMENT0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        vertex_array.bind();

        program.use();
        glBindTexture(GL_TEXTURE_2D, color.texture);
        glActiveTexture(GL_TEXTURE0);
        program.set_int("color", 0);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }
}
#include "button_renderer.hpp"
#include "state.hpp"
#include "render_system.hpp"

const char* vs_button = R"###(
#version 330 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec3 a_color;
out vec3 v_color;
uniform mat4 u_vp;
void main() {
    gl_Position = u_vp * vec4(a_pos, 0.1f, 1.0);
    v_color = a_color;
}
)###";

const char* fs_button = R"###(
#version 330 core
in vec3 v_color;
out vec4 color;
void main()
{    
    color = vec4(v_color, 1.0f);
}
)###"; 

namespace spk {
    void button_batch_mesh_t::init() {
        m_init();

        indexes = 0;
        vertices = 0;

        buffer.resize(vertices_per_button * 0xfff);
        vertex_buffer.buffer_data(sizeof(button_vertex_t) * vertices_per_button * 0xfff, nullptr, GL_DYNAMIC_DRAW);
    }

    void button_batch_mesh_t::zero() {
        indexes  = 0;
        vertices = 0;
    }

    void button_batch_mesh_t::free() {
        m_free();
    }

    void button_batch_mesh_t::subdata() {
        vertex_buffer.buffer_sub_data(0, indexes * sizeof(button_vertex_t), buffer.data());
    }

    void button_batch_mesh_t::add_ui_button(ui_button_t* btn) {
        button_vertex_t* vtx = buffer.data() + indexes;
        const float x = btn->abs_pos.x;
        const float y = btn->abs_pos.y;
        const float hw = btn->abs_size.x;
        const float hh = btn->abs_size.y;
        glm::vec3 offset = {0.0f, 0.0f, 0.0f};
        glm::vec3 color;

        if(btn->time_when_clicked + 0.1f > spk::time.get_time()) {
            offset = {0.5f, 0.5f, 0.5f};
        }

        color = btn->color - offset;
 
        vtx[0] = { {x - hw,  y - hh}, color};
        vtx[2] = { {x + hw,  y + hh}, color};
        vtx[1] = { {x + hw,  y - hh}, color};
        vtx[3] = { {x - hw,  y + hh}, color};

        indexes += indexes_per_button;
        vertices += vertices_per_button;
    }    
    
    void button_renderer_t::init() {
        b_init();
        mesh.init();

        vertex_layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 3), 0, 0);
        vertex_layout.add(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 3), sizeof(float) * 2, 0);
        
        bool ret = program.load_shader_str(vs_button, fs_button);
        spk_assert(ret);
    }

    void button_renderer_t::free() {
        mesh.free();
        b_free();
    }

    void button_renderer_t::draw() {
        if(!mesh.vertices)
            return;

        mesh.subdata();

        vertex_layout.set_buffer(0, mesh.vertex_buffer);
        vertex_layout.set_buffer(1, mesh.vertex_buffer);
        vertex_array.bind_layout(vertex_layout);

        vertex_array.bind();
        state.get_current_renderer()->quad_index_buffer.bind();
        program.use();
        program.set_mat4("u_vp", mesh.vp);

        glDrawElements(GL_TRIANGLES, mesh.vertices, GL_UNSIGNED_INT, nullptr);
    }

}
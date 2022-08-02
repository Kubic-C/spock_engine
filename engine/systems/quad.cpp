#include "quad.hpp"
#include <glm/gtc/matrix_transform.hpp>
 
uint32_t index_a[] = {
   0, 1, 2,
   1, 2, 3
};
 
namespace spk {
    void quad_renderer_tt::init(sfk::window_tt& window, flecs::world& world, void* pscene) {

        { // vertex data
            vertex_array.init();
            vertex_array.bind();
    
            vertex_buffer.init(GL_ARRAY_BUFFER);
            vertex_buffer.buffer_data(sizeof(float) * 2 * 6, nullptr, GL_DYNAMIC_DRAW);
            vertex_layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0, vertex_buffer);
            vertex_array.bind_layout(vertex_layout);
        }
    
        { // shaders
            uint32_t vsh, fsh;
    
            vsh = sfk::create_shader(GL_VERTEX_SHADER, "./shaders/vs_tiles.glsl");
            fsh = sfk::create_shader(GL_FRAGMENT_SHADER, "./shaders/fs_tiles.glsl");
    
            if(fsh == UINT32_MAX || vsh == UINT32_MAX) {
                abort();
            }
    
            program = glCreateProgram();
            glAttachShader(program, vsh);
            glAttachShader(program, fsh);
            glLinkProgram(program);
            glDeleteShader(vsh);
            glDeleteShader(fsh);
    
            {
                int success;
                char info_log[512];
                glGetProgramiv(program, GL_LINK_STATUS, &success);
                if(!success) {
                    glGetShaderInfoLog(program, 512, NULL, info_log);
                    sfk::logger.add_log(sfk::LOG_TYPE_ERROR, "program linking failed: %s\n", info_log);
                    abort();
                }
            }
        }
    
        {
            int width, height;
    
            window.get_size(&width, &height);
            resize(width, height);
        }
    }
    
    void quad_renderer_tt::render(scene_tt& scene) {
        flecs::world& world = scene.world;
        auto q = world.query<transform_tt, quad_render_tt>();
    
        q.iter([&](flecs::iter& it, transform_tt* c_transforms, quad_render_tt* c_quads) {
            for(auto i : it) {
                transform_tt& transform = c_transforms[i];
             
                float pos[] = {
                   -(transform.scale.x * 1.0f) + transform.position.x, -(transform.scale.y * 1.0f) + transform.position.y, // bl
                    (transform.scale.x * 1.0f) + transform.position.x, -(transform.scale.y * 1.0f) + transform.position.y, // br
                   -(transform.scale.x * 1.0f) + transform.position.x,  (transform.scale.y * 1.0f) + transform.position.y, // tl
                    
                    (transform.scale.x * 1.0f) + transform.position.x,  (transform.scale.y * 1.0f) + transform.position.y, // tr
                    (transform.scale.x * 1.0f) + transform.position.x, -(transform.scale.y * 1.0f) + transform.position.y, // tr
                   -(transform.scale.x * 1.0f) + transform.position.x,  (transform.scale.y * 1.0f) + transform.position.y, // tr
                };           

                vertex_buffer.buffer_sub_data(0, sizeof(pos), pos);
           
                vertex_array.bind();

                glUseProgram(program);
                glUniformMatrix4fv(glGetUniformLocation(program, "u_vp"), 1, false, &vp[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        });
    }
 
    void quad_renderer_tt::resize(int width, int height) {
        float half_width  = (float)width / 2;
        float half_height = (float)height / 2;
    
        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        proj = glm::ortho(0.0f, (float)width, -half_height, half_height);
        vp   = proj * view;
    }
 
    void quad_renderer_tt::free() {
        vertex_array.free();
        vertex_buffer.free();
        glDeleteProgram(program);
    }
}

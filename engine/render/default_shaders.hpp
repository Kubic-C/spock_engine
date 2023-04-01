/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

namespace spk {
    inline const char* basic_vs = R"###(
    #version 330 core
    layout(location = 0) in vec2 a_pos;

    out vec4 v_color;

    uniform mat4 u_vp;
    uniform vec4 color;

    void main() {
        gl_Position = u_vp * vec4(a_pos, 0.0f, 1.0);
        v_color = color;
    })###";

    inline const char* basic_fs = R"###(
    #version 330 core
    in vec4 v_color;

    out vec4 fragment_color;

    void main() {
        fragment_color = vec4(v_color.rgb, v_color.a);
    })###";

    const char* sprite_vs = R"###(
    #version 330 core
    layout(location = 0) in vec3 a_pos;
    layout(location = 1) in vec3 a_tex_coords;

    out vec3 v_tex_coords;

    uniform mat4 u_vp;

    void main() {
        gl_Position = u_vp * vec4(a_pos, 1.0);
        v_tex_coords = a_tex_coords;
    })###";

    const char* sprite_fs = R"###(
    #version 330 core
    in vec3 v_tex_coords;

    uniform sampler2DArray array;

    out vec4 fragment_color;

    void main() {
        fragment_color = texture(array, v_tex_coords);
    })###";

    const char* texture_vs = R"###(
    #version 330 core
    layout(location = 0) in vec3 a_pos;
    layout(location = 1) in vec2 a_tex_coords;

    out vec2 v_tex_coords;

    uniform mat4 u_vp;

    void main() {
        gl_Position = u_vp * vec4(a_pos, 1.0);
        v_tex_coords = a_tex_coords;
    })###";

    const char* texture_fs = R"###(
    #version 330 core
    in vec2 v_tex_coords;

    uniform sampler2D tex;

    out vec4 fragment_color;

    void main() {
        fragment_color = texture(tex, v_tex_coords);
    })###";

    const char* text_vs = R"###(
    #version 330 core
    layout(location = 0) in vec3 a_pos;
    layout(location = 1) in vec3 a_tex_coords;
    layout(location = 2) in vec3 a_color;

    out vec3 v_tex_coords;
    out vec3 v_color;

    uniform mat4 u_vp;

    void main() {
        gl_Position = u_vp * vec4(a_pos, 1.0);
        v_tex_coords = a_tex_coords;
        v_color = a_color;
    }
    )###";

    const char* text_fs = R"###(
    #version 330 core
    in vec3 v_tex_coords;
    in vec3 v_color;

    uniform sampler2DArray font;

    out vec4 color;

    void main() {    
        float blend = texture(font, v_tex_coords).r;

        // multiplying it will give a more clear look
        color = vec4(v_color, blend);
    })###";

    const char* container_vs = R"###(
    #version 330 core
    layout(location = 0) in vec3 a_pos;
    layout(location = 1) in vec3 a_tex_coords;
    layout(location = 2) in vec3 a_color;

    out vec3 v_tex_coords;
    out vec3 v_color;

    uniform mat4 u_vp;

    void main() {
        gl_Position = u_vp * vec4(a_pos, 1.0);
        v_tex_coords = a_tex_coords;
        v_color = a_color;
    }
    )###";

    const char* container_fs = R"###(
    #version 330 core
    in vec3 v_tex_coords;
    in vec3 v_color;

    uniform sampler2DArray array;

    out vec4 color;

    void main() {    
        if(v_tex_coords.z == -1.0f)
            color = vec4(v_color, 1.0f);
        else {
            color = texture(array, v_tex_coords) * vec4(v_color, 1.0f);
        }
    })###";
}

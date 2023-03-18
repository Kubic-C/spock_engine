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
}

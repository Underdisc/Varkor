#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec2 a_tex_coord;

out vec3 color;
out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    mat4 transform = proj * view * model;
    gl_Position = transform * vec4(a_pos, 1.0);
    color = a_color;
    tex_coord = a_tex_coord;
}

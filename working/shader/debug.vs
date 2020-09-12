#version 330 core
layout (location = 0) in vec3 a_pos;

uniform mat4 view;
uniform mat4 proj;

void main()
{
    mat4 transform = proj * view;
    gl_Position = transform * vec4(a_pos, 1.0);
}

#version 330 core
in vec3 color;
in vec2 tex_coord;

out vec4 final_color;

uniform sampler2D tex;

void main()
{
    final_color = texture(tex, tex_coord);
}

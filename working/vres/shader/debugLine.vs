layout (location = 0) in vec3 aPos;

uniform mat4 uView;
uniform mat4 uProj;

void main()
{
    mat4 transform = uProj * uView;
    gl_Position = transform * vec4(aPos, 1.0);
}

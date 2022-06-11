layout(location = 0) in vec3 aPos;

uniform mat4 uProjView;
uniform mat4 uModel;

void main()
{
  gl_Position = uProjView * uModel * vec4(aPos, 1.0);
}
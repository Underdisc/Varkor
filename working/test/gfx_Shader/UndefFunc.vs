#type vertex

layout(location = 0) in vec3 aPos;

void UndefFunc();

void main()
{
  UndefFunc();
  gl_Position = vec4(aPos, 1.0);
}
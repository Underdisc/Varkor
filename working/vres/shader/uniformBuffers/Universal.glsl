layout(std140) uniform Universal
{
  mat4 uView; // 0
  mat4 uProj; // 64
  vec3 uViewPos; // 128
  float uTotalTime; // 140
}; // 148
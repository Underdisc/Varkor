#type fragment

in vec3 iTextureCoords;
out vec4 iFinalColor;

uniform samplerCube uCubemap;

void main()
{
  iFinalColor = texture(uCubemap, iTextureCoords);
}
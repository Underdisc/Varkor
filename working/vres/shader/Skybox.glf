in vec3 iTextureCoords;
out vec4 iFinalColor;

uniform samplerCube uSkyboxSampler;

void main()
{
  iFinalColor = texture(uSkyboxSampler, iTextureCoords);
}
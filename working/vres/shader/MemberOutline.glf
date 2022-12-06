#type fragment

out vec4 iFinalColor;

in vec2 iUv;

uniform isampler2D uTexture;
uniform int uWidth;
uniform vec4 uColor;

void main()
{
  // Ignore all fragments that are valid memberIds.
  const int invalidMemberId = -1;
  iFinalColor = vec4(0.0, 0.0, 0.0, 0.0);
  int memberId = texture(uTexture, iUv).r;
  if (memberId != invalidMemberId) {
    return;
  }

  // Color all fragments that are within some range of valid memberIds.
  vec2 texelSize = 1.0 / textureSize(uTexture, 0);
  for (int i = -uWidth; i <= uWidth; ++i) {
    for (int j = -uWidth; j <= uWidth; ++j) {
      vec2 uv = iUv;
      uv.x = uv.x + i * texelSize.x;
      uv.y = uv.y + j * texelSize.y;
      memberId = texture(uTexture, uv).r;
      if (memberId != invalidMemberId) {
        iFinalColor = uColor;
        return;
      }
    }
  }
}
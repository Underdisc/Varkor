#type vertex

#include "uniformBuffers/Universal.glsl"

layout(location = 0) in vec2 aPos;

out vec2 iTextureCoords;
out vec4 iShadowClipPos;

uniform mat4 uModel;
uniform mat4 uShadowProjView;

void main()
{
  vec4 worldPos = uModel * vec4(aPos, 0.0, 1.0);
  gl_Position = uProj * uView * worldPos;
  iShadowClipPos = uShadowProjView * worldPos;
  iTextureCoords.x = aPos.x + 0.5;
  iTextureCoords.y = aPos.y + 0.5;
}

////////////////////////////////////////////////////////////////////////////////
#type fragment

out vec4 iFinalColor;

in vec2 iTextureCoords;
in vec4 iShadowClipPos;

uniform sampler2D uTexture;
uniform float uShadowBias;
uniform sampler2D uShadowTexture;

void main()
{
  // Get the position of the fragment in the shadow map's ndc space.
  vec3 ndcShadowPos = iShadowClipPos.xyz / iShadowClipPos.w;
  ndcShadowPos = ndcShadowPos * 0.5 + 0.5;

  // Apply a bias to the fragment depth so surfaces receiving and casting
  // shadows don't experience shadow acne.
  float currentDepth = ndcShadowPos.z - uShadowBias;

  vec2 shadowTexelSize = 1.0 / textureSize(uShadowTexture, 0);
  float shadow = 0.0;
  if (currentDepth > 1.0) {
    // No shadow is applied for fragments outside of the shadow map's frustum.
    shadow = 0.0;
  }
  else {
    // Find a [0, 1] value representing the shadow based on the average of
    // surrounding texels in the shadow map (pcf).
    for (int x = -1; x <= 1; ++x) {
      for (int y = -1; y <= 1; ++y) {
        vec2 offset = vec2(x, y) * shadowTexelSize;
        vec2 shadowTexel = ndcShadowPos.xy + offset;
        float closestDepth = texture(uShadowTexture, shadowTexel).r;
        if (currentDepth > closestDepth) {
          shadow = shadow + 1.0;
        }
      }
    }
    shadow = shadow / 9.0;
  }

  vec4 alphaColor = texture(uTexture, iTextureCoords);
  vec3 color = alphaColor.xyz;
  color = color - color * shadow * 0.75;
  iFinalColor = vec4(color, alphaColor.a);
}

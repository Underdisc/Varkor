#type fragment

in vec2 iUv;
out vec4 iFinalColor;

uniform sampler2D uMain;
uniform sampler2D uOther;

void main()
{
  vec3 mainColor = texture(uMain, iUv).rgb;
  vec3 otherColor = texture(uOther, iUv).rgb;
  iFinalColor = vec4(mainColor + otherColor, 1.0);
}
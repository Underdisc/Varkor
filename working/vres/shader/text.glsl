const float onEdgeValue = 120.0 / 255.0;
const float pixelDistance = 40.0 / 255.0;
const float aliasWidth = 10.0 / 255.0;

// The contribution of a value below the edgeValue will result in 0. Above will
// result in 1 but there will be a 0 to 1 gradient within the alias width.
float Contribution(float value, float edgeValue)
{
  float dist = value - edgeValue;
  return clamp(dist, 0.0, aliasWidth) / aliasWidth;
}

float InverseContribution(float value, float edgeValue)
{
  return 1.0 - Contribution(value, edgeValue);
}

float ContributionDiscard(float value, float edgeValue)
{
  float contribution = Contribution(value, edgeValue);
  if (contribution == 0.0)
  {
    discard;
  }
  return contribution;
}

vec4 BasicText(float value, vec3 color)
{
  float contribution = ContributionDiscard(value, onEdgeValue);
  return vec4(color, contribution);
}

vec4 StrokeText(float value, float width, vec3 strokeColor, vec3 fillColor)
{
  width = width / 255.0;
  float stokeContrib = InverseContribution(value, onEdgeValue);
  float fillContrib = Contribution(value, onEdgeValue);
  vec3 color = stokeContrib * strokeColor + fillContrib * fillColor;
  float strokeOuterEdgeValue = onEdgeValue - width;
  float contribution = ContributionDiscard(value, strokeOuterEdgeValue);
  return vec4(color, contribution);
}

vec3 SmoothColor(float seed)
{
  const float tao = 6.28;
  float red = clamp(sin(seed) + 0.5, 0.0, 1.0);
  float green = clamp(sin(seed - tao / 3.0) + 0.5, 0.0, 1.0);
  float blue = clamp(sin(seed - 2.0 * tao / 3.0) + 0.5, 0.0, 1.0);
  return vec3(red, green, blue);
}

vec4 RainbowFlowText(float value, float time)
{
  vec3 color = SmoothColor(time + value * 15.0f);
  float contribution = ContributionDiscard(value, onEdgeValue);
  return vec4(color, contribution);
}

namespace Math {

template<typename T>
T Clamp(T minimum, T maximum, T value)
{
  if (value < minimum)
  {
    return minimum;
  }
  if (value > maximum)
  {
    return maximum;
  }
  return value;
}

} // namespace Math

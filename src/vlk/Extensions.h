#ifndef vlk_Extensions_h
#define vlk_Extensions_h

#include "gfx/HdrColor.h"
#include "math/Vector.h"
#include "rsl/ResourceId.h"
#include "vlk/Valkor.h"

namespace Vlk {

template<>
struct Converter<std::string>
{
  static void Serialize(Value& val, const std::string& value)
  {
    val.ExpectType(Value::Type::TrueValue);
    val.mTrueValue = value;
  }

  static bool Deserialize(const Value& val, std::string* value)
  {
    if (val.mType != Value::Type::TrueValue) {
      return false;
    }
    *value = val.mTrueValue;
    return true;
  }
};

template<typename T, unsigned int N>
struct Converter<Math::Vector<T, N>>
{
  static void Serialize(Value& val, const Math::Vector<T, N>& value)
  {
    val[{N}];
    for (int i = 0; i < N; ++i) {
      val[i] = value[i];
    }
  }

  static bool Deserialize(const Explorer& ex, Math::Vector<T, N>* value)
  {
    for (int i = 0; i < N; ++i) {
      Explorer element = ex[i];
      if (!element.Valid()) {
        return false;
      }
      (*value)[i] = element.As<T>();
    }
    return true;
  }
};

template<>
struct Converter<Gfx::HdrColor>
{
  static void Serialize(Value& val, const Gfx::HdrColor& value)
  {
    val("Color") = value.mColor;
    val("Intensity") = value.mIntensity;
  }

  static bool Deserialize(const Explorer& ex, Gfx::HdrColor* value)
  {
    Explorer colorEx = ex("Color");
    Explorer intensityEx = ex("Intensity");
    if (!colorEx.Valid() || !intensityEx.Valid()) {
      return false;
    }
    value->mColor = colorEx.As<Vec3>();
    value->mIntensity = intensityEx.As<float>();
    return true;
  }
};

template<>
struct Converter<Rsl::ResId>
{
  static void Serialize(Value& val, const ResId& value)
  {
    val.ExpectType(Value::Type::TrueValue);
    val.mTrueValue = value.mId;
  }

  static bool Deserialize(const Value& val, ResId* value)
  {
    if (val.mType != Value::Type::TrueValue) {
      return false;
    }
    value->mId = val.mTrueValue;
    return true;
  }
};

} // namespace Vlk

#endif

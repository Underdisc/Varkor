#ifndef vlk_Extensions_h
#define vlk_Extensions_h

#include <string>

#include "gfx/HdrColor.h"
#include "math/Quaternion.h"
#include "math/Vector.h"
#include "rsl/ResourceId.h"
#include "vlk/Valkor.h"

namespace Vlk {

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

  static bool Deserialize(const Value& val, Math::Vector<T, N>* value)
  {
    for (int i = 0; i < N; ++i) {
      const Value* element = val.TryGetConstValue(i);
      if (element == nullptr) {
        return false;
      }
      if (!Converter<T>::Deserialize(*element, value->mD + i)) {
        return false;
      }
    }
    return true;
  }
};

template<>
struct Converter<Quat>
{
  static void Serialize(Value& val, const Quat& quat)
  {
    val[{4}];
    for (int i = 0; i < 4; ++i) {
      val[i] = quat[i];
    }
  }

  static bool Deserialize(const Value& val, Quat* quat)
  {
    for (int i = 0; i < 4; ++i) {
      const Value* element = val.TryGetConstValue(i);
      if (element == nullptr) {
        return false;
      }
      if (!Converter<float>::Deserialize(*element, quat->mVec.mD + i)) {
        return false;
      }
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

  static bool Deserialize(const Value& val, Gfx::HdrColor* value)
  {
    const Value* colorVal = val.TryGetConstPair("Color");
    const Value* intensityVal = val.TryGetConstPair("Intensity");
    if (colorVal == nullptr || intensityVal == nullptr) {
      return false;
    }
    return Converter<Vec3>::Deserialize(*colorVal, &value->mColor) &&
      Converter<float>::Deserialize(*intensityVal, &value->mIntensity);
  }
};

template<>
struct Converter<Rsl::ResId>
{
  static void Serialize(Value& val, const ResId& value)
  {
    val.EnsureType(Value::Type::TrueValue);
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

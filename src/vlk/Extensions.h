#include "math/Vector.h"
#include "vlk/Valkor.h"

namespace Vlk {

template<>
void Value::operator=(const Vec3& value);
template<>
Vec3 Explorer::AsInternal<Vec3>(const Vec3& defaultValue) const;

} // namespace Vlk

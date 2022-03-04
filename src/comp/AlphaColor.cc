#include "comp/AlphaColor.h"

namespace Comp {

void AlphaColor::VInit(const World::Object& owner)
{
  mAlphaColor = {0.0f, 1.0f, 0.0f, 1.0f};
}

} // namespace Comp

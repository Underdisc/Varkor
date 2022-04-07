#ifndef gfx_LineModel_h
#define gfx_LineModel_h

#include "AssetLibrary.h"
#include "ds/Vector.h"
#include "math/Vector.h"

namespace Gfx {

enum class TerminalType
{
  Flat,
  Point,
  CollapsedNormal,
  CollapsedBinormal,
  Arrow,
};

void InitLineModel(
  AssetId modelId,
  const Ds::Vector<Vec3> points,
  float fill,
  float thickness,
  TerminalType startTerminal,
  TerminalType endTerminal);

} // namespace Gfx

#endif
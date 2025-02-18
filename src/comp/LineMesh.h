#ifndef gfx_LineMesh_h
#define gfx_LineMesh_h

#include "ds/Vector.h"
#include "math/Vector.h"
#include "rsl/ResourceId.h"

enum class TerminalType {
  Flat,
  Point,
  CollapsedNormal,
  CollapsedBinormal,
  Arrow,
};

void InitLineMesh(
  ResId meshId,
  const Ds::Vector<Vec3> points,
  float fill,
  float thickness,
  TerminalType startTerminal,
  TerminalType endTerminal);

#endif

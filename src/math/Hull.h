#ifndef math_Mesh_h
#define math_Mesh_h

#include "Result.h"
#include "ds/List.h"
#include "ds/Vector.h"
#include "math/Vector.h"
#include "math/Plane.h"

namespace Math {

struct Hull {
  struct HalfEdge;
  struct Face;
  struct Vertex {
    Vec3 mPosition;
    Ds::List<HalfEdge>::Iter mHalfEdge;
  };
  struct HalfEdge {
    Ds::List<Vertex>::Iter mVertex;
    Ds::List<HalfEdge>::Iter mTwin;
    Ds::List<HalfEdge>::Iter mNext;
    Ds::List<Face>::Iter mFace;
    Ds::List<HalfEdge>::Iter Prev() const;
  };
  struct Face {
    Ds::List<HalfEdge>::Iter mHalfEdge;
    Math::Plane Plane() const;
  };
  Ds::List<Vertex> mVertices;
  Ds::List<HalfEdge> mHalfEdges;
  Ds::List<Face> mFaces;

  static VResult<Hull> QuickHull(const Ds::Vector<Vec3>& points);
};

} // namespace Math

#endif

#include <functional>

#include "math/Constants.h"
#include "math/Hull.h"
#include "math/Ray.h"

#include "Input.h"
#include "debug/Draw.h"

namespace Math {

Ds::List<Hull::HalfEdge>::Iter Hull::HalfEdge::Prev() const {
  Ds::List<Hull::HalfEdge>::Iter currentEdge = mNext;
  while (currentEdge->mNext->mNext != mNext) {
    currentEdge = currentEdge->mNext;
  }
  return currentEdge;
}

Math::Plane Hull::Face::Plane() const {
  return Math::Plane::Points(
    mHalfEdge->mNext->mNext->mVertex->mPosition,
    mHalfEdge->mNext->mVertex->mPosition,
    mHalfEdge->mVertex->mPosition);
}

VResult<Hull> Hull::QuickHull(const Ds::Vector<Vec3>& points) {
  // The extreme points are organised like so: x, y, z, -x, -y, -z.
  const Vec3* extremePoints[6] = {&points[0]};
  for (int i = 0; i < 6; ++i) {
    extremePoints[i] = &points[0];
  }
  for (int p = 1; p < points.Size(); ++p) {
    const Vec3& point = points[p];
    for (int i = 0; i < 3; ++i) {
      if (point[i] > (*extremePoints[i])[i]) {
        extremePoints[i] = &point;
      }
      if (point[i] < (*extremePoints[i + 3])[i]) {
        extremePoints[i + 3] = &point;
      }
    }
  }

  // Cases where extreme points collapse or we don't get a polyhedron need to be
  // handled before we construct a polyhedron. We choose the first four extreme
  // points we find to create a polyhedron.
  Hull hull;
  hull.mVertices.PushBack({*extremePoints[0], hull.mHalfEdges.end()});
  Ds::Vector<Ds::List<Vertex>::Iter> verts;
  verts.Push(hull.mVertices.Back());
  for (size_t i = 1; i < 6; ++i) {
    const Vec3& newPoint = *extremePoints[i];
    if (verts.Size() == 1) {
      if (!Math::Near(newPoint, verts[0]->mPosition)) {
        hull.mVertices.PushBack({newPoint, hull.mHalfEdges.end()});
        verts.Push(hull.mVertices.Back());
      }
    }
    else if (verts.Size() == 2) {
      Math::Ray edge =
        Math::Ray::Points(verts[0]->mPosition, verts[1]->mPosition);
      if (!Math::Near(edge.DistanceSq(newPoint), 0.0f)) {
        hull.mVertices.PushBack({newPoint, hull.mHalfEdges.end()});
        verts.Push(hull.mVertices.Back());
      }
    }
    else if (verts.Size() == 3) {
      Math::Plane plane = Math::Plane::Points(
        verts[0]->mPosition, verts[1]->mPosition, verts[2]->mPosition);
      float pointDist = plane.Distance(newPoint);
      if (!Math::Near(pointDist, 0.0f)) {
        hull.mVertices.PushBack({newPoint, hull.mHalfEdges.end()});
        verts.Push(hull.mVertices.Back());
        if (pointDist < 0.0f) {
          verts.Swap(1, 2);
        }
      }
    }
  }
  if (verts.Size() < 4) {
    return Result("The points do not form a hull.");
  }

  // Create the initial half edge structure representing the polyhedron.
  Ds::List<Hull::HalfEdge>& edgeList = hull.mHalfEdges;
  Ds::List<Hull::Face>::Iter faces[4] = {
    hull.mFaces.PushBack({edgeList.end()}),
    hull.mFaces.PushBack({edgeList.end()}),
    hull.mFaces.PushBack({edgeList.end()}),
    hull.mFaces.PushBack({edgeList.end()}),
  };
  Ds::List<Hull::HalfEdge>::Iter edges[12] = {
    edgeList.PushBack({verts[0], edgeList.end(), edgeList.end(), faces[0]}),
    edgeList.PushBack({verts[1], edgeList.end(), edgeList.end(), faces[0]}),
    edgeList.PushBack({verts[2], edgeList.end(), edgeList.end(), faces[0]}),
    edgeList.PushBack({verts[1], edgeList.end(), edgeList.end(), faces[1]}),
    edgeList.PushBack({verts[0], edgeList.end(), edgeList.end(), faces[1]}),
    edgeList.PushBack({verts[3], edgeList.end(), edgeList.end(), faces[1]}),
    edgeList.PushBack({verts[2], edgeList.end(), edgeList.end(), faces[2]}),
    edgeList.PushBack({verts[1], edgeList.end(), edgeList.end(), faces[2]}),
    edgeList.PushBack({verts[3], edgeList.end(), edgeList.end(), faces[2]}),
    edgeList.PushBack({verts[0], edgeList.end(), edgeList.end(), faces[3]}),
    edgeList.PushBack({verts[2], edgeList.end(), edgeList.end(), faces[3]}),
    edgeList.PushBack({verts[3], edgeList.end(), edgeList.end(), faces[3]})};

  edges[0]->mTwin = edges[3];
  edges[1]->mTwin = edges[6];
  edges[2]->mTwin = edges[9];
  edges[3]->mTwin = edges[0];
  edges[4]->mTwin = edges[11];
  edges[5]->mTwin = edges[7];
  edges[6]->mTwin = edges[1];
  edges[7]->mTwin = edges[5];
  edges[8]->mTwin = edges[10];
  edges[9]->mTwin = edges[2];
  edges[10]->mTwin = edges[8];
  edges[11]->mTwin = edges[4];

  edges[0]->mNext = edges[1];
  edges[1]->mNext = edges[2];
  edges[2]->mNext = edges[0];
  edges[3]->mNext = edges[4];
  edges[4]->mNext = edges[5];
  edges[5]->mNext = edges[3];
  edges[6]->mNext = edges[7];
  edges[7]->mNext = edges[8];
  edges[8]->mNext = edges[6];
  edges[9]->mNext = edges[10];
  edges[10]->mNext = edges[11];
  edges[11]->mNext = edges[9];

  faces[0]->mHalfEdge = edges[0];
  faces[1]->mHalfEdge = edges[3];
  faces[2]->mHalfEdge = edges[6];
  faces[3]->mHalfEdge = edges[9];

  verts[0]->mHalfEdge = edges[0];
  verts[1]->mHalfEdge = edges[3];
  verts[2]->mHalfEdge = edges[6];
  verts[3]->mHalfEdge = edges[11];

  // Create a conflict list for each face. Each conflict list stores a vector of
  // points that do not lie in the hull.
  struct ConflictList {
    Ds::List<Face>::Iter mFace;
    Math::Plane mPlane;
    struct Point {
      Vec3 mPosition;
      float mDistance;
    };
    Ds::Vector<Point> mPoints;
  };
  Ds::Vector<ConflictList> conflictLists;
  Ds::List<Face>::Iter currentFace = hull.mFaces.begin();
  for (; currentFace != hull.mFaces.end(); ++currentFace) {
    ConflictList newList;
    newList.mFace = currentFace;
    newList.mPlane = currentFace->Plane();
    conflictLists.Push(newList);
  }

  // Find the plane each point is closest to and give the point to that plane's
  // conflict list. Conflict lists that didn't receive a point are removed.
  auto assignConflictPoint =
    [](const Vec3& point, Ds::Vector<ConflictList>& lists)
  {
    float minDist = FLT_MAX;
    ConflictList* bestConflictList = nullptr;
    for (ConflictList& list: lists) {
      float dist = list.mPlane.Distance(point);
      if (dist <= nEpsilon) {
        continue;
      }
      if (dist < minDist) {
        minDist = dist;
        bestConflictList = &list;
      }
    }
    if (bestConflictList != nullptr) {
      bestConflictList->mPoints.Push({point, minDist});
    }
  };
  for (const Vec3& point: points) {
    assignConflictPoint(point, conflictLists);
  }
  for (int c = 0; c < conflictLists.Size(); ++c) {
    if (conflictLists[c].mPoints.Size() == 0) {
      conflictLists.LazyRemove(c--);
    }
  }

  // Add points to the hull until there are no remaining conflict lists.
  while (conflictLists.Size() > 0) {
    // For all points in the conflict lists, find the point with maximum
    // distsance from its respective plane..
    float maxDist = -FLT_MAX;
    int bestConflictListIdx = -1;
    int bestConflictPointIdx = -1;
    for (int l = 0; l < conflictLists.Size(); ++l) {
      const ConflictList& conflictList = conflictLists[l];
      for (int p = 0; p < conflictList.mPoints.Size(); ++p) {
        const ConflictList::Point conflictPoint = conflictList.mPoints[p];
        if (conflictPoint.mDistance > maxDist) {
          maxDist = conflictPoint.mDistance;
          bestConflictListIdx = l;
          bestConflictPointIdx = p;
        }
      }
    }

    // Treating the best point as an eye looking towards the current hull, find
    // the edges that form the horizon around the hull.
    const ConflictList& conflictList = conflictLists[bestConflictListIdx];
    const Vec3& newPoint = conflictList.mPoints[bestConflictPointIdx].mPosition;
    Ds::Vector<Ds::List<HalfEdge>::Iter> horizon;
    Ds::Vector<Ds::List<Face>::Iter> visitedFaces;
    std::function<void(Ds::List<HalfEdge>::CIter)> visitEdge =
      [&](Ds::List<HalfEdge>::CIter edge)
    {
      if (visitedFaces.Contains(edge->mFace)) {
        return;
      }
      visitedFaces.Push(edge->mFace);
      Ds::List<HalfEdge>::CIter currentEdge = edge;
      do {
        Ds::List<HalfEdge>::Iter twin = currentEdge->mTwin;
        Math::Plane twinPlane = twin->mFace->Plane();
        if (twinPlane.HalfSpaceContains(newPoint)) {
          horizon.Push(twin);
        }
        else {
          visitEdge(twin);
        }
      } while ((currentEdge = currentEdge->mNext) != edge);
    };
    visitEdge(conflictList.mFace->mHalfEdge);

    // Add the new vertex to the half edge geometry and create the new conflict
    // lists representing those faces.
    Ds::Vector<ConflictList> newConflictLists;
    Ds::List<HalfEdge>::Iter end = edgeList.end();
    Ds::List<Vertex>::Iter newVertex = hull.mVertices.PushBack({newPoint, end});
    for (int i = 0; i < horizon.Size(); ++i) {
      Ds::List<HalfEdge>::Iter hEdge = horizon[i];
      Ds::List<Face>::Iter newFace = hull.mFaces.PushBack({end});
      Ds::List<HalfEdge>::Iter newEdges[3] = {
        edgeList.PushBack({hEdge->mTwin->mVertex, end, end, newFace}),
        edgeList.PushBack({hEdge->mVertex, end, end, newFace}),
        edgeList.PushBack({newVertex, end, end, newFace})};
      newFace->mHalfEdge = newEdges[0];
      hEdge->mTwin = newEdges[0];
      newEdges[0]->mTwin = hEdge;
      for (int e = 0; e < 3; ++e) {
        newEdges[e]->mNext = newEdges[(e + 1) % 3];
        newEdges[e]->mVertex->mHalfEdge = newEdges[e];
      }
      ConflictList newConflictList;
      newConflictList.mFace = newFace;
      newConflictList.mPlane = newFace->Plane();
      newConflictLists.Push(newConflictList);
    }
    for (int i = 0; i < horizon.Size(); ++i) {
      Ds::List<HalfEdge>::CIter hEdge = horizon[i];
      Ds::List<HalfEdge>::CIter hNextEdge = horizon[(i + 1) % horizon.Size()];
      hEdge->mTwin->mNext->mTwin = hNextEdge->mTwin->Prev();
      hNextEdge->mTwin->Prev()->mTwin = hEdge->mTwin->mNext;
      newVertex->mHalfEdge = hEdge->mTwin->Prev();
    }

    // Delete dead vertices, edges, faces, and conflict lists.
    Ds::Vector<Ds::List<Vertex>::Iter> deadVerts;
    for (const Ds::List<Face>::Iter& faceIt: visitedFaces) {
      Ds::List<HalfEdge>::Iter currentEdge = faceIt->mHalfEdge;
      do {
        // Determine whether the edge's vertex should be erased.
        bool horizonVertex = false;
        for (const Ds::List<HalfEdge>::Iter& hEdge: horizon) {
          if (hEdge->mVertex == currentEdge->mVertex) {
            horizonVertex = true;
            break;
          }
        }
        if (!horizonVertex && !deadVerts.Contains(currentEdge->mVertex)) {
          deadVerts.Push(currentEdge->mVertex);
        }
        // Erase the edge.
        Ds::List<HalfEdge>::Iter nextEdge = currentEdge->mNext;
        hull.mHalfEdges.Erase(currentEdge);
        currentEdge = nextEdge;
      } while (currentEdge != faceIt->mHalfEdge);

      // Check if the face has an asociated conflict list and delete the face.
      int oldConflictListIdx = -1;
      for (int c = 0; c < conflictLists.Size(); ++c) {
        if (conflictLists[c].mFace == faceIt) {
          oldConflictListIdx = c;
          break;
        }
      }
      hull.mFaces.Erase(faceIt);
      if (oldConflictListIdx == -1) {
        continue;
      }

      // Migrate each point in the old conflict list to one of the new conflict
      // lists and remove the old conflict list. The plane the point lies
      // closest to determines its new conflict list.
      ConflictList& oldConflictList = conflictLists[oldConflictListIdx];
      for (const ConflictList::Point& conflictPoint: oldConflictList.mPoints) {
        assignConflictPoint(conflictPoint.mPosition, newConflictLists);
      }
      conflictLists.Remove(oldConflictListIdx);
    }

    // Delete dead vertices and add all of the new conflict lists.
    for (const Ds::List<Vertex>::Iter& vertIt: deadVerts) {
      hull.mVertices.Erase(vertIt);
    }
    for (ConflictList& newList: newConflictLists) {
      if (newList.mPoints.Size() > 0) {
        conflictLists.Push(std::move(newList));
      }
    }
  }
  return hull;
}

} // namespace Math

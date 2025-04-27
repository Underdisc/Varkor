#include <functional>

#include "math/Constants.h"
#include "math/Hull.h"
#include "math/Ray.h"

#include "Input.h"
#include "debug/Draw.h"

namespace Math {

Math::Plane Hull::Face::Plane() const {
  Ds::Vector<Vec3> points;
  Ds::List<HalfEdge>::Iter currentEdge = mHalfEdge;
  do {
    points.Push(currentEdge->mVertex->mPosition);
    currentEdge = currentEdge->mNext;
  } while (currentEdge != mHalfEdge);
  return Math::Plane::Newell(points);
}

Vec3 Hull::Face::Center() const {
  Vec3 center = {0, 0, 0};
  int count = 0;
  Ds::List<HalfEdge>::Iter currentEdge = mHalfEdge;
  do {
    center += currentEdge->mVertex->mPosition;
    ++count;
    currentEdge = currentEdge->mNext;
  } while (currentEdge != mHalfEdge);
  return center / (float)count;
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
  Ds::List<HalfEdge>::Iter end = edgeList.end();
  Ds::List<Hull::Face>::Iter faces[4] = {
    hull.mFaces.PushBack({end}),
    hull.mFaces.PushBack({end}),
    hull.mFaces.PushBack({end}),
    hull.mFaces.PushBack({end}),
  };
  Ds::List<Hull::HalfEdge>::Iter edges[12] = {
    edgeList.PushBack({verts[0], end, end, end, faces[0]}),
    edgeList.PushBack({verts[1], end, end, end, faces[0]}),
    edgeList.PushBack({verts[2], end, end, end, faces[0]}),
    edgeList.PushBack({verts[1], end, end, end, faces[1]}),
    edgeList.PushBack({verts[0], end, end, end, faces[1]}),
    edgeList.PushBack({verts[3], end, end, end, faces[1]}),
    edgeList.PushBack({verts[2], end, end, end, faces[2]}),
    edgeList.PushBack({verts[1], end, end, end, faces[2]}),
    edgeList.PushBack({verts[3], end, end, end, faces[2]}),
    edgeList.PushBack({verts[0], end, end, end, faces[3]}),
    edgeList.PushBack({verts[2], end, end, end, faces[3]}),
    edgeList.PushBack({verts[3], end, end, end, faces[3]})};

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

  edges[1]->mPrev = edges[0];
  edges[2]->mPrev = edges[1];
  edges[0]->mPrev = edges[2];
  edges[4]->mPrev = edges[3];
  edges[5]->mPrev = edges[4];
  edges[3]->mPrev = edges[5];
  edges[7]->mPrev = edges[6];
  edges[8]->mPrev = edges[7];
  edges[6]->mPrev = edges[8];
  edges[10]->mPrev = edges[9];
  edges[11]->mPrev = edges[10];
  edges[9]->mPrev = edges[11];

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
    [&](const Vec3& point, Ds::Vector<ConflictList>& lists)
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

  // The convex hull has been obtained once all conflicting points are handled.
  while (conflictLists.Size() > 0) {
    // For all points in the conflict lists, find the point with maximum
    // distance from its respective plane. This point will be added next.
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

    // Treating the best point as an eye looking towards the current hull,
    // find the edges that form the horizon around the hull. The horizon edges
    // are the edges that border the faces to be deleted and they are stored in
    // a ccw order.
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

    // Imagine drawing a line from the best point to each of the vertices that
    // lie on the horizon. The new faces formed by these lines and the horizon
    // edges are created here. We create a new vertex for each horizon vertex
    // because it makes deleting no longer needed elements a bit easier.
    Ds::List<Vertex>::Iter newVertex = hull.mVertices.PushBack({newPoint, end});
    Ds::Vector<Ds::List<Vertex>::Iter> newHorizonVerts;
    for (const Ds::List<HalfEdge>::Iter& hEdge: horizon) {
      newHorizonVerts.Push(
        hull.mVertices.PushBack({hEdge->mVertex->mPosition, end}));
    }
    for (int i = 0; i < horizon.Size(); ++i) {
      Ds::List<HalfEdge>::Iter hEdge = horizon[i];
      Ds::List<HalfEdge>::Iter hEdgeNext = horizon[(i + 1) % horizon.Size()];
      Ds::List<Vertex>::Iter nhVert = newHorizonVerts[i];
      Ds::List<Vertex>::Iter nhVertNext =
        newHorizonVerts[(i + 1) % horizon.Size()];
      Ds::List<Face>::Iter newFace = hull.mFaces.PushBack({end});
      Ds::List<HalfEdge>::Iter newEdges[3] = {
        edgeList.PushBack({newVertex, end, end, end, newFace}),
        edgeList.PushBack({nhVert, end, end, end, newFace}),
        edgeList.PushBack({nhVertNext, end, end, end, newFace})};
      newFace->mHalfEdge = newEdges[0];
      newVertex->mHalfEdge = newEdges[0];
      nhVert->mHalfEdge = newEdges[1];
      Ds::List<HalfEdge>::Iter currentOldVertEdge = hEdge;
      do {
        currentOldVertEdge->mVertex = nhVert;
        currentOldVertEdge = currentOldVertEdge->mPrev->mTwin;
      } while (currentOldVertEdge != hEdgeNext->mTwin);
      hEdgeNext->mTwin = newEdges[1];
      newEdges[1]->mTwin = hEdgeNext;
      for (int e = 0; e < 3; ++e) {
        newEdges[e]->mNext = newEdges[(e + 1) % 3];
        newEdges[(e + 1) % 3]->mPrev = newEdges[e];
      }
    }
    for (int i = 0; i < horizon.Size(); ++i) {
      Ds::List<HalfEdge>::CIter hEdge = horizon[i];
      Ds::List<HalfEdge>::CIter hEdgeNext = horizon[(i + 1) % horizon.Size()];
      hEdge->mTwin->mNext->mTwin = hEdgeNext->mTwin->mPrev;
      hEdgeNext->mTwin->mPrev->mTwin = hEdge->mTwin->mNext;
    }

    // Any time we delete a face we need to see if that face had a conflict
    // list and, if so, save its conflict points in order to reassign them to
    // the new conflict lists at the end of the iteration.
    Ds::Vector<Vec3> conflictPoints;
    auto tryRemoveConflictList = [&](Ds::List<Face>::Iter face)
    {
      for (size_t c = 0; c < conflictLists.Size(); ++c) {
        if (conflictLists[c].mFace == face) {
          for (size_t p = 0; p < conflictLists[c].mPoints.Size(); ++p) {
            conflictPoints.Push(conflictLists[c].mPoints[p].mPosition);
          }
          conflictLists.LazyRemove(c);
          break;
        }
      }
    };

    // Delete dead vertices, edges, faces, and conflict lists that were covered
    // by the new faces.
    Ds::Vector<Ds::List<Vertex>::Iter> deadVerts;
    for (const Ds::List<Face>::Iter& faceIt: visitedFaces) {
      Ds::List<HalfEdge>::Iter currentEdge = faceIt->mHalfEdge;
      do {
        if (!deadVerts.Contains(currentEdge->mVertex)) {
          deadVerts.Push(currentEdge->mVertex);
        }
        Ds::List<HalfEdge>::Iter nextEdge = currentEdge->mNext;
        hull.mHalfEdges.Erase(currentEdge);
        currentEdge = nextEdge;
      } while (currentEdge != faceIt->mHalfEdge);
      tryRemoveConflictList(faceIt);
      hull.mFaces.Erase(faceIt);
    }
    for (const Ds::List<Vertex>::Iter& vertIt: deadVerts) {
      hull.mVertices.Erase(vertIt);
    }

    // We now need to merge faces that a coplanar. We only need check whether
    // the faces adjacent across new edges are coplanar.
    Ds::Vector<Ds::List<HalfEdge>::Iter> possibleMerges;
    Ds::List<HalfEdge>::Iter currentEdge = newVertex->mHalfEdge;
    do {
      possibleMerges.Push(currentEdge->mNext);
      possibleMerges.Push(currentEdge->mNext->mNext);
      currentEdge = currentEdge->mPrev->mTwin;
    } while (currentEdge != newVertex->mHalfEdge);
    auto tryRemovePossibleMerge = [&](Ds::List<HalfEdge>::Iter edge)
    {
      VResult<size_t> search = possibleMerges.Find(edge);
      if (search.Success()) {
        possibleMerges.LazyRemove(search.mValue);
      }
    };

    // As we merge faces, topological errors can arise. If only two edges emerge
    // from a vertex, we have a topological error. Every vertex needs to have 3
    // edges to make it be a part of the volume.
    auto ensureValidVertex = [&](Ds::List<Vertex>::Iter vertex)
    {
      int vertexEdgeCount = 0;
      Ds::Vector<Ds::List<HalfEdge>::Iter> vertexEdges;
      Ds::List<HalfEdge>::Iter currentVertexEdge = vertex->mHalfEdge;
      do {
        vertexEdges.Push(currentVertexEdge);
        currentVertexEdge = currentVertexEdge->mTwin->mNext;
      } while (currentVertexEdge != vertex->mHalfEdge);
      if (vertexEdges.Size() != 2) {
        return;
      }
      // How we deal with this topological error is determined by the number of
      // vertices the two adjacent faces have.
      int faceEdgeCounts[2] = {0, 0};
      for (int ve = 0; ve < 2; ++ve) {
        Ds::List<HalfEdge>::Iter currentFaceEdge = vertexEdges[ve];
        do {
          ++faceEdgeCounts[ve];
          currentFaceEdge = currentFaceEdge->mNext;
        } while (currentFaceEdge != vertexEdges[ve]);
      }
      Ds::List<HalfEdge>::Iter edges[2] = {
        vertex->mHalfEdge->mPrev, vertex->mHalfEdge};
      Ds::List<HalfEdge>::Iter edgeTwins[2] = {
        edges[1]->mTwin, edges[0]->mTwin};
      if (faceEdgeCounts[0] == 3 || faceEdgeCounts[1] == 3) {
        // One of the faces was a triangle, so we must remove the vertex and all
        // edges going to and emerging from it. First update all references to
        // edges that will be removed.
        edges[0]->mPrev->mNext = edgeTwins[1]->mNext;
        edgeTwins[1]->mNext->mPrev = edges[0]->mPrev;
        edges[1]->mNext->mPrev = edgeTwins[0]->mPrev;
        edgeTwins[0]->mPrev->mNext = edges[1]->mNext;
        // Ensure that the new face references a remaining half edge and that
        // all edges reference the new face.
        Ds::List<Face>::Iter newFace = hull.mFaces.PushBack({end});
        newFace->mHalfEdge = edges[1]->mNext;
        Ds::List<HalfEdge>::Iter currentEdge = edges[1]->mNext;
        do {
          currentEdge->mFace = newFace;
          currentEdge = currentEdge->mNext;
        } while (currentEdge != edges[1]->mNext);
        // Ensure that the remaing vertices reference existing half edges.
        edges[0]->mVertex->mHalfEdge = edges[0]->mPrev->mNext;
        edgeTwins[0]->mVertex->mHalfEdge = edgeTwins[0]->mPrev->mNext;
        // Remove no longer necessary elements.
        tryRemoveConflictList(edges[0]->mFace);
        tryRemoveConflictList(edgeTwins[0]->mFace);
        tryRemovePossibleMerge(edges[0]);
        tryRemovePossibleMerge(edges[1]);
        tryRemovePossibleMerge(edgeTwins[0]);
        tryRemovePossibleMerge(edgeTwins[1]);
        hull.mVertices.Erase(vertex);
        hull.mFaces.Erase(edges[0]->mFace);
        hull.mFaces.Erase(edgeTwins[0]->mFace);
        hull.mHalfEdges.Erase(edges[0]);
        hull.mHalfEdges.Erase(edges[1]);
        hull.mHalfEdges.Erase(edgeTwins[0]);
        hull.mHalfEdges.Erase(edgeTwins[1]);
      }
      else {
        // Neither of the adjacent faces were triangles, so the redundant
        // vertices edges are colinear and must be merged into a single edge.
        // Update all references to edges that will be removed.
        edges[0]->mNext = edges[1]->mNext;
        edges[0]->mTwin = edgeTwins[0];
        edges[1]->mNext->mPrev = edges[0];
        edgeTwins[0]->mNext = edgeTwins[1]->mNext;
        edgeTwins[0]->mTwin = edges[0];
        edgeTwins[1]->mNext->mPrev = edgeTwins[0];
        // Ensure that the faces reference existing edges.
        edges[0]->mFace->mHalfEdge = edges[0];
        edgeTwins[0]->mFace->mHalfEdge = edgeTwins[0];
        // Remove no longer necessary elements.
        tryRemovePossibleMerge(edges[1]);
        tryRemovePossibleMerge(edgeTwins[1]);
        hull.mVertices.Erase(vertex);
        hull.mHalfEdges.Erase(edges[1]);
        hull.mHalfEdges.Erase(edgeTwins[1]);
      }
    };

    // We merge coplanar faces given one of the edges shared between them.
    auto mergeFaces = [&](Ds::List<HalfEdge>::Iter edge)
    {
      Ds::List<Face>::Iter newFace = hull.mFaces.PushBack({end});
      Ds::List<HalfEdge>::Iter edgeTwin = edge->mTwin;
      // Link the edges going away and towards the deleted edge.
      edge->mPrev->mNext = edgeTwin->mNext;
      edge->mNext->mPrev = edgeTwin->mPrev;
      edgeTwin->mPrev->mNext = edge->mNext;
      edgeTwin->mNext->mPrev = edge->mPrev;
      // Ensure that the new face and vertices reference a remaining half edge
      // and that all remaining edges reference the new face.
      newFace->mHalfEdge = edge->mNext;
      edge->mVertex->mHalfEdge = edgeTwin->mNext;
      edgeTwin->mVertex->mHalfEdge = edge->mNext;
      Ds::List<HalfEdge>::Iter currentEdge = edge->mNext;
      do {
        currentEdge->mFace = newFace;
        currentEdge = currentEdge->mNext;
      } while (currentEdge != edge->mNext);
      // Ensure that the two vertices that lost an edge an edge are still valid
      // and erase no long necessary elements.
      ensureValidVertex(edge->mVertex);
      ensureValidVertex(edgeTwin->mVertex);
      tryRemoveConflictList(edge->mFace);
      tryRemoveConflictList(edgeTwin->mFace);
      tryRemovePossibleMerge(edge);
      tryRemovePossibleMerge(edgeTwin);
      hull.mFaces.Erase(edge->mFace);
      hull.mFaces.Erase(edgeTwin->mFace);
      hull.mHalfEdges.Erase(edge);
      hull.mHalfEdges.Erase(edgeTwin);
    };

    // Merge faces coplanar with one another over possible merge edges.
    while (!possibleMerges.Empty()) {
      Ds::List<HalfEdge>::Iter edge = possibleMerges[0];
      Ds::List<HalfEdge>::Iter twinEdge = edge->mTwin;
      Plane facePlane = edge->mFace->Plane();
      Plane twinFacePlane = twinEdge->mFace->Plane();
      if (Near(facePlane, twinFacePlane)) {
        mergeFaces(edge);
      }
      else {
        possibleMerges.LazyRemove(0);
      }
    }

    // Create new conflict lists from the faces that remain after merging,
    // distribute orphaned conflict points to the new conflict lists, and
    // ignore any conflict lists that have no conflict points.
    Ds::Vector<ConflictList> newConflictLists;
    currentEdge = newVertex->mHalfEdge;
    do {
      ConflictList newConflictList;
      newConflictList.mFace = currentEdge->mFace;
      newConflictList.mPlane = currentEdge->mFace->Plane();
      newConflictLists.Push(newConflictList);
      currentEdge = currentEdge->mPrev->mTwin;
    } while (currentEdge != newVertex->mHalfEdge);
    for (const Vec3& point: conflictPoints) {
      assignConflictPoint(point, newConflictLists);
    }
    for (ConflictList& newList: newConflictLists) {
      if (newList.mPoints.Size() > 0) {
        conflictLists.Push(std::move(newList));
      }
    }
  }
  return hull;
}

Result Hull::IsStructureStable() const {
  Ds::List<Math::Hull::Vertex>::CIter vertIt = mVertices.cbegin();
  Ds::List<Math::Hull::Vertex>::CIter vertEnd = mVertices.cend();
  for (; vertIt != vertEnd; ++vertIt) {
    if (vertIt != vertIt->mHalfEdge->mVertex) {
      return Result("Vertex edge has incorrect vertex reference");
    }
  }

  size_t encounteredEdges = 0;
  Ds::List<Math::Hull::Face>::CIter faceIt = mFaces.cbegin();
  Ds::List<Math::Hull::Face>::CIter faceEnd = mFaces.cend();
  for (; faceIt != faceEnd; ++faceIt) {
    Ds::List<Math::Hull::HalfEdge>::CIter currentEdge = faceIt->mHalfEdge;
    do {
      if (faceIt != currentEdge->mFace) {
        return Result("Edge in face's loop refers to wrong face");
      }
      if (currentEdge != currentEdge->mTwin->mTwin) {
        return Result("Twin references of twin pair aren't cyclic");
      }
      ++encounteredEdges;
    } while ((currentEdge = currentEdge->mNext) != faceIt->mHalfEdge);
  }
  if (encounteredEdges != mHalfEdges.Size()) {
    return Result("Number of visited edges not equivalent to edge list size");
  }
  return Result();
}

} // namespace Math

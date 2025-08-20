#include <functional>

#include "ds/HashMap.h"
#include "math/Constants.h"
#include "math/Hull.h"
#include "math/Ray.h"

template<>
size_t Ds::Hash(const Ds::List<Math::Hull::Face>::Iter& it) {
  return (size_t)it.Current();
}

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

  // Find an epsilon that accounts for the span of the point collection.
  const Vec3** eps = extremePoints;
  Vec3 maxes = {
    Max(Abs((*eps[0])[0]), Abs((*eps[3])[0])),
    Max(Abs((*eps[1])[1]), Abs((*eps[4])[1])),
    Max(Abs((*eps[2])[2]), Abs((*eps[5])[2]))};
  float epsilon = 3.0f * (maxes[0] + maxes[1] + maxes[2]) * nEpsilon;

  // Cases where extreme points collapse or we don't get a polyhedron need to
  // be handled before we construct a polyhedron. We choose the first four
  // extreme points we find to create a polyhedron.
  Hull hull;
  hull.mVertices.PushBack({*extremePoints[0], hull.mHalfEdges.end()});
  Ds::Vector<Ds::List<Vertex>::Iter> verts;
  verts.Push(hull.mVertices.Back());
  for (size_t i = 1; i < 6; ++i) {
    const Vec3& newPoint = *extremePoints[i];
    if (verts.Size() == 1) {
      if (!Math::Near(newPoint, verts[0]->mPosition, epsilon)) {
        hull.mVertices.PushBack({newPoint, hull.mHalfEdges.end()});
        verts.Push(hull.mVertices.Back());
      }
    }
    else if (verts.Size() == 2) {
      Math::Ray edge =
        Math::Ray::Points(verts[0]->mPosition, verts[1]->mPosition);
      if (!Math::Near(edge.DistanceSq(newPoint), 0.0f, epsilon)) {
        hull.mVertices.PushBack({newPoint, hull.mHalfEdges.end()});
        verts.Push(hull.mVertices.Back());
      }
    }
    else if (verts.Size() == 3) {
      Math::Plane plane = Math::Plane::Points(
        verts[0]->mPosition, verts[1]->mPosition, verts[2]->mPosition);
      float pointDist = plane.Distance(newPoint);
      if (!Math::Near(pointDist, 0.0f, epsilon)) {
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
    Math::Plane mPlane;
    struct Point {
      Vec3 mPosition;
      float mDistance;
    };
    Ds::Vector<Point> mPoints;
    ConflictList(const Math::Plane& plane) {
      mPlane = plane;
    }
  };
  Ds::HashMap<Ds::List<Face>::Iter, ConflictList> faceConflictLists;
  Ds::List<Face>::Iter faceIt = hull.mFaces.begin();
  while (faceIt != hull.mFaces.end()) {
    faceConflictLists.Insert(faceIt, ConflictList(faceIt->Plane()));
    ++faceIt;
  }

  // Find the plane each point is closest to and give the point to that plane's
  // conflict list. Conflict lists that didn't receive a point are removed.
  auto assignConflictPoint =
    [&](
      const Vec3& point,
      Ds::HashMap<Ds::List<Face>::Iter, ConflictList>& faceConflictLists) {
      float minDist = FLT_MAX;
      auto bestFaceConflictListIt = faceConflictLists.end();
      auto faceContlictListIt = faceConflictLists.begin();
      while (faceContlictListIt != faceConflictLists.end()) {
        float dist = faceContlictListIt->mValue.mPlane.Distance(point);
        if (dist > epsilon && dist < minDist) {
          minDist = dist;
          bestFaceConflictListIt = faceContlictListIt;
        }
        ++faceContlictListIt;
      }
      if (bestFaceConflictListIt != faceConflictLists.end()) {
        bestFaceConflictListIt->mValue.mPoints.Push({point, minDist});
      }
    };

  // We only use unique points to define the hull. Equivalent points can
  // potentially be added to the hull multiple times, resulting in a degenerate
  // face. This is caused by a point lying outside of an average plane defined
  // by a face containing an equivalent point.
  Ds::Vector<Vec3> uniquePoints;
  for (const Vec3& point: points) {
    bool unique = true;
    for (const Vec3& uniquePoint: uniquePoints) {
      if (Near(point, uniquePoint, epsilon)) {
        unique = false;
        break;
      }
    }
    if (unique) {
      uniquePoints.Push(point);
    }
  }
  for (const Vec3& uniquePoint: uniquePoints) {
    assignConflictPoint(uniquePoint, faceConflictLists);
  }

  // Any faces without conflicting points do not need to be considered.
  auto faceConflictListIt = faceConflictLists.begin();
  while (faceConflictListIt != faceConflictLists.end()) {
    if (faceConflictListIt->mValue.mPoints.Size() == 0) {
      faceConflictListIt = faceConflictLists.Remove(faceConflictListIt);
    }
    else {
      ++faceConflictListIt;
    }
  }

  // The convex hull has been obtained once all conflicting points are handled.
  while (faceConflictLists.Size() > 0) {
    // For all points in the conflict lists, find the point with maximum
    // distance from its respective plane. This point will be added next.
    float maxDist = -FLT_MAX;
    auto bestFaceConflictListIt = faceConflictLists.end();
    int bestConflictPointIdx = -1;
    auto it = faceConflictLists.begin();
    while (it != faceConflictLists.end()) {
      for (int p = 0; p < it->mValue.mPoints.Size(); ++p) {
        const ConflictList::Point& conflictPoint = it->mValue.mPoints[p];
        if (conflictPoint.mDistance > maxDist) {
          maxDist = conflictPoint.mDistance;
          bestFaceConflictListIt = it;
          bestConflictPointIdx = p;
        }
      }
      ++it;
    }

    // Treating the best point as an eye looking towards the current hull,
    // find the edges that form the horizon around the hull. The horizon edges
    // are the edges that border the faces to be deleted and they are stored in
    // a ccw order.
    ConflictList& conflictList = bestFaceConflictListIt->mValue;
    Vec3 newPoint = conflictList.mPoints[bestConflictPointIdx].mPosition;
    // The point is being added to the hull and is hence no longer a conflict.
    conflictList.mPoints.LazyRemove(bestConflictPointIdx);
    Ds::Vector<Ds::List<HalfEdge>::Iter> horizon;
    Ds::Vector<Ds::List<Face>::Iter> visitedFaces;
    std::function<void(Ds::List<HalfEdge>::CIter)> visitEdge =
      [&](Ds::List<HalfEdge>::CIter edge) {
        if (visitedFaces.Contains(edge->mFace)) {
          return;
        }
        visitedFaces.Push(edge->mFace);
        Ds::List<HalfEdge>::CIter currentEdge = edge;
        do {
          Ds::List<HalfEdge>::Iter twin = currentEdge->mTwin;
          Math::Plane twinPlane = twin->mFace->Plane();
          if (twinPlane.HalfSpaceContains(newPoint, epsilon)) {
            horizon.Push(twin);
          }
          else {
            visitEdge(twin);
          }
        } while ((currentEdge = currentEdge->mNext) != edge);
      };
    visitEdge(bestFaceConflictListIt->Key()->mHalfEdge);

    // We create a new vertex for each horizon vertex because it makes deleting
    // no longer needed elements a bit easier.
    Ds::Vector<Ds::List<Vertex>::Iter> newHorizonVerts;
    for (const Ds::List<HalfEdge>::Iter& hEdge: horizon) {
      newHorizonVerts.Push(
        hull.mVertices.PushBack({hEdge->mVertex->mPosition, end}));
    }

    // Imagine drawing a line from the best point to each of the vertices that
    // lie on the horizon. The new faces formed by these lines and the horizon
    // edges are created here.
    Ds::List<Vertex>::Iter newVertex = hull.mVertices.PushBack({newPoint, end});
    Ds::HashMap<Ds::List<Face>::Iter, ConflictList> newFaceConflictLists;
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

      // Ensure that all edges referencing the old horizon vertex reference the
      // new horizon vertex.
      Ds::List<HalfEdge>::Iter currentOldVertEdge = hEdge;
      do {
        currentOldVertEdge->mVertex = nhVert;
        currentOldVertEdge = currentOldVertEdge->mPrev->mTwin;
      } while (currentOldVertEdge != hEdgeNext->mTwin);

      // Link together all edge edge references and create a conflict list
      // representing the new face.
      hEdgeNext->mTwin = newEdges[1];
      newEdges[1]->mTwin = hEdgeNext;
      for (int e = 0; e < 3; ++e) {
        newEdges[e]->mNext = newEdges[(e + 1) % 3];
        newEdges[(e + 1) % 3]->mPrev = newEdges[e];
      }
      newFaceConflictLists.Insert(newFace, ConflictList(newFace->Plane()));
    }

    // Set the twin references of all edges going to and from the new vertex.
    for (int i = 0; i < horizon.Size(); ++i) {
      Ds::List<HalfEdge>::CIter hEdge = horizon[i];
      Ds::List<HalfEdge>::CIter hEdgeNext = horizon[(i + 1) % horizon.Size()];
      hEdge->mTwin->mNext->mTwin = hEdgeNext->mTwin->mPrev;
      hEdgeNext->mTwin->mPrev->mTwin = hEdge->mTwin->mNext;
    }

    // Any time we delete a face, we need to see if that face has an existing or
    // new conflict list associated with it. If it has an existing conflict
    // list, we save its conflict points in order to reassign them to the new
    // set of conflict lists at the end of the iteration.
    Ds::Vector<Vec3> conflictPoints;
    auto tryRemoveFaceConflictList = [&](Ds::List<Face>::Iter faceIt) {
      auto faceConflictIt = faceConflictLists.Find(faceIt);
      if (faceConflictIt != faceConflictLists.end()) {
        const ConflictList& conflictList = faceConflictIt->mValue;
        for (size_t p = 0; p < conflictList.mPoints.Size(); ++p) {
          conflictPoints.Push(conflictList.mPoints[p].mPosition);
        }
        faceConflictLists.Remove(faceConflictIt);
      }
      auto newFaceConflictIt = newFaceConflictLists.Find(faceIt);
      if (newFaceConflictIt != newFaceConflictLists.end()) {
        newFaceConflictLists.Remove(newFaceConflictIt);
      }
    };

    // Delete dead vertices, edges, faces, and conflict lists that were covered
    // by the new faces.
    Ds::Vector<Ds::List<Vertex>::Iter> deadVerts;
    Ds::Vector<Ds::List<HalfEdge>::Iter> deadEdges;
    for (const Ds::List<Face>::Iter& faceIt: visitedFaces) {
      Ds::List<HalfEdge>::Iter currentEdge = faceIt->mHalfEdge;
      do {
        if (!deadVerts.Contains(currentEdge->mVertex)) {
          deadVerts.Push(currentEdge->mVertex);
        }
        deadEdges.Push(currentEdge);
        currentEdge = currentEdge->mNext;
      } while (currentEdge != faceIt->mHalfEdge);
      tryRemoveFaceConflictList(faceIt);
      hull.mFaces.Erase(faceIt);
    }
    for (const Ds::List<Vertex>::Iter& vertIt: deadVerts) {
      hull.mVertices.Erase(vertIt);
    }
    for (const Ds::List<HalfEdge>::Iter& edgeIt: deadEdges) {
      hull.mHalfEdges.Erase(edgeIt);
    }

    // We now need to merge faces that are coplanar. We only need to check
    // whether faces adjacent across new edges are coplanar. We collect all of
    // those edges here.
    Ds::Vector<Ds::List<HalfEdge>::Iter> possibleMerges;
    Ds::List<HalfEdge>::Iter currentEdge = newVertex->mHalfEdge;
    do {
      possibleMerges.Push(currentEdge->mNext);
      possibleMerges.Push(currentEdge->mNext->mNext);
      currentEdge = currentEdge->mPrev->mTwin;
    } while (currentEdge != newVertex->mHalfEdge);
    auto tryRemovePossibleMerge = [&](Ds::List<HalfEdge>::Iter edge) {
      VResult<size_t> search = possibleMerges.Find(edge);
      if (search.Success()) {
        possibleMerges.LazyRemove(search.mValue);
      }
    };

    // As we merge faces, topological errors can arise. If only two edges emerge
    // from a vertex, we have a topological error. Every vertex needs to have 3
    // edges to make it be a part of the volume.
    Ds::Vector<Ds::List<Vertex>::Iter> mergedVerts;
    Ds::Vector<Ds::List<HalfEdge>::Iter> mergedEdges;
    auto ensureValidVertex = [&](Ds::List<Vertex>::Iter vertex) {
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
        // When one of the faces is a triangle, we must remove the vertex and
        // all edges going to and from it. First we update all references to
        // edges that will be removed.
        edges[0]->mPrev->mNext = edgeTwins[1]->mNext;
        edgeTwins[1]->mNext->mPrev = edges[0]->mPrev;
        edges[1]->mNext->mPrev = edgeTwins[0]->mPrev;
        edgeTwins[0]->mPrev->mNext = edges[1]->mNext;

        // Create the new face used to reprsent the merged faces.
        Ds::List<Face>::Iter newFace = hull.mFaces.PushBack({edges[1]->mNext});
        newFaceConflictLists.Insert(newFace, ConflictList(newFace->Plane()));

        // Ensure all edges within the merged faces reference the new face and
        // that remaining vertices reference existing half edges.
        Ds::List<HalfEdge>::Iter currentEdge = edges[1]->mNext;
        do {
          currentEdge->mFace = newFace;
          currentEdge = currentEdge->mNext;
        } while (currentEdge != edges[1]->mNext);
        edges[0]->mVertex->mHalfEdge = edges[0]->mPrev->mNext;
        edgeTwins[0]->mVertex->mHalfEdge = edgeTwins[0]->mPrev->mNext;

        // Remove no longer necessary elements.
        tryRemoveFaceConflictList(edges[0]->mFace);
        tryRemoveFaceConflictList(edgeTwins[0]->mFace);
        tryRemovePossibleMerge(edges[0]);
        tryRemovePossibleMerge(edges[1]);
        tryRemovePossibleMerge(edgeTwins[0]);
        tryRemovePossibleMerge(edgeTwins[1]);
        mergedVerts.Push(vertex);
        hull.mFaces.Erase(edges[0]->mFace);
        hull.mFaces.Erase(edgeTwins[0]->mFace);
        mergedEdges.Push(edges[0]);
        mergedEdges.Push(edges[1]);
        mergedEdges.Push(edgeTwins[0]);
        mergedEdges.Push(edgeTwins[1]);
      }
      else {
        // When neither of the adjacent faces are triangles, the vertex edges
        // are colinear and must be merged into a single edge. We repurpose one
        // set of half edges to represent the merged edge and update  references
        // to the other two half edges that will be removed.
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
        mergedVerts.Push(vertex);
        mergedEdges.Push(edges[1]);
        mergedEdges.Push(edgeTwins[1]);
      }
    };

    // Coplanar faces are merged using one of the edges shared between them.
    auto mergeFaces = [&](Ds::List<HalfEdge>::Iter edge) {
      // Link the edges going away and towards the deleted edge.
      Ds::List<HalfEdge>::Iter edgeTwin = edge->mTwin;
      edge->mPrev->mNext = edgeTwin->mNext;
      edge->mNext->mPrev = edgeTwin->mPrev;
      edgeTwin->mPrev->mNext = edge->mNext;
      edgeTwin->mNext->mPrev = edge->mPrev;

      // Create the new face and ensure vertices reference a remaining half edge
      // and that all remaining edges reference the new face.
      Ds::List<Face>::Iter newFace = hull.mFaces.PushBack({edge->mNext});
      newFaceConflictLists.Insert(newFace, ConflictList(newFace->Plane()));
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
      tryRemoveFaceConflictList(edge->mFace);
      tryRemoveFaceConflictList(edgeTwin->mFace);
      tryRemovePossibleMerge(edge);
      tryRemovePossibleMerge(edgeTwin);
      hull.mFaces.Erase(edge->mFace);
      hull.mFaces.Erase(edgeTwin->mFace);
      mergedEdges.Push(edge);
      mergedEdges.Push(edgeTwin);
    };

    // Check whether a merge should be performed over all possible merges.
    while (!possibleMerges.Empty()) {
      // If a face's halfspace contains the center of the adjacent face and vice
      // versa, the edge is considered convex.
      Ds::List<HalfEdge>::Iter edge = possibleMerges.Top();
      Ds::List<HalfEdge>::Iter twinEdge = edge->mTwin;
      Plane facePlane = edge->mFace->Plane();
      Vec3 faceCenter = edge->mFace->Center();
      Plane twinFacePlane = twinEdge->mFace->Plane();
      Vec3 twinFaceCenter = twinEdge->mFace->Center();
      bool convex = facePlane.HalfSpaceContains(twinFaceCenter, epsilon) &&
        twinFacePlane.HalfSpaceContains(faceCenter, epsilon);

      // If the edge is convex and the angle between face normals lies within
      // the epsilon, the faces are merged.
      float angle = Math::Angle(facePlane.Normal(), twinFacePlane.Normal());
      const float angleEpsilon = 0.015f;
      if (convex && Near(angle, 0.0f, angleEpsilon)) {
        mergeFaces(edge);
      }
      else {
        possibleMerges.Pop();
      }
    }
    for (const Ds::List<Vertex>::Iter& vertIt: mergedVerts) {
      hull.mVertices.Erase(vertIt);
    }
    for (const Ds::List<HalfEdge>::Iter& edgeIt: mergedEdges) {
      hull.mHalfEdges.Erase(edgeIt);
    }

    // Distribute orphaned conflict points to the new conflict lists. We ignore
    // any conflict lists that have no conflict points.
    for (const Vec3& point: conflictPoints) {
      assignConflictPoint(point, newFaceConflictLists);
    }
    for (auto& newFaceConflictListIt: newFaceConflictLists) {
      ConflictList& newFaceConflistList = newFaceConflictListIt.mValue;
      if (newFaceConflistList.mPoints.Size() > 0) {
        faceConflictLists.Insert(
          newFaceConflictListIt.Key(), std::move(newFaceConflistList));
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

Result Hull::HasDegenerateFace() const {
  for (const Face& face: mFaces) {
    auto edgeIt = face.mHalfEdge;
    do {
      const Vec3& a = edgeIt->mPrev->mVertex->mPosition;
      const Vec3& b = edgeIt->mVertex->mPosition;
      const Vec3& c = edgeIt->mNext->mVertex->mPosition;
      Vec3 ba = a - b;
      Vec3 bc = c - b;
      if (Near(ba, bc)) {
        return Result("Degenerate Face Found");
      }
      edgeIt = edgeIt->mNext;
    } while (edgeIt != face.mHalfEdge);
  }
  return Result();
}

} // namespace Math

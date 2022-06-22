#include "gfx/LineModel.h"
#include "gfx/Model.h"
#include "math/Geometry.h"
#include "math/Quaternion.h"

namespace Gfx {

void InitLineModel(
  AssetId modelId,
  const Ds::Vector<Vec3> points,
  float fill,
  float thickness,
  TerminalType startTerminal,
  TerminalType endTerminal)
{
  LogAbortIf(
    points.Size() < 2,
    "Initialialzing a line model requires two 2 or more points.");
  auto& modelAsset = AssLib::GetAsset<Gfx::Model>(modelId);

  // There is no mesh if the fill is 0.
  if (fill == 0.0f) {
    modelAsset.mResource.Init(nullptr, nullptr, 0, 0, 0, 0);
    return;
  }

  // Every line segment has a tangent, normal, and binormal associated with it.
  // We find all of these vectors first.
  Ds::Vector<Vec3> tangents;
  Ds::Vector<Vec3> normals;
  Ds::Vector<Vec3> binormals;
  tangents.Reserve(points.Size());
  normals.Reserve(points.Size());
  binormals.Reserve(points.Size());
  tangents.Push(Math::Normalize(points[1] - points[0]));
  normals.Push(Math::Normalize(Math::PerpendicularTo(tangents[0])));
  binormals.Push(Math::Cross(normals[0], tangents[0]));
  for (int i = 1; i < points.Size() - 1; ++i) {
    tangents.Push(Math::Normalize(points[i + 1] - points[i]));
    Math::Quaternion rotation;
    rotation.FromTo(tangents[i - 1], tangents[i]);
    normals.Push(rotation.Rotate(normals[i - 1]));
    binormals.Push(rotation.Rotate(binormals[i - 1]));
  }
  // We make a copy of the last basis vectors because it makes writing the
  // remaining code easier.
  tangents.Push(tangents.Top());
  normals.Push(normals.Top());
  binormals.Push(binormals.Top());

  // We account for the start terminal type first. ru means right up. lu means
  // left up. These are just short acornyms to represent the normal binormal
  // combination being used.
  Ds::Vector<Vec3> allVerts;
  float halfThickness = thickness / 2.0f;
  Vec3 ruOffset = halfThickness * (normals[0] + binormals[0]);
  Vec3 luOffset = halfThickness * (-normals[0] + binormals[0]);
  switch (startTerminal) {
  case TerminalType::Flat:
    allVerts.Push(points[0] + ruOffset);
    allVerts.Push(points[0] + luOffset);
    allVerts.Push(points[0] - ruOffset);
    allVerts.Push(points[0] - luOffset);
    break;
  case TerminalType::Point: allVerts.Push(points[0], 4); break;
  case TerminalType::CollapsedNormal:
    allVerts.Push(points[0] + binormals[0] * halfThickness);
    allVerts.Push(points[0] + binormals[0] * halfThickness);
    allVerts.Push(points[0] - binormals[0] * halfThickness);
    allVerts.Push(points[0] - binormals[0] * halfThickness);
    break;
  case TerminalType::CollapsedBinormal:
    allVerts.Push(points[0] + normals[0] * halfThickness);
    allVerts.Push(points[0] - normals[0] * halfThickness);
    allVerts.Push(points[0] - normals[0] * halfThickness);
    allVerts.Push(points[0] + normals[0] * halfThickness);
    break;
  case TerminalType::Arrow:
    allVerts.Push(points[0] - tangents[0] * thickness * 2.0f, 4);
    allVerts.Push(points[0] + ruOffset * 2.0f);
    allVerts.Push(points[0] + luOffset * 2.0f);
    allVerts.Push(points[0] - ruOffset * 2.0f);
    allVerts.Push(points[0] - luOffset * 2.0f);
    allVerts.Push(points[0] + ruOffset);
    allVerts.Push(points[0] + luOffset);
    allVerts.Push(points[0] - ruOffset);
    allVerts.Push(points[0] - luOffset);
    break;
  }

  // We find points that are on the edges of each line segment. These can be
  // used in combination with the tangents of the line segments to find the
  // vertices for a corner.
  Ds::Vector<Vec3> edgePoints;
  edgePoints.Reserve(points.Size() * 4);
  for (int i = 0; i < points.Size(); ++i) {
    Vec3 diagonal = halfThickness * (normals[i] + binormals[i]);
    Vec3 reflectedDiagonal = halfThickness * (-normals[i] + binormals[i]);
    edgePoints.Push(points[i] + diagonal);
    edgePoints.Push(points[i] + reflectedDiagonal);
    edgePoints.Push(points[i] - diagonal);
    edgePoints.Push(points[i] - reflectedDiagonal);
  }

  // Now we find all of the interior points that make up the line.
  for (int i = 1; i < points.Size() - 1; ++i) {
    for (int j = 0; j < 4; ++j) {
      int edgePointIndex = i * 4 + j;
      Math::Ray prevRay, currentRay;
      prevRay.InitNormalized(edgePoints[edgePointIndex - 4], tangents[i - 1]);
      currentRay.InitNormalized(edgePoints[edgePointIndex], tangents[i]);
      if (prevRay.HasClosestTo(currentRay)) {
        allVerts.Push(prevRay.ClosestPointTo(currentRay));
      }
      else {
        allVerts.Push(currentRay.mStart);
      }
    }
  }

  // Handle the end terminal type.
  ruOffset = halfThickness * (normals.Top() + binormals.Top());
  luOffset = halfThickness * (-normals.Top() + binormals.Top());
  switch (endTerminal) {
  case TerminalType::Flat:
    allVerts.Push(points.Top() + ruOffset);
    allVerts.Push(points.Top() + luOffset);
    allVerts.Push(points.Top() - ruOffset);
    allVerts.Push(points.Top() - luOffset);
    break;
  case TerminalType::Point: allVerts.Push(points.Top(), 4); break;
  case TerminalType::CollapsedNormal:
    allVerts.Push(points.Top() + binormals.Top() * halfThickness);
    allVerts.Push(points.Top() + binormals.Top() * halfThickness);
    allVerts.Push(points.Top() - binormals.Top() * halfThickness);
    allVerts.Push(points.Top() - binormals.Top() * halfThickness);
    break;
  case TerminalType::CollapsedBinormal:
    allVerts.Push(points.Top() + normals.Top() * halfThickness);
    allVerts.Push(points.Top() - normals.Top() * halfThickness);
    allVerts.Push(points.Top() - normals.Top() * halfThickness);
    allVerts.Push(points.Top() + normals.Top() * halfThickness);
    break;
  case TerminalType::Arrow:
    allVerts.Push(points.Top() + ruOffset);
    allVerts.Push(points.Top() + luOffset);
    allVerts.Push(points.Top() - ruOffset);
    allVerts.Push(points.Top() - luOffset);
    allVerts.Push(points.Top() + ruOffset * 2.0f);
    allVerts.Push(points.Top() + luOffset * 2.0f);
    allVerts.Push(points.Top() - ruOffset * 2.0f);
    allVerts.Push(points.Top() - luOffset * 2.0f);
    allVerts.Push(points.Top() + tangents.Top() * thickness * 2.0f, 4);
    break;
  }

  // Now we need to account for the fill amount. We start by finding the total
  // length of one arbitrary edge.
  Ds::Vector<float> lengths;
  float totalLength = 0.0f;
  for (int i = 4; i < allVerts.Size(); i += 4) {
    lengths.Push(Math::Magnitude(allVerts[i] - allVerts[i - 4]));
    totalLength += lengths.Top();
  }

  // The vertex and element buffer that will be used to initialize the model.
  Ds::Vector<Vec3> verts;
  Ds::Vector<unsigned int> elements;
  verts.Reserve(allVerts.Size());
  elements.Reserve((allVerts.Size() / 4) * 24 + 12);

  // When the fill is greater than 0, we construct from the start to the end.
  if (fill > 0.0f) {
    float remainingLength = totalLength * fill;

    elements.Push(0);
    elements.Push(2);
    elements.Push(3);

    elements.Push(0);
    elements.Push(1);
    elements.Push(2);

    verts.Push(allVerts[0]);
    verts.Push(allVerts[1]);
    verts.Push(allVerts[2]);
    verts.Push(allVerts[3]);

    int lastRu = 0;
    for (int i = 4; i < allVerts.Size(); i += 4) {
      lastRu = i;
      for (int j = 0; j < 3; ++j) {
        elements.Push(i + j - 4);
        elements.Push(i + j);
        elements.Push(i + j + 1);

        elements.Push(i + j - 4);
        elements.Push(i + j + 1);
        elements.Push(i + j - 3);
      }
      elements.Push(i - 1);
      elements.Push(i + 3);
      elements.Push(i);

      elements.Push(i - 1);
      elements.Push(i);
      elements.Push(i - 4);

      int lengthIndex = i / 4 - 1;
      if (remainingLength < lengths[lengthIndex]) {
        float t = remainingLength / lengths[lengthIndex];
        verts.Push(allVerts[i - 4] + t * (allVerts[i + 0] - allVerts[i - 4]));
        verts.Push(allVerts[i - 3] + t * (allVerts[i + 1] - allVerts[i - 3]));
        verts.Push(allVerts[i - 2] + t * (allVerts[i + 2] - allVerts[i - 2]));
        verts.Push(allVerts[i - 1] + t * (allVerts[i + 3] - allVerts[i - 1]));
        break;
      }
      else {
        verts.Push(allVerts[i + 0]);
        verts.Push(allVerts[i + 1]);
        verts.Push(allVerts[i + 2]);
        verts.Push(allVerts[i + 3]);
        remainingLength -= lengths[lengthIndex];
      }
    }
    elements.Push(lastRu);
    elements.Push(lastRu + 2);
    elements.Push(lastRu + 1);

    elements.Push(lastRu);
    elements.Push(lastRu + 3);
    elements.Push(lastRu + 2);
  }

  // When the fill is less than 0, we construct from the end to the start.
  if (fill < 0.0f) {
    float remainingLength = totalLength * fill * -1.0f;

    elements.Push(0);
    elements.Push(2);
    elements.Push(1);

    elements.Push(0);
    elements.Push(3);
    elements.Push(2);

    verts.Push(allVerts[allVerts.Size() - 4]);
    verts.Push(allVerts[allVerts.Size() - 3]);
    verts.Push(allVerts[allVerts.Size() - 2]);
    verts.Push(allVerts[allVerts.Size() - 1]);

    int lastRu = 0;
    for (int i = (int)allVerts.Size() - 4; i > 0; i -= 4) {
      unsigned int v = (unsigned int)verts.Size();
      lastRu = v;
      for (int j = 0; j < 3; ++j) {
        elements.Push(v + j - 4);
        elements.Push(v + j + 1);
        elements.Push(v + j);

        elements.Push(v + j - 4);
        elements.Push(v + j - 3);
        elements.Push(v + j + 1);
      }
      elements.Push(v - 1);
      elements.Push(v);
      elements.Push(v + 3);

      elements.Push(v - 1);
      elements.Push(v - 4);
      elements.Push(v);

      int lengthIndex = i / 4 - 1;
      if (remainingLength < lengths[lengthIndex]) {
        float t = remainingLength / lengths[lengthIndex];
        verts.Push(allVerts[i + 0] + t * (allVerts[i - 4] - allVerts[i + 0]));
        verts.Push(allVerts[i + 1] + t * (allVerts[i - 3] - allVerts[i + 1]));
        verts.Push(allVerts[i + 2] + t * (allVerts[i - 2] - allVerts[i + 2]));
        verts.Push(allVerts[i + 3] + t * (allVerts[i - 1] - allVerts[i + 3]));
        break;
      }
      else {
        verts.Push(allVerts[i - 4]);
        verts.Push(allVerts[i - 3]);
        verts.Push(allVerts[i - 2]);
        verts.Push(allVerts[i - 1]);
        remainingLength -= lengths[lengthIndex];
      }
    }
    elements.Push(lastRu);
    elements.Push(lastRu + 1);
    elements.Push(lastRu + 2);

    elements.Push(lastRu);
    elements.Push(lastRu + 2);
    elements.Push(lastRu + 3);
  }

  // We can finally initialize the model. Everything needed to do it is ugly,
  // but it works.
  modelAsset.mResource.Init(
    (void*)verts.CData(),
    (void*)elements.CData(),
    (unsigned int)(sizeof(Vec3) * verts.Size()),
    (unsigned int)(sizeof(unsigned int) * elements.Size()),
    Gfx::Attribute::Position,
    (unsigned int)elements.Size());
}

} // namespace Gfx
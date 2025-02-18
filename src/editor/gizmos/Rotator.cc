#include "editor/gizmos/Rotator.h"
#include "Input.h"
#include "comp/Mesh.h"
#include "comp/Transform.h"
#include "editor/Editor.h"
#include "editor/gizmos/Gizmos.h"
#include "gfx/Material.h"
#include "gfx/Renderer.h"
#include "math/Constants.h"
#include "math/Intersection.h"
#include "rsl/Library.h"

namespace Editor {
namespace Gizmos {

Rotator::Rotator(): mOperation(Operation::None) {
  // Create all of the handles.
  mParent = nSpace.CreateMember();
  nSpace.AddComponent<Comp::Transform>(mParent);
  for (int i = 0; i < smHandleCount; ++i) {
    mHandles[i] = nSpace.CreateChildMember(mParent);
  }

  nSpace.AddComponent<Comp::Transform>(mX);
  auto& xMesh = nSpace.AddComponent<Comp::Mesh>(mX);
  xMesh.mMeshId = nTorusMeshId;
  xMesh.mMaterialId =
    ResId(smRotatorAssetName, smMaterialNames[(int)Operation::X]);

  Comp::Transform& yT = nSpace.AddComponent<Comp::Transform>(mY);
  Math::Quaternion rotation;
  rotation.AngleAxis(Math::nPi / 2.0f, {0.0f, 0.0f, 1.0f});
  yT.SetRotation(rotation);
  auto& yMesh = nSpace.AddComponent<Comp::Mesh>(mY);
  yMesh.mMeshId = nTorusMeshId;
  yMesh.mMaterialId =
    ResId(smRotatorAssetName, smMaterialNames[(int)Operation::Y]);

  Comp::Transform& zT = nSpace.AddComponent<Comp::Transform>(mZ);
  rotation.AngleAxis(-Math::nPi / 2.0f, {0.0f, 1.0f, 0.0f});
  zT.SetRotation(rotation);
  auto& zMesh = nSpace.AddComponent<Comp::Mesh>(mZ);
  zMesh.mMeshId = nTorusMeshId;
  zMesh.mMaterialId =
    ResId(smRotatorAssetName, smMaterialNames[(int)Operation::Z]);

  Comp::Transform& xyzT = nSpace.AddComponent<Comp::Transform>(mXyz);
  xyzT.SetUniformScale(0.8f);
  auto& xyzMesh = nSpace.AddComponent<Comp::Mesh>(mXyz);
  xyzMesh.mMeshId = nSphereMeshId;
  xyzMesh.mMaterialId =
    ResId(smRotatorAssetName, smMaterialNames[(int)Operation::Xyz]);
}

Rotator::~Rotator() {
  if (mParent != World::nInvalidMemberId) {
    nSpace.DeleteMember(mParent);
  }
}

Rotator::Rotator(Rotator&& other) {
  mParent = other.mParent;
  for (int i = 0; i < smHandleCount; ++i) {
    mHandles[i] = other.mHandles[i];
  }
  mOperation = other.mOperation;
  mMouseOffset = other.mMouseOffset;
  mRotationPlane = other.mRotationPlane;

  other.mParent = World::nInvalidMemberId;
}

void Rotator::SetNextOperation(
  const Vec3& translation, const Quat& referenceFrame) {
  // Find out the type of operation.
  const World::Object cameraObject = nCamera.GetObject();
  const auto& cameraComp = cameraObject.Get<Comp::Camera>();
  World::MemberId hoveredHandleId =
    Gfx::Renderer::HoveredMemberId(nSpace, cameraObject);
  // clang-format off
  if (hoveredHandleId == mX) { mOperation = Operation::X; }
  else if (hoveredHandleId == mY) { mOperation = Operation::Y; }
  else if (hoveredHandleId == mZ) { mOperation = Operation::Z; }
  else if (hoveredHandleId == mXyz) { mOperation = Operation::Xyz; }
  else { mOperation = Operation::None; return; }
  // clang-format on

  // Prepare mRotationPlane for the new operation.
  mRotationPlane.mPoint = translation;
  Vec3 xRotated = referenceFrame.Rotate({1.0f, 0.0f, 0.0f});
  Vec3 yRotated = referenceFrame.Rotate({0.0f, 1.0f, 0.0f});
  Vec3 zRotated = referenceFrame.Rotate({0.0f, 0.0f, 1.0f});
  switch (mOperation) {
  case Operation::X: mRotationPlane.Normal(xRotated); break;
  case Operation::Y: mRotationPlane.Normal(yRotated); break;
  case Operation::Z: mRotationPlane.Normal(zRotated); break;
  default: break;
  }

  // Prepare mMouseOffset for the new operation.
  Math::Ray mouseRay =
    cameraComp.NdcPositionToWorldRay(Input::NdcMousePosition(), cameraObject);
  bool singleAxisOperation = mOperation != Operation::Xyz;
  if (!singleAxisOperation) {
    return;
  }
  Math::RayPlane intersectionInfo =
    Math::Intersection(mouseRay, mRotationPlane);
  if (!intersectionInfo.mIntersecting) {
    return;
  }
  mMouseOffset = intersectionInfo.mIntersection - translation;
  if (Math::Near(mMouseOffset, {0.0f, 0.0f, 0.0f})) {
    mOperation = Operation::None;
  }
}

Quat Rotator::Run(
  const Quat& rotation, const Vec3& translation, const Quat& referenceFrame) {
  SetParentTransformation(mParent, translation, referenceFrame);

  // Handle the transitions between all operation types.
  if (!Input::MouseDown(Input::Mouse::Left) && mOperation != Operation::None) {
    auto& meshComp = nSpace.Get<Comp::Mesh>(mHandles[(int)mOperation]);
    meshComp.mMaterialId =
      ResId(smRotatorAssetName, smMaterialNames[(int)mOperation]);
    mOperation = Operation::None;
    return rotation;
  }
  if (Input::MousePressed(Input::Mouse::Left)) {
    SetNextOperation(translation, referenceFrame);
    if (mOperation == Operation::None) {
      return rotation;
    }
    Editor::nSuppressObjectPicking |= true;
    auto& meshComp = nSpace.Get<Comp::Mesh>(mHandles[(int)mOperation]);
    if (mOperation == Operation::Xyz) {
      meshComp.mMaterialId =
        ResId(smRotatorAssetName, "TransparentActiveColor");
    }
    else {
      meshComp.mMaterialId = ResId(smRotatorAssetName, "ActiveColor");
    }
    return rotation;
  }

  // Perform the active operation.
  const World::Object cameraObject = nCamera.GetObject();
  const auto& cameraComp = cameraObject.Get<Comp::Camera>();
  if (mOperation < Operation::Xyz) {
    // We get two rays on the rotation plane and find the angle between them to
    // determine the change in rotation.
    Math::Ray mouseRay =
      cameraComp.NdcPositionToWorldRay(Input::NdcMousePosition(), cameraObject);
    Math::RayPlane intersectionInfo =
      Math::Intersection(mouseRay, mRotationPlane);
    if (!intersectionInfo.mIntersecting) {
      return rotation;
    }
    Vec3 newMouseOffset = intersectionInfo.mIntersection - translation;
    if (Math::Near(Math::MagnitudeSq(newMouseOffset), 0.0f)) {
      return rotation;
    }
    Vec3 normOld = Math::Normalize(mMouseOffset);
    Vec3 normNew = Math::Normalize(newMouseOffset);
    // We need to clamp to avoid nan values from the cosine function.
    float dot = Math::Clamp(-1.0f, 1.0f, Math::Dot(normOld, normNew));
    float angle = acosf(dot);
    if (nSnapping) {
      angle = Math::RoundToNearest(angle, nScaleSnapInterval);
    }
    Vec3 positiveDirection = Math::Cross(mRotationPlane.Normal(), normOld);
    if (Math::Dot(normNew, positiveDirection) < 0.0f) {
      angle *= -1.0f;
    }
    Math::Quaternion delta;
    delta.AngleAxis(angle, mRotationPlane.Normal());
    mMouseOffset = delta.Rotate(mMouseOffset);
    return delta * rotation;
  }
  else if (mOperation == Operation::Xyz) {
    const float pixelsPerRadian = 500.0f;
    Vec2 radians = (Input::MouseMotion() / pixelsPerRadian) * Math::nPi;
    Math::Quaternion horizontalRotation, verticalRotation;
    horizontalRotation.AngleAxis(radians[0], cameraComp.WorldUp(cameraObject));
    verticalRotation.AngleAxis(radians[1], cameraComp.WorldRight(cameraObject));
    return horizontalRotation * verticalRotation * rotation;
  }
  return rotation;
}

void Rotator::Init() {
  // Create all of the handles.
  Rsl::Asset& rotatorAsset = Rsl::AddAsset(smRotatorAssetName);
  for (int i = 0; i < smHandleCount; ++i) {
    Gfx::Material& material =
      rotatorAsset.InitRes<Gfx::Material>(smMaterialNames[i], nColorShaderId);
    material.mUniforms.Add<Vec4>("uColor") = smHandleColors[i];
  }
  auto& activeMaterial =
    rotatorAsset.InitRes<Gfx::Material>("ActiveColor", nColorShaderId);
  activeMaterial.mUniforms.Add<Vec4>("uColor") = {1, 1, 1, 1};
  auto& transparentActiveMaterial = rotatorAsset.InitRes<Gfx::Material>(
    "TransparentActiveColor", nColorShaderId);
  transparentActiveMaterial.mUniforms.Add<Vec4>("uColor") = {1, 1, 1, 0.8f};
  rotatorAsset.Finalize();
}

void Rotator::Purge() {
  Rsl::RemAsset(smRotatorAssetName);
}

} // namespace Gizmos
} // namespace Editor

#include "editor/gizmos/Scalor.h"
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

const Vec4 Scalor::smHandleColors[] = {
  {0.7f, 0.0f, 0.0f, 1.0f},
  {0.0f, 0.7f, 0.0f, 1.0f},
  {0.0f, 0.0f, 0.7f, 1.0f},
  {0.7f, 0.7f, 0.0f, 1.0f},
  {0.7f, 0.0f, 0.7f, 1.0f},
  {0.0f, 0.7f, 0.7f, 1.0f},
  {0.7f, 0.7f, 0.7f, 1.0f}};

Scalor::Scalor(): mOperation(Operation::None) {
  mParent = nSpace.CreateMember();
  nSpace.AddComponent<Comp::Transform>(mParent);
  for (int i = 0; i < smHandleCount; ++i) {
    mHandles[i] = nSpace.CreateChildMember(mParent);
  }

  Comp::Transform& xT = nSpace.AddComponent<Comp::Transform>(mX);
  xT.SetTranslation({0.5f, 0.0f, 0.0f});
  auto& xMesh = nSpace.AddComponent<Comp::Mesh>(mX);
  xMesh.mMeshId = nScaleMeshId;
  xMesh.mMaterialId =
    ResId(smScalorAssetName, smMaterialNames[(int)Operation::X]);

  Comp::Transform& yT = nSpace.AddComponent<Comp::Transform>(mY);
  yT.SetTranslation({0.0f, 0.5f, 0.0f});
  yT.SetRotation(Quat::AngleAxis(Math::nPiO2, {0.0f, 0.0f, 1.0f}));
  auto& yMesh = nSpace.AddComponent<Comp::Mesh>(mY);
  yMesh.mMeshId = nScaleMeshId;
  yMesh.mMaterialId =
    ResId(smScalorAssetName, smMaterialNames[(int)Operation::Y]);

  Comp::Transform& zT = nSpace.AddComponent<Comp::Transform>(mZ);
  zT.SetTranslation({0.0f, 0.0f, 0.5f});
  zT.SetRotation(Quat::AngleAxis(-Math::nPiO2, {0.0f, 1.0f, 0.0f}));
  auto& zMesh = nSpace.AddComponent<Comp::Mesh>(mZ);
  zMesh.mMeshId = nScaleMeshId;
  zMesh.mMaterialId =
    ResId(smScalorAssetName, smMaterialNames[(int)Operation::Z]);

  Comp::Transform& xyT = nSpace.AddComponent<Comp::Transform>(mXy);
  xyT.SetTranslation({0.5f, 0.5f, 0.0f});
  xyT.SetScale({0.15f, 0.15f, 0.01f});
  auto& xyMesh = nSpace.AddComponent<Comp::Mesh>(mXy);
  xyMesh.mMeshId = nCubeMeshId;
  xyMesh.mMaterialId =
    ResId(smScalorAssetName, smMaterialNames[(int)Operation::Xy]);

  Comp::Transform& xzT = nSpace.AddComponent<Comp::Transform>(mXz);
  xzT.SetTranslation({0.5f, 0.0f, 0.5f});
  xzT.SetScale({0.15f, 0.01f, 0.15f});
  auto& xzMesh = nSpace.AddComponent<Comp::Mesh>(mXz);
  xzMesh.mMeshId = nCubeMeshId;
  xzMesh.mMaterialId =
    ResId(smScalorAssetName, smMaterialNames[(int)Operation::Xz]);

  Comp::Transform& yzT = nSpace.AddComponent<Comp::Transform>(mYz);
  yzT.SetTranslation({0.0f, 0.5f, 0.5f});
  yzT.SetScale({0.01f, 0.15f, 0.15f});
  auto& yzMesh = nSpace.AddComponent<Comp::Mesh>(mYz);
  yzMesh.mMeshId = nCubeMeshId;
  yzMesh.mMaterialId =
    ResId(smScalorAssetName, smMaterialNames[(int)Operation::Yz]);

  Comp::Transform& xyzT = nSpace.AddComponent<Comp::Transform>(mXyz);
  xyzT.SetUniformScale(1.2f);
  auto& xyzMesh = nSpace.AddComponent<Comp::Mesh>(mXyz);
  xyzMesh.mMeshId = nTorusMeshId;
  xyzMesh.mMaterialId =
    ResId(smScalorAssetName, smMaterialNames[(int)Operation::Xyz]);
}

Scalor::~Scalor() {
  if (mParent != World::nInvalidMemberId) {
    nSpace.DeleteMember(mParent);
  }
}

Scalor::Scalor(Scalor&& other) {
  mParent = other.mParent;
  for (int i = 0; i < smHandleCount; ++i) {
    mHandles[i] = other.mHandles[i];
  }
  mOperation = other.mOperation;
  mMousePosition = other.mMousePosition;
  mScaleRay = other.mScaleRay;
  mScalePlane = other.mScalePlane;
  mUniformScaleDirection = other.mUniformScaleDirection;
  mNormalizedStartScale = other.mNormalizedStartScale;

  other.mParent = World::nInvalidMemberId;
}

void Scalor::SetNextOperation(
  const Vec3& scale, const Vec3& translation, const Quat& referenceFrame) {
  // Find the next operation type.
  const World::Object cameraObject = nCamera.GetObject();
  const auto& cameraComp = cameraObject.Get<Comp::Camera>();
  World::MemberId hoveredHandleId =
    Gfx::Renderer::HoveredMemberId(nSpace, cameraObject);
  // clang-format off
  if (hoveredHandleId == mX) { mOperation = Operation::X; }
  else if (hoveredHandleId == mY) { mOperation = Operation::Y; }
  else if (hoveredHandleId == mZ) { mOperation = Operation::Z; }
  else if (hoveredHandleId == mXy) { mOperation = Operation::Xy; }
  else if (hoveredHandleId == mXz) { mOperation = Operation::Xz; }
  else if (hoveredHandleId == mYz) { mOperation = Operation::Yz; }
  else if (hoveredHandleId == mXyz) { mOperation = Operation::Xyz; }
  else { mOperation = Operation::None; return; }
  // clang-format on

  // Set values to be used for the selected operation during following frames.
  mScaleRay.mStart = translation;
  mScalePlane.mPoint = translation;
  Vec3 xRotated = referenceFrame.Rotate({1.0f, 0.0f, 0.0f});
  Vec3 yRotated = referenceFrame.Rotate({0.0f, 1.0f, 0.0f});
  Vec3 zRotated = referenceFrame.Rotate({0.0f, 0.0f, 1.0f});
  Vec3 forward = cameraComp.WorldForward(cameraObject);
  switch (mOperation) {
  case Operation::X: mScaleRay.Direction(xRotated); break;
  case Operation::Y: mScaleRay.Direction(yRotated); break;
  case Operation::Z: mScaleRay.Direction(zRotated); break;
  case Operation::Xy: mScalePlane.Normal(zRotated); break;
  case Operation::Xz: mScalePlane.Normal(yRotated); break;
  case Operation::Yz: mScalePlane.Normal(xRotated); break;
  case Operation::Xyz: mScalePlane.Normal(forward); break;
  default: break;
  }
  Math::Ray mouseRay =
    cameraComp.NdcPositionToWorldRay(Input::NdcMousePosition(), cameraObject);
  if (mOperation < Operation::Xy) {
    if (!mScaleRay.HasClosestTo(mouseRay)) {
      mOperation = Operation::None;
      return;
    }
    mMousePosition = mScaleRay.ClosestPointTo(mouseRay);
  }
  else if (mOperation < Operation::Xyz) {
    Math::RayPlane intersectionInfo = Math::Intersection(mouseRay, mScalePlane);
    if (!intersectionInfo.mIntersecting) {
      mOperation = Operation::None;
      return;
    }
    mMousePosition = intersectionInfo.mIntersection;
  }
  else {
    Math::RayPlane intersectionInfo = Math::Intersection(mouseRay, mScalePlane);
    mMousePosition = intersectionInfo.mIntersection;
    mUniformScaleDirection = Math::Normalize(mMousePosition - translation);
    mNormalizedStartScale = Math::Normalize(scale);
  }
}

Vec3 Scalor::Run(
  const Vec3& scale, const Vec3& translation, const Quat& referenceFrame) {
  // Set the transformations for the handles that change every frame.
  SetParentTransformation(mParent, translation, referenceFrame);
  const World::Object cameraObject = nCamera.GetObject();
  const auto& cameraComp = cameraObject.Get<Comp::Camera>();
  const Vec3 cameraTranslation = cameraComp.WorldTranslation(cameraObject);
  Vec3 torusDirection = cameraTranslation - translation;
  Math::Quaternion xyzRotation =
    Quat::FromTo({1.0f, 0.0f, 0.0f}, torusDirection);
  xyzRotation = referenceFrame.Conjugate() * xyzRotation;
  Comp::Transform& xyzTransform = nSpace.GetComponent<Comp::Transform>(mXyz);
  xyzTransform.SetRotation(xyzRotation);

  // Handle any switching between operations.
  if (!Input::MouseDown(Input::Mouse::Left) && mOperation != Operation::None) {
    auto& meshComp = nSpace.Get<Comp::Mesh>(mHandles[(int)mOperation]);
    meshComp.mMaterialId =
      ResId(smScalorAssetName, smMaterialNames[(int)mOperation]);
    mOperation = Operation::None;
    return scale;
  }
  if (Input::MousePressed(Input::Mouse::Left)) {
    SetNextOperation(scale, translation, referenceFrame);
    if (mOperation == Operation::None) {
      return scale;
    }
    Editor::nSuppressObjectPicking |= true;
    auto& meshComp = nSpace.Get<Comp::Mesh>(mHandles[(int)mOperation]);
    meshComp.mMaterialId = ResId(smScalorAssetName, "ActiveColor");
    return scale;
  }

  // Perform the desired operation.
  Vec3 axes;
  switch (mOperation) {
  case Operation::X: axes = {1.0, 0.0f, 0.0f}; break;
  case Operation::Y: axes = {0.0f, 1.0f, 0.0f}; break;
  case Operation::Z: axes = {0.0f, 0.0f, 1.0f}; break;
  case Operation::Xy: axes = {1.0f, 1.0f, 0.0f}; break;
  case Operation::Xz: axes = {1.0f, 0.0f, 1.0f}; break;
  case Operation::Yz: axes = {0.0f, 1.0f, 1.0f}; break;
  case Operation::Xyz: axes = {1.0f, 1.0f, 1.0f}; break;
  default: break;
  }
  Math::Ray mouseRay =
    cameraComp.NdcPositionToWorldRay(Input::NdcMousePosition(), cameraObject);
  if (mOperation < Operation::Xy) {
    if (!mScaleRay.HasClosestTo(mouseRay)) {
      return scale;
    }
    Vec3 newMousePosition = mScaleRay.ClosestPointTo(mouseRay);
    Vec3 mouseDelta = newMousePosition - mMousePosition;
    float deltaMagnitude = Math::Dot(mouseDelta, mScaleRay.Direction());
    if (!nSnapping) {
      mMousePosition = newMousePosition;
      return scale + axes * deltaMagnitude;
    }
    deltaMagnitude = Math::RoundToNearest(deltaMagnitude, nScaleSnapInterval);
    mMousePosition += mScaleRay.Direction() * deltaMagnitude;
    return scale + axes * deltaMagnitude;
  }
  else if (mOperation < Operation::Xyz) {
    Math::RayPlane intersectionInfo = Math::Intersection(mouseRay, mScalePlane);
    if (!intersectionInfo.mIntersecting) {
      return scale;
    }
    Vec3 delta = intersectionInfo.mIntersection - mMousePosition;
    delta = referenceFrame.Conjugate().Rotate(delta);
    delta = Math::ComponentwiseProduct(axes, delta);
    if (nSnapping) {
      delta = Math::ScaleComponentsToInterval(delta, nScaleSnapInterval);
    }
    mMousePosition += referenceFrame.Rotate(delta);
    return scale + delta;
  }
  else if (mOperation == Operation::Xyz) {
    Math::RayPlane intersectionInfo = Math::Intersection(mouseRay, mScalePlane);
    if (!intersectionInfo.mIntersecting) {
      return scale;
    }
    Vec3 mouseDelta = intersectionInfo.mIntersection - mMousePosition;
    float amount = Math::Dot(mouseDelta, mUniformScaleDirection);
    mMousePosition += amount * mUniformScaleDirection;
    return scale + mNormalizedStartScale * amount;
  }

  // Guarantee that the handles are always facing the user.
  AxisHandleGroup groups[3] = {
    {referenceFrame.XBasisAxis(), mX, {mXy, mXz}},
    {referenceFrame.YBasisAxis(), mY, {mXy, mYz}},
    {referenceFrame.ZBasisAxis(), mZ, {mXz, mYz}}};
  OrientHandlesTowardsCamera(groups, translation, cameraTranslation);
  return scale;
}

void Scalor::Init() {
  Rsl::Asset& scalorAsset = Rsl::AddAsset(smScalorAssetName);
  for (int i = 0; i < smHandleCount; ++i) {
    Gfx::Material& material =
      scalorAsset.InitRes<Gfx::Material>(smMaterialNames[i], nColorShaderId);
    material.Add<Vec4>("uColor") = smHandleColors[i];
  }
  scalorAsset.InitRes<Gfx::Material>("ActiveColor", nColorShaderId)
    .Add<Vec4>("uColor") = {1, 1, 1, 1};
}

void Scalor::Purge() {
  Rsl::RemAsset(smScalorAssetName);
}

} // namespace Gizmos
} // namespace Editor

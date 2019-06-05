#pragma once

#include "BsCorePrerequisites.h"
// #include "Resources/BsIResourceListener.h"
// #include "Math/BsBounds.h"
// #include "Math/BsAABox.h"
// #include "Scene/BsSceneActor.h"
#include "Material/BsShader.h"
#include "RenderAPI/BsGpuBuffer.h"
#include "RenderAPI/BsVertexDeclaration.h"
#include "RenderAPI/BsVertexBuffer.h"
#include "Mesh/BsMesh.h"
#include "Math/BsBounds.h"

namespace bs::ecs {

enum class RenderableAnimType
{
	None,
	Skinned,
	Morph,
	SkinnedMorph,
	Count // Keep at end
};

struct EBoneMatrix {
  SPtr<bs::GpuBufferProperties> mBoneMatrixBuffer;
};

struct EMorphable {
  SPtr<bs::VertexBufferProperties> mMorphShapeBuffer;
  SPtr<bs::VertexDeclarationProperties> mMorphVertexDeclaration;
};

struct EMaterial {
	Shader mShader;
	SPtr<MaterialParams> mParams;
	Vector<SPtr<Technique>> mTechniques;
	ShaderVariation mVariation;
};

struct ERenderable {
	MeshProperties mMesh;
	Vector<EMaterial> mMaterials;
	UINT64 mLayer = 1;
	AABox mOverrideBounds;
	bool mUseOverrideBounds = false;
	float mCullDistanceFactor = 1.0f;
	Matrix4 mTfrmMatrix = BsIdentity;
	Matrix4 mTfrmMatrixNoScale = BsIdentity;
	RenderableAnimType mAnimType = RenderableAnimType::None;
};

}  // namespace bs::ecs
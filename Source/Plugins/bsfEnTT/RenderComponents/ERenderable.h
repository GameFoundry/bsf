#pragma once

#include "BsCorePrerequisites.h"
// #include "Resources/BsIResourceListener.h"
// #include "Math/BsBounds.h"
// #include "Math/BsAABox.h"
// #include "Scene/BsSceneActor.h"
// #include "Material/BsShader.h"
// #include "Material/BsMaterial.h"
#include "Material/BsMaterialParams.h"
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

struct BS_CORE_EXPORT ERenderable {
	HMesh mMesh;
	Vector<HMaterial> mMaterials;
	UINT64 mLayer{1};
	AABox mOverrideBounds;
	bool mUseOverrideBounds{false};
	float mCullDistanceFactor{1.0f};
	Matrix4 mTfrmMatrix{BsIdentity};
	Matrix4 mTfrmMatrixNoScale{BsIdentity};
	RenderableAnimType mAnimType{RenderableAnimType::None};

	/**	Gets world bounds of the mesh rendered by this object. */
	Bounds getBounds() const;

};

}  // namespace bs::ecs

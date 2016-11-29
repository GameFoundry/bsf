//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParam.h"
#include "BsRenderableElement.h"
#include "BsRendererMaterial.h"
#include "BsParamBlocks.h"
#include "BsRendererObject.h"

namespace bs
{
	struct RendererAnimationData;

	/** @addtogroup RenderBeast
	 *  @{
	 */

	BS_PARAM_BLOCK_BEGIN(PerFrameParamBuffer)
		BS_PARAM_BLOCK_ENTRY(float, gTime)
	BS_PARAM_BLOCK_END

	BS_PARAM_BLOCK_BEGIN(PerObjectParamBuffer)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldViewProj)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatWorld)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatInvWorld)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldNoScale)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatInvWorldNoScale)
		BS_PARAM_BLOCK_ENTRY(float, gWorldDeterminantSign)
	BS_PARAM_BLOCK_END

	/**	Data bound to the shader when rendering a specific renderable object. */
	struct RenderableShaderData
	{
		Matrix4 worldTransform;
		Matrix4 invWorldTransform;
		Matrix4 worldNoScaleTransform;
		Matrix4 invWorldNoScaleTransform;
		float worldDeterminantSign;
	};

	/** Manages initialization and rendering of individual renderable object, represented as RenderableElement%s. */
	class BS_BSRND_EXPORT ObjectRenderer
	{
	public:
		ObjectRenderer();

		/** Initializes the specified renderable element, making it ready to be used. */
		void initElement(BeastRenderableElement& element);

		/** Updates global per frame parameter buffers with new values. To be called at the start of every frame. */
		void setParamFrameParams(float time);

		/**
		 * Updates object specific parameter buffers with new values. To be called whenever object specific values change.
		 */
		void setPerObjectParams(const BeastRenderableElement& element, const RenderableShaderData& data,
			const Matrix4& wvpMatrix, const SPtr<GpuBufferCore>& boneMatrices = nullptr);

	protected:
		PerFrameParamBuffer mPerFrameParams;
		PerObjectParamBuffer mPerObjectParams;
	};

	/** Basic shader that is used when no other is available. */
	class DefaultMaterial : public RendererMaterial<DefaultMaterial> { RMAT_DEF("Default.bsl"); };

	/** @} */
}
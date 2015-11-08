#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsRenderableHandler.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParam.h"
#include "BsRenderableElement.h"
#include "BsRenderBeast.h"
#include "BsParamBlocks.h"

namespace BansheeEngine
{
	BS_PARAM_BLOCK_BEGIN(PerFrameParamBuffer)
		BS_PARAM_BLOCK_ENTRY(float, gTime)
	BS_PARAM_BLOCK_END

	BS_PARAM_BLOCK_BEGIN(PerCameraParamBuffer)
		BS_PARAM_BLOCK_ENTRY(Vector3, gViewDir)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatView)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatProj)
	BS_PARAM_BLOCK_END

	BS_PARAM_BLOCK_BEGIN(PerObjectParamBuffer)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldViewProj)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatWorld)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatInvWorld)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldNoScale)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatInvWorldNoScale)
		BS_PARAM_BLOCK_ENTRY(float, gWorldDeterminantSign)
	BS_PARAM_BLOCK_END

	/**
	 * @brief	Renderable handler that manages initializing, updating and 
	 * 			rendering of static renderable objects.
	 */
	class BS_BSRND_EXPORT StaticRenderableHandler : public RenderableHandler
	{
	public:
		/**
		 * @brief	Contains lit tex renderable data unique for each object.
		 */
		struct PerObjectData
		{
			GpuParamMat4Core wvpParam;
			GpuParamMat4Core wParam;
			GpuParamMat4Core iwParam;
			GpuParamMat4Core wNoScaleParam;
			GpuParamMat4Core iwNoScaleParam;
			GpuParamFloatCore worldDeterminantSignParam;

			Vector<RenderableElement::BufferBindInfo> perObjectBuffers;
		};

		StaticRenderableHandler();

		/**
		 * @copydoc	RenderableController::initializeRenderElem
		 */
		void initializeRenderElem(RenderableElement& element) override;

		/**
		 * @copydoc	RenderableController::bindPerObjectBuffers
		 */
		void bindPerObjectBuffers(const RenderableElement& element) override;

		/**
		 * @brief	Updates global per frame parameter buffers with new values. 
		 *			To be called at the start of every frame.
		 */
		void updatePerFrameBuffers(float time);

		/**
		 * @brief	Updates global per frame parameter buffers with new values. 
		 *			To be called at the start of rendering for every camera.
		 */
		void updatePerCameraBuffers(const Matrix4& vpMatrix, const Matrix4& vMatrix, const Matrix4& pMatrix, const Vector3& viewDir);

		/**
		 * @brief	Updates object specific parameter buffers with new values.
		 *			To be called whenever object specific values change.
		 */
		void updatePerObjectBuffers(RenderableElement& element, const RenderableShaderData& data, const Matrix4& wvpMatrix);

	protected:
		/**
		 * @brief	Creates a new default shader used for lit textured renderables.
		 *			It is used for matching custom shaders and determining if they
		 *			comply with lit textured renderable requirements.
		 */
		SPtr<ShaderCore> createDefaultShader();

		SPtr<ShaderCore> defaultShader;

		GpuParamBlockDesc staticParamBlockDesc;
		GpuParamBlockDesc perFrameParamBlockDesc;
		GpuParamBlockDesc perCameraParamBlockDesc;
		GpuParamBlockDesc perObjectParamBlockDesc;

		GpuParamDataDesc timeParamDesc;
		GpuParamDataDesc vpParamDesc;
		GpuParamDataDesc vParamDesc;
		GpuParamDataDesc pParamDesc;
		GpuParamDataDesc viewDirParamDesc;

		GpuParamDataDesc wvpParamDesc;
		GpuParamDataDesc wParamDesc;
		GpuParamDataDesc iwParamDesc;
		GpuParamDataDesc wNoScaleParamDesc;
		GpuParamDataDesc iwNoScaleParamDesc;
		GpuParamDataDesc worldDeterminantSignParamDesc;

		SPtr<GpuParamBlockBufferCore> perFrameParamBuffer;
		SPtr<GpuParamBlockBufferCore> perCameraParamBuffer;
		SPtr<GpuParamBlockBufferCore> perObjectParamBuffer;

		SPtr<GpuParamsCore> perFrameParams;
		SPtr<GpuParamsCore> perCameraParams;

		GpuParamVec3Core viewDirParam;
		GpuParamMat4Core viewProjMatParam;
		GpuParamMat4Core viewMatParam;
		GpuParamMat4Core projMatParam;
		GpuParamFloatCore timeParam;
	};
}
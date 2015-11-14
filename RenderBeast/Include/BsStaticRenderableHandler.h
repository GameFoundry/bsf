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
		PerFrameParamBuffer mPerFrameParams;
		PerCameraParamBuffer mPerCameraParams;
		PerObjectParamBuffer mPerObjectParams;
	};
}
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsRenderableController.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParam.h"
#include "BsRenderableElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Renderer handler that manages initializing, updating
	 *			and rendering of renderable objects with a single texture
	 *			and a single light.
	 *
	 * @note	This class is DEBUG ONLY. Until a better renderer is complete.
	 */
	class BS_BSRND_EXPORT LitTexRenderableController : public RenderableController
	{
	public:
		/**
		 * @brief	Contains lit tex renderable data unique for each object.
		 */
		struct PerObjectData
		{
			SPtr<GpuParamBlockBufferCore> perObjectParamBuffer;

			GpuParamMat4Core wvpParam;

			Vector<RenderableElement::BufferBindInfo> perObjectBuffers;
		};

		LitTexRenderableController();

		/**
		 * @copydoc	RenderableHandler::initializeRenderElem
		 */
		void initializeRenderElem(RenderableElement& element) override;

		/**
		 * @copydoc	RenderableHandler::bindPerObjectBuffers
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
		void updatePerCameraBuffers(const Vector3& viewDir);

		/**
		 * @brief	Updates object specific parameter buffers with new values.
		 *			To be called whenever object specific values change.
		 */
		void updatePerObjectBuffers(RenderableElement& element, const Matrix4& wvpMatrix);

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
		GpuParamDataDesc lightDirParamDesc;
		GpuParamDataDesc wvpParamDesc;
		GpuParamDataDesc viewDirParamDesc;

		SPtr<GpuParamBlockBufferCore> staticParamBuffer;
		SPtr<GpuParamBlockBufferCore> perFrameParamBuffer;
		SPtr<GpuParamBlockBufferCore> perCameraParamBuffer;

		SPtr<GpuParamsCore> staticParams;
		SPtr<GpuParamsCore> perFrameParams;
		SPtr<GpuParamsCore> perCameraParams;

		GpuParamVec4Core lightDirParam;
		GpuParamVec3Core viewDirParam;
		GpuParamFloatCore timeParam;
	};
}
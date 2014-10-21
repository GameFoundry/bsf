#pragma once

#include "BsBansheeRendererPrerequisites.h"
#include "BsRenderableController.h"
#include "BsMaterialProxy.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParam.h"

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
			GpuParamBlockBufferPtr perObjectParamBuffer;

			bool hasWVPParam = false;
			GpuParamMat4 wvpParam;

			Vector<MaterialProxy::BufferBindInfo> perObjectBuffers;
		};

		LitTexRenderableController();

		/**
		 * @copydoc	RenderableHandler::initializeRenderElem
		 */
		void initializeRenderElem(RenderableElement* element);

		/**
		 * @copydoc	RenderableHandler::bindPerObjectBuffers
		 */
		void bindPerObjectBuffers(const RenderableElement* element);

		/**
		 * @brief	Updates global parameter buffers with new values. 
		 *			To be called whenever global values change.
		 */
		void updateGlobalBuffers(float time);

		/**
		 * @brief	Updates object specific parameter buffers with new values.
		 *			To be called whenever object specific values change.
		 */
		void updatePerObjectBuffers(RenderableElement* element, const Matrix4& wvpMatrix);

	protected:
		/**
		 * @brief	Creates a new default shader used for lit textured renderables.
		 *			It is used for matching custom shaders and determining if they
		 *			comply with lit textured renderable requirements.
		 */
		ShaderPtr createDefaultShader();

		ShaderPtr defaultShader;

		GpuParamBlockDesc staticParamBlockDesc;
		GpuParamBlockDesc perFrameParamBlockDesc;
		GpuParamBlockDesc perObjectParamBlockDesc;

		GpuParamDataDesc timeParamDesc;
		GpuParamDataDesc lightDirParamDesc;
		GpuParamDataDesc wvpParamDesc;

		GpuParamBlockBufferPtr staticParamBuffer;
		GpuParamBlockBufferPtr perFrameParamBuffer;

		GpuParamsPtr staticParams;
		GpuParamsPtr perFrameParams;

		GpuParamVec4 lightDirParam;
		GpuParamFloat timeParam;
	};
}
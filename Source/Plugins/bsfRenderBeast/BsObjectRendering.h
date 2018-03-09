//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "RenderAPI/BsGpuParamDesc.h"
#include "RenderAPI/BsGpuParam.h"
#include "Renderer/BsRenderableElement.h"
#include "Renderer/BsRendererMaterial.h"
#include "Renderer/BsParamBlocks.h"
#include "BsRendererObject.h"

namespace bs 
{ 
	struct EvaluatedAnimationData;

	namespace ct
	{
	/** @addtogroup RenderBeast
	 *  @{
	 */

	BS_PARAM_BLOCK_BEGIN(PerFrameParamDef)
		BS_PARAM_BLOCK_ENTRY(float, gTime)
	BS_PARAM_BLOCK_END

	extern PerFrameParamDef gPerFrameParamDef;

	/** Manages initialization and rendering of individual renderable object, represented as RenderableElement%s. */
	class ObjectRenderer
	{
	public:
		ObjectRenderer();

		/** Initializes the specified renderable element, making it ready to be used. */
		void initElement(RendererObject& owner, BeastRenderableElement& element);

		/** Updates global per frame parameter buffers with new values. To be called at the start of every frame. */
		void setParamFrameParams(float time);

	protected:
		SPtr<GpuParamBlockBuffer> mPerFrameParamBuffer;
	};

	/** Basic shader that is used when no other is available. */
	class DefaultMaterial : public RendererMaterial<DefaultMaterial> { RMAT_DEF("Default.bsl"); };

	/** @} */
}}
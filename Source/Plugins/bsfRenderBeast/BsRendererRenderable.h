//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "Renderer/BsRenderElement.h"
#include "Renderer/BsRenderable.h"
#include "Renderer/BsParamBlocks.h"
#include "Material/BsMaterialParam.h"
#include "RenderAPI/BsGpuPipelineParamInfo.h"
#include "BsRendererReflectionProbe.h"

namespace bs { namespace ct
{
	/** @addtogroup RenderBeast
	 *  @{
	 */

	BS_PARAM_BLOCK_BEGIN(PerObjectParamDef)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatWorld)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatInvWorld)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldNoScale)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatInvWorldNoScale)
		BS_PARAM_BLOCK_ENTRY(float, gWorldDeterminantSign)
	BS_PARAM_BLOCK_END

	extern PerObjectParamDef gPerObjectParamDef;

	BS_PARAM_BLOCK_BEGIN(PerCallParamDef)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldViewProj)
	BS_PARAM_BLOCK_END

	extern PerCallParamDef gPerCallParamDef;

	struct MaterialSamplerOverrides;

	/**
	 * Contains information required for rendering a single Renderable sub-mesh, representing a generic static or animated
	 * 3D model.
	 */
	class RenderableElement : public RenderElement
	{
	public:
		/**
		 * Optional overrides for material sampler states. Used when renderer wants to override certain sampling properties
		 * on a global scale (for example filtering most commonly).
		 */
		MaterialSamplerOverrides* samplerOverrides;

		/** Identifier of the animation running on the renderable's mesh. -1 if no animation. */
		UINT64 animationId;

		/** Type of animation applied to this element, if any. */
		RenderableAnimType animType;

		/** Binding indices representing where should the per-camera param block buffer be bound to. */
		GpuParamBinding perCameraBindings[GPT_COUNT];

		/** Binding indices representing where should lights param block buffer be bound to. */
		GpuParamBinding gridParamsBindings[GPT_COUNT];

		/** 
		 * Parameter to which to bind a buffer containing light grid offsets and size, per grid cell. Used for forward
		 * rendering. 
		 */
		GpuParamBuffer gridLightOffsetsAndSizeParam;

		/** Parameter to which to bind a buffer containing all light indices, as mapped by grid offsets & size. */
		GpuParamBuffer gridLightIndicesParam;

		/** Parameter to which to bind light buffer used for forward rendering. */
		GpuParamBuffer lightsBufferParam;

		/** 
		 * Parameter to which to bind a buffer containing reflection probe grid offsets and size, per grid cell. Used for
		 * forward rendering. 
		 */
		GpuParamBuffer gridProbeOffsetsAndSizeParam;

		/** Collection of parameters used for image based lighting. */
		ImageBasedLightingParams imageBasedParams;
		
		/** 
		 * Binding for a parameter block containing a list of lights influencing this object. Only used when standard
		 * (non-clustered) forward rendering is used. 
		 */
		GpuParamBinding lightsParamBlockBinding;

		/** 
		 * Binding for a parameter block that contains the number of lights and reflection probes in the light/refl. probe 
		 * parameter blocks. Only used when standard (non-clustered) forward rendering is used.
		 */
		GpuParamBinding lightAndReflProbeParamsParamBlockBinding;

		/** GPU buffer containing element's bone matrices, if it requires any. */
		SPtr<GpuBuffer> boneMatrixBuffer;

		/** Vertex buffer containing element's morph shape vertices, if it has any. */
		SPtr<VertexBuffer> morphShapeBuffer;

		/** Vertex declaration used for rendering meshes containing morph shape information. */
		SPtr<VertexDeclaration> morphVertexDeclaration;

		/** Version of the morph shape vertices in the buffer. */
		mutable UINT32 morphShapeVersion;
	};

	 /** Contains information about a Renderable, used by the Renderer. */
	struct RendererRenderable
	{
		RendererRenderable();

		/** Updates the per-object GPU buffer according to the currently set properties. */
		void updatePerObjectBuffer();

		/** 
		 * Updates the per-call GPU buffer according to the provided parameters. 
		 * 
		 * @param[in]	viewProj	Combined view-projection matrix of the current camera.
		 * @param[in]	flush		True if the buffer contents should be immediately flushed to the GPU.
		 */
		void updatePerCallBuffer(const Matrix4& viewProj, bool flush = true);

		Renderable* renderable;
		Vector<RenderableElement> elements;

		SPtr<GpuParamBlockBuffer> perObjectParamBuffer;
		SPtr<GpuParamBlockBuffer> perCallParamBuffer;
	};

	/** @} */
}}

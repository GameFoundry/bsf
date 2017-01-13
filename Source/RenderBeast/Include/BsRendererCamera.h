//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsPostProcessing.h"
#include "BsObjectRendering.h"
#include "BsRenderQueue.h"
#include "BsRendererObject.h"
#include "BsBounds.h"
#include "BsConvexVolume.h"

namespace bs { namespace ct
{
	/** @addtogroup RenderBeast
	 *  @{
	 */

	BS_PARAM_BLOCK_BEGIN(PerCameraParamDef)
		BS_PARAM_BLOCK_ENTRY(Vector3, gViewDir)
		BS_PARAM_BLOCK_ENTRY(Vector3, gViewOrigin)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatView)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatProj)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatInvProj)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatInvViewProj)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatScreenToWorld)
		BS_PARAM_BLOCK_ENTRY(Vector2, gDeviceZToWorldZ)
		BS_PARAM_BLOCK_ENTRY(Vector4, gClipToUVScaleOffset)
	BS_PARAM_BLOCK_END

	extern PerCameraParamDef gPerCameraParamDef;

	/** Shader that renders a skybox using a cubemap. */
	class SkyboxMat : public RendererMaterial<SkyboxMat>
	{
		RMAT_DEF("Skybox.bsl");

	public:
		SkyboxMat();

		/** Binds the material for rendering and sets up any global parameters. */
		void bind(const SPtr<GpuParamBlockBuffer>& perCamera);

		/** Updates the skybox texture used by the material. */
		void setParams(const SPtr<Texture>& texture);
	private:
		GpuParamTexture mSkyTextureParam;
	};

	/** Set of properties describing the output render target used by a renderer view. */
	struct RENDERER_VIEW_TARGET_DESC
	{
		Rect2I viewRect;
		UINT32 targetWidth;
		UINT32 targetHeight;
		UINT32 numSamples;

		UINT32 clearFlags;
		Color clearColor;
		float clearDepthValue;
		UINT16 clearStencilValue;
	};

	/** Set of properties used describing a specific view that the renderer can render. */
	struct RENDERER_VIEW_DESC
	{
		RENDERER_VIEW_TARGET_DESC target;

		Matrix4 viewTransform;
		Matrix4 projTransform;
		Vector3 viewDirection;
		Vector3 viewOrigin;
		float nearPlane;

		bool isOverlay : 1;
		bool isHDR : 1;

		UINT64 visibleLayers;
		ConvexVolume cullFrustum;

		StateReduction stateReduction;

		SPtr<Texture> skyboxTexture;
	};

	/** Contains information about a Camera, used by the Renderer. */
	class RendererCamera
	{
	public:
		RendererCamera();
		RendererCamera(const RENDERER_VIEW_DESC& desc);

		/** Sets state reduction mode that determines how do render queues group & sort renderables. */
		void setStateReductionMode(StateReduction reductionMode);

		/** Updates the internal camera post-processing data. */
		void setPostProcessSettings(const SPtr<PostProcessSettings>& ppSettings);

		/** Updates the internal information with a new view transform. */
		void setTransform(const Vector3& origin, const Vector3& direction, const Matrix4& view,
						  const Matrix4& proj);

		/** Updates all internal information with new view information. */
		void setView(const RENDERER_VIEW_DESC& desc);

		/** 
		 * Prepares render targets for rendering. When done call endRendering().
		 *
		 * @param[in]	useRenderTargets	If using the internal render targets containing the GBuffer (retrieved via
		 *									getRenderTargets()) while rendering you must set this to true.
		 */
		void beginRendering(bool useRenderTargets);

		/** Ends rendering and frees any acquired resources. */
		void endRendering();

		/** Returns the view's renderTargets. Only valid if called in-between beginRendering() and endRendering() calls. */
		SPtr<RenderTargets> getRenderTargets() const { return mRenderTargets; }

		/** 
		 * Returns a render queue containing all opaque objects. Make sure to call determineVisible() beforehand if view 
		 * or object transforms changed since the last time it was called.
		 */
		const SPtr<RenderQueue>& getOpaqueQueue() const { return mOpaqueQueue; }
		
		/** 
		 * Returns a render queue containing all transparent objects. Make sure to call determineVisible() beforehand if 
		 * view or object transforms changed since the last time it was called.
		 */
		const SPtr<RenderQueue>& getTransparentQueue() const { return mTransparentQueue; }

		/**
		 * Populates view render queues by determining visible renderable objects. 
		 *
		 * @param[in]	renderables			A set of renderable objects to iterate over and determine visibility for.
		 * @param[in]	renderableBounds	A set of world bounds for the provided renderable objects. Must be the same size
		 *									as the @p renderables array.
		 * @param[in]	visibility			Output parameter that will have the true bit set for any visible renderable
		 *									object. If the bit for an object is already set to true, the method will never
		 *									change it to false which allows the same bitfield to be provided to multiple
		 *									renderer views. Must be the same size as the @p renderables array.
		 *									
		 *									As a side-effect, per-view visibility data is also calculated and can be
		 *									retrieved by calling getVisibilityMask().
		 */
		void determineVisible(const Vector<RendererObject*>& renderables, const Vector<Bounds>& renderableBounds, 
			Vector<bool>& visibility);

		/** Returns the visibility mask calculated with the last call to determineVisible(). */
		const Vector<bool>& getVisibilityMask() const { return mVisibility; }

		/** 
		 * Returns a structure containing information about post-processing effects. This structure will be modified and
		 * maintained by the post-processing system.
		 */
		PostProcessInfo& getPPInfo() { return mPostProcessInfo; }

		/** Updates the GPU buffer containing per-view information, with the latest internal data. */
		void updatePerViewBuffer();

		/** Returns a buffer that stores per-view parameters. */
		SPtr<GpuParamBlockBuffer> getPerViewBuffer() const { return mParamBuffer; }

	private:
		/**
		 * Extracts the necessary values from the projection matrix that allow you to transform device Z value into
		 * world Z value.
		 * 
		 * @param[in]	projMatrix	Projection matrix that was used to create the device Z value to transform.
		 * @return					Returns two values that can be used to transform device z to world z using this formula:
		 * 							z = (deviceZ + y) * x.
		 */
		Vector2 getDeviceZTransform(const Matrix4& projMatrix) const;

		RENDERER_VIEW_DESC mViewDesc;

		SPtr<RenderQueue> mOpaqueQueue;
		SPtr<RenderQueue> mTransparentQueue;

		SPtr<RenderTargets> mRenderTargets;
		PostProcessInfo mPostProcessInfo;
		bool mUsingRenderTargets;

		SPtr<GpuParamBlockBuffer> mParamBuffer;
		Vector<bool> mVisibility;
	};

	/** @} */
}}
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsRendererMaterial.h"
#include "BsParamBlocks.h"

namespace BansheeEngine
{
	/** @addtogroup RenderBeast
	 *  @{
	 */

	BS_PARAM_BLOCK_BEGIN(PerLightParamBuffer)
		BS_PARAM_BLOCK_ENTRY(Vector4, gLightPositionAndType)
		BS_PARAM_BLOCK_ENTRY(Vector4, gLightColorAndIntensity)
		BS_PARAM_BLOCK_ENTRY(Vector4, gLightSpotAnglesAndSqrdInvRadius)
		BS_PARAM_BLOCK_ENTRY(Vector3, gLightDirection)
		BS_PARAM_BLOCK_ENTRY(Vector4, gLightGeometry)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatConeTransform)
	BS_PARAM_BLOCK_END

	/** Manipulates PerLight parameter buffer used in various shaders. */
	class PerLightParams
	{
	public:
		/** Updates data in the parameter buffer from the data in the provided light. */
		void setParameters(const LightCore* light);

		/** Returns the internal parameter buffer that can be bound to the pipeline. */
		const SPtr<GpuParamBlockBufferCore>& getBuffer() const;
	private:
		PerLightParamBuffer mBuffer;
	};

	/** Shader that renders directional light sources during deferred rendering light pass. */
	class DirectionalLightMat : public RendererMaterial<DirectionalLightMat>
	{
		RMAT_DEF("DeferredDirectionalLightPass.bsl");

	public:
		DirectionalLightMat();

		/** Updates parameters that are common for all lights. */
		void setStaticParameters(const SPtr<RenderTargets>& gbuffer, const SPtr<GpuParamBlockBufferCore>& perCamera);

		/** Updates the parameter buffers used by the material. */
		void setParameters(const LightCore* light);
	private:
		PerLightParams mParams; // Note: Should this buffer be shared between both point and directional lights?
		MaterialParamTextureCore mGBufferA;
		MaterialParamTextureCore mGBufferB;
		MaterialParamTextureCore mGBufferDepth;
	};

	/** 
	 * Shader that renders point (radial & spot) light sources during deferred rendering light pass. Used when the camera
	 * is inside the point light geometry.
	 */
	class PointLightInMat : public RendererMaterial<PointLightInMat>
	{
		RMAT_DEF("DeferredPointLightPassIn.bsl");

	public:
		PointLightInMat();

		/** Updates parameters that are common for all lights.  */
		void setStaticParameters(const SPtr<RenderTargets>& gbuffer, const SPtr<GpuParamBlockBufferCore>& perCamera);

		/** Updates the parameter buffers used by the material. */
		void setParameters(const LightCore* light);
	private:
		PerLightParams mParams; // Note: Should this buffer be shared between both point and directional lights?
		MaterialParamTextureCore mGBufferA;
		MaterialParamTextureCore mGBufferB;
		MaterialParamTextureCore mGBufferDepth;
	};

	/** 
	 * Shader that renders point (radial & spot) light sources during deferred rendering light pass. Used when the camera
	 * is outside the point light geometry.
	 */
	class PointLightOutMat : public RendererMaterial<PointLightOutMat>
	{
		RMAT_DEF("DeferredPointLightPassOut.bsl");

	public:
		PointLightOutMat();

		/** Updates parameters that are common for all lights.  */
		void setStaticParameters(const SPtr<RenderTargets>& gbuffer, const SPtr<GpuParamBlockBufferCore>& perCamera);

		/** Updates the parameter buffers used by the material. */
		void setParameters(const LightCore* light);
	private:
		PerLightParams mParams; // Note: Should this buffer be shared between both point and directional lights?
		MaterialParamTextureCore mGBufferA;
		MaterialParamTextureCore mGBufferB;
		MaterialParamTextureCore mGBufferDepth;
	};

	/** @} */
}
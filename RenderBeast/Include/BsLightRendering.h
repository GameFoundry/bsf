#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsRendererMaterial.h"
#include "BsParamBlocks.h"

namespace BansheeEngine
{
	BS_PARAM_BLOCK_BEGIN(PerLightParamBuffer)
		BS_PARAM_BLOCK_ENTRY(Vector4, gLightPositionAndType)
		BS_PARAM_BLOCK_ENTRY(Vector4, gLightColorAndIntensity)
		BS_PARAM_BLOCK_ENTRY(Vector2, gLightSpotAngles)
		BS_PARAM_BLOCK_ENTRY(Vector3, gLightDirection)
		BS_PARAM_BLOCK_ENTRY(Vector4, gLightGeometry)
	BS_PARAM_BLOCK_END

	/**
	 * Manipulates PerLight parameter buffer used in various shaders.
	 */
	class PerLightParams
	{
	public:
		/**
		 * Updates data in the parameter buffer from the data in the provided light.
		 */
		void setParameters(const LightCore* light);

		/**
		 * Returns the internal parameter buffer that can be bound to the pipeline.
		 */
		const SPtr<GpuParamBlockBufferCore>& getBuffer() const;
	private:
		PerLightParamBuffer mBuffer;
	};

	/**
	 * Shader that renders directional light sources during deferred rendering light pass. 
	 */
	class DirectionalLightMat : public RendererMaterial<DirectionalLightMat>
	{
		RMAT_DEF("DeferredDirectionalLightPass.bsl");

	public:
		DirectionalLightMat();

		/**
		 * Updates the parameter buffers used by the material.
		 */
		void setParameters(const LightCore* light);
	private:
		PerLightParams mParams; // Note: Should this buffer be shared between both point and directional lights?
	};

	/**
	 * Shader that renders point (radial & spot) light sources during deferred rendering light pass. 
	 */
	class PointLightMat : public RendererMaterial<PointLightMat>
	{
		RMAT_DEF("DeferredPointLightPass.bsl");

	public:
		PointLightMat();

		/**
		 * Updates the parameter buffers used by the material.
		 */
		void setParameters(const LightCore* light);
	private:
		PerLightParams mParams; // Note: Should this buffer be shared between both point and directional lights?
	};
}
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsRendererMaterial.h"

namespace BansheeEngine
{
	/**
	 * Shader that renders directional light sources during deferred rendering light pass. 
	 */
	class DirectionalLightMat : public RendererMaterial<DirectionalLightMat>
	{
		RMAT_DEF("DeferredDirectionalLightPass.bsl");

	public:
		/**
		 * Updates the parameter buffers used by the material.
		 */
		void setParameters(const LightCore* light);
	private:
		/**
		 * @copydoc RendererMaterial::initialize
		 */
		void initialize() override;
	};

	/**
	 * Shader that renders point (radial & spot) light sources during deferred rendering light pass. 
	 */
	class PointLightMat : public RendererMaterial<PointLightMat>
	{
		RMAT_DEF("DeferredPointLightPass.bsl");

	public:
		/**
		 * Updates the parameter buffers used by the material.
		 */
		void setParameters(const LightCore* light);
	private:
		/**
		 * @copydoc RendererMaterial::initialize
		 */
		void initialize() override;
	};
}
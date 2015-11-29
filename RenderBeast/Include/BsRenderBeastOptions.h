#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsRenderer.h"
#include "BsRenderQueue.h"

namespace BansheeEngine
{
	/**
	 * @brief	Texture filtering options for RenderBeast.
	 */
	enum class RenderBeastFiltering
	{
		Bilinear, /**< Sample linearly in X and Y directions within a texture mip level. */
		Trilinear, /**< Sample bilinearly and also between texture mip levels to hide the mip transitions. */
		Anisotropic /**< High quality dynamic filtering that improves quality of angled surfaces */
	};

	/**
	 * @brief	A set of options used for controlling the 
	 *			rendering of the RenderBeast renderer.
	 */
	struct BS_BSRND_EXPORT RenderBeastOptions : public CoreRendererOptions
	{
		/**
		 * @brief	Type of filtering to use for all textures on scene elements.
		 */
		RenderBeastFiltering filtering = RenderBeastFiltering::Anisotropic;

		/**
		 * @brief	Maximum number of samples to be used when performing anisotropic
		 *			filtering. Only relevant if ::filter is set to RenderBeastFiltering::Anisotropic.
		 */
		UINT32 anisotropyMax = 16;

		/**
		 * @brief	Number of samples per pixel. More samples means less aliasing but this may
		 *			seriously increase fillrate and memory consumption on the GPU.
		 */

		UINT32 msaa = 1;
		/** 
		 * All colors output from shaders will be automatically converted to gamma 
		 * space when written to render target(s). Normally used when the renderer
		 * performs calculations in linear space.
		 */
		bool gammaCorrect = true; 

		/**
		 * High dynamic range allows light intensity to be more correctly recorded
		 * when rendering by allowing for a larger range of values. The stored light
		 * is then converted into visible colors using a tone mapping operator depending
		 * on average scene brightness.
		 */
		bool hdr = true;

		/**
		 * Controls if and how a render queue groups renderable objects by material in 
		 * order to reduce number of state changes. Sorting by material can reduce CPU usage
		 * but could increase overdraw.
		 */
		StateReduction stateReductionMode = StateReduction::Distance;
	};
}
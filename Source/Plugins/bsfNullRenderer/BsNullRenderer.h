//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsNullRendererPrerequisites.h"
#include "Renderer/BsRenderer.h"
#include "Renderer/BsRendererMaterial.h"
#include "Renderer/BsRendererFactory.h"
#include "Renderer/BsIBLUtility.h"

namespace bs
{
	/** Renderer factory implementation that creates and initializes the null renderer. Used by the RendererManager. */
	class NullRendererFactory : public RendererFactory
	{
	public:
		static constexpr const char* SystemName = "bsfNullRenderer";

		/** @copydoc RendererFactory::create */
		SPtr<ct::Renderer> create() override;

		/** @copydoc RendererFactory::name */
		const String& name() const override;
	};

	namespace ct
	{

	/** @addtogroup NullRenderer
	 *  @{
	 */

	/** Null renderer. */
	class NullRenderer final : public Renderer
	{
	public:
		NullRenderer() = default;

		/** @copydoc Renderer::getName */
		const StringID& getName() const override;

		/** @copydoc Renderer::renderAll */
		void renderAll(PerFrameData perFrameData) override;

		/** @copydoc Renderer::captureSceneCubeMap */
		void captureSceneCubeMap(const SPtr<Texture>& cubemap, const Vector3& position,
			const CaptureSettings& settings) override { }
	};

	/** Render beast implementation of IBLUtility. */
	class NullIBLUtility : public IBLUtility
	{
	public:
		/** @copydoc IBLUtility::filterCubemapForSpecular */
		void filterCubemapForSpecular(const SPtr<Texture>& cubemap, const SPtr<Texture>& scratch) const override { }

		/** @copydoc IBLUtility::filterCubemapForIrradiance(const SPtr<Texture>&, const SPtr<Texture>&) const */
		void filterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output) const override { }

		/** @copydoc IBLUtility::filterCubemapForIrradiance(const SPtr<Texture>&, const SPtr<Texture>&, UINT32) const */
		void filterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output,
			UINT32 outputIdx) const override { }

		/** @copydoc IBLUtility::scaleCubemap */
		void scaleCubemap(const SPtr<Texture>& src, UINT32 srcMip, const SPtr<Texture>& dst, UINT32 dstMip) const override { }
	};

	/**	Provides easy access to the null renderer. */
	SPtr<NullRenderer> gNullRenderer();

	/** @} */
}}

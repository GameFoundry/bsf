//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Image/BsTexture.h"
#include "RenderAPI/BsRenderTarget.h"

namespace bs
{
	/** @addtogroup RenderAPI
	 *  @{
	 */

	/**	Structure that describes a render texture color and depth/stencil surfaces. */
	struct BS_CORE_EXPORT RENDER_TEXTURE_DESC
	{
		RENDER_SURFACE_DESC colorSurfaces[BS_MAX_MULTIPLE_RENDER_TARGETS];
		RENDER_SURFACE_DESC depthStencilSurface;
	};

	namespace ct { struct RENDER_TEXTURE_DESC; }

	/**	Contains various properties that describe a render texture. */
	class BS_CORE_EXPORT RenderTextureProperties : public RenderTargetProperties
	{
	public:
		RenderTextureProperties(const RENDER_TEXTURE_DESC& desc, bool requiresFlipping);
		RenderTextureProperties(const ct::RENDER_TEXTURE_DESC& desc, bool requiresFlipping);
		virtual ~RenderTextureProperties() { }

	private:
		void construct(const TextureProperties* textureProps, UINT32 numSlices, UINT32 mipLevel, bool requiresFlipping,
					   bool hwGamma);

		friend class ct::RenderTexture;
		friend class RenderTexture;
	};

	/**
	 * Render target specialization that allows you to render into one or multiple textures. Such textures can then be used
	 * in other operations as GPU program input.
	 *
	 * @note	Sim thread only. Retrieve core implementation from getCore() for core thread only functionality.
	 */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Rendering) RenderTexture : public RenderTarget
	{
	public:
		virtual ~RenderTexture() = default;

		/** @copydoc TextureManager::createRenderTexture(const TEXTURE_DESC&, bool, PixelFormat) */
		static SPtr<RenderTexture> create(const TEXTURE_DESC& colorDesc,
			bool createDepth = true, PixelFormat depthStencilFormat = PF_D32);

		/** @copydoc TextureManager::createRenderTexture(const RENDER_TEXTURE_DESC&) */
		static SPtr<RenderTexture> create(const RENDER_TEXTURE_DESC& desc);

		/**
		 * Returns a color surface texture you may bind as an input to an GPU program.
		 *
		 * @note	Be aware that you cannot bind a render texture for reading and writing at the same time.
		 */
		const HTexture& getColorTexture(UINT32 idx) const { return mBindableColorTex[idx]; }

		/**
		 * Returns a depth/stencil surface texture you may bind as an input to an GPU program.
		 *
		 * @note	Be aware that you cannot bind a render texture for reading and writing at the same time.
		 */
		const HTexture& getDepthStencilTexture() const { return mBindableDepthStencilTex; }

		/**
		 * Retrieves a core implementation of a render texture usable only from the core thread.
		 *
		 * @note	Core thread only.
		 */
		SPtr<ct::RenderTexture> getCore() const;

		/**	Returns properties that describe the render texture. */
		const RenderTextureProperties& getProperties() const;

	protected:
		friend class TextureManager;

		RenderTexture(const RENDER_TEXTURE_DESC& desc);

		/** @copydoc CoreObject::createCore */
		SPtr<ct::CoreObject> createCore() const override;

		/** @copydoc CoreObject::syncToCore */
		CoreSyncData syncToCore(FrameAlloc* allocator) override;

	protected:
		HTexture mBindableColorTex[BS_MAX_MULTIPLE_RENDER_TARGETS];
		HTexture mBindableDepthStencilTex;

		RENDER_TEXTURE_DESC mDesc;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class RenderTextureRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** @} */

	namespace ct
	{
	/** @addtogroup RenderAPI-Internal
	 *  @{
	 */

	/**
	 * @see		bs::RENDER_TEXTURE_DESC
	 *
	 * @note	References core textures instead of texture handles.
	 */
	struct BS_CORE_EXPORT RENDER_TEXTURE_DESC
	{
		RENDER_SURFACE_DESC colorSurfaces[BS_MAX_MULTIPLE_RENDER_TARGETS];
		RENDER_SURFACE_DESC depthStencilSurface;
	};

	/**
	 * Provides access to internal render texture implementation usable only from the core thread.
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT RenderTexture : public RenderTarget
	{
	public:
		RenderTexture(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx);
		virtual ~RenderTexture() = default;

		/** @copydoc CoreObject::initialize */
		void initialize() override;

		/** @copydoc TextureManager::createRenderTexture(const RENDER_TEXTURE_DESC&, UINT32) */
		static SPtr<RenderTexture> create(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx = 0);

		/**
		 * Returns a color surface texture you may bind as an input to an GPU program.
		 *
		 * @note	Be aware that you cannot bind a render texture for reading and writing at the same time.
		 */
		SPtr<Texture> getColorTexture(UINT32 idx) const { return mDesc.colorSurfaces[idx].texture; }

		/**
		 * Returns a depth/stencil surface texture you may bind as an input to an GPU program.
		 *
		 * @note	Be aware that you cannot bind a render texture for reading and writing at the same time.
		 */
		SPtr<Texture> getDepthStencilTexture() const { return mDesc.depthStencilSurface.texture; }

		/**	Returns properties that describe the render texture. */
		const RenderTextureProperties& getProperties() const;

	protected:
		/** @copydoc CoreObject::syncToCore */
		void syncToCore(const CoreSyncData& data) override;

	private:
		/**	Throws an exception of the color and depth/stencil buffers aren't compatible. */
		void throwIfBuffersDontMatch() const;

	protected:
		friend class bs::RenderTexture;

		SPtr<TextureView> mColorSurfaces[BS_MAX_MULTIPLE_RENDER_TARGETS];
		SPtr<TextureView> mDepthStencilSurface;

		RENDER_TEXTURE_DESC mDesc;
	};

	/** @} */
	}
}

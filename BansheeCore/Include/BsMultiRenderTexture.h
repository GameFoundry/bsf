#pragma once

#include "BsCorePrerequisites.h"
#include "BsRenderTarget.h"

namespace BansheeEngine
{
	/**
	 * @brief	Descriptor class used for initializing a MultiRenderTexture.
	 *			Contains descriptors for each individual color render surface and 
	 *			their common depth/stencil surface.
	 */
	struct BS_CORE_EXPORT MULTI_RENDER_TEXTURE_DESC
	{
		Vector<RENDER_SURFACE_DESC> colorSurfaces;
		RENDER_SURFACE_DESC depthStencilSurface;
	};

	/**
	 * @brief	Contains various properties that describe a render texture.
	 */
	class BS_CORE_EXPORT MultiRenderTextureProperties : public RenderTargetProperties
	{
	public:
		MultiRenderTextureProperties(const MULTI_RENDER_TEXTURE_DESC& desc);
		virtual ~MultiRenderTextureProperties() { }

	protected:
		friend class MultiRenderTextureCore;
		friend class MultiRenderTexture;
	};

	/**
	 * @brief	Object representing multiple render textures. You may bind this to the pipeline
	 *			in order to render to all or some of the textures at once.
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT MultiRenderTextureCore : public RenderTargetCore
	{
	public:
		virtual ~MultiRenderTextureCore();

		/**
		 * @copydoc	CoreObjectCore::initialize
		 */
		virtual void initialize();

		/**
		 * @brief	Returns properties that describe the render texture.
		 */
		const MultiRenderTextureProperties& getProperties() const;

	protected:
		MultiRenderTextureCore(const MULTI_RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc	CoreObjectCore::syncFromCore
		 */
		virtual CoreSyncData syncFromCore(FrameAlloc* allocator);

		/**
		 * @copydoc	CoreObjectCore::syncToCore
		 */
		virtual void syncToCore(const CoreSyncData& data);

	private:
		/**
		 * @brief	Checks that all render surfaces and depth/stencil surface match. If they do not match
		 *			an exception is thrown.
		 */
		void throwIfBuffersDontMatch() const;

		// TODO - Not implemented
		virtual void copyToMemory(PixelData &dst, FrameBuffer buffer = FB_AUTO);

	protected:
		Vector<TextureViewPtr> mColorSurfaces;
		TextureViewPtr mDepthStencilSurface;

		MULTI_RENDER_TEXTURE_DESC mDesc;
	};

	/**
	 * @brief	Object representing multiple render textures. You may bind this to the pipeline
	 *			in order to render to all or some of the textures at once.
	 *
	 * @note	Sim thread only.
	 */
	class BS_CORE_EXPORT MultiRenderTexture : public RenderTarget
	{
	public:
		virtual ~MultiRenderTexture() { }

		/**
		 * @brief	Retrieves a core implementation of a render texture usable only from the
		 *			core thread.
		 */
		SPtr<MultiRenderTextureCore> getCore() const;

		/**
		 * @copydoc	TextureManager::createMultiRenderTexture
		 */
		static MultiRenderTexturePtr create(const MULTI_RENDER_TEXTURE_DESC& desc);

		/**
		 * @brief	Returns properties that describe the render texture.
		 */
		const MultiRenderTextureProperties& getProperties() const;

	protected:
		MultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc	RenderTarget::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		/**
		 * @copydoc	CoreObjectCore::syncToCore
		 */
		virtual CoreSyncData syncToCore(FrameAlloc* allocator);

		/**
		 * @copydoc	CoreObjectCore::syncFromCore
		 */
		virtual void syncFromCore(const CoreSyncData& data);

		MULTI_RENDER_TEXTURE_DESC mDesc;
	};
}
#pragma once

#include "BsGLPrerequisites.h"
#include "BsMultiRenderTexture.h"
#include "BsGLFrameBufferObject.h"

namespace BansheeEngine
{
	class GLMultiRenderTexture;

	/**
	 * @brief	OpenGL implementation of a render texture with multiple color surfaces.
	 *
	 * @note	Core thread only.
	 */
	class BS_RSGL_EXPORT GLMultiRenderTextureCore : public MultiRenderTextureCore
	{
	public:
		GLMultiRenderTextureCore(GLMultiRenderTexture* parent, MultiRenderTextureProperties* properties, const MULTI_RENDER_TEXTURE_DESC& desc);
		virtual ~GLMultiRenderTextureCore();

		/**
		 * @copydoc MultiRenderTextureCore::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;
	protected:
		friend class GLTextureManager;

		/**
		 * @copydoc	CoreObjectCore::initialize
		 */
		virtual void initialize();

		/**
		 * @copydoc	CoreObjectCore::destroy
		 */
		virtual void destroy();

	private:
		GLFrameBufferObject* mFB;
	};

	/**
	 * @brief	OpenGL implementation of a render texture with multiple color surfaces.
	 *
	 * @note	Sim thread only.
	 */
	class BS_RSGL_EXPORT GLMultiRenderTexture : public MultiRenderTexture
	{
	public:
		virtual ~GLMultiRenderTexture() { }

		/**
		 * @copydoc	MultiRenderTexture::requiresTextureFlipping
		 */
		bool requiresTextureFlipping() const { return true; }

	protected:
		friend class GLTextureManager;

		GLMultiRenderTexture() { }

		/**
		 * @copydoc	MultiRenderTexture::createProperties
		 */
		virtual RenderTargetProperties* createProperties() const;

		/**
		 * @copydoc	RenderTexture::createCore
		 */
		virtual SPtr<CoreObjectCore> createCore() const;
	};
}
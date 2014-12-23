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
		GLMultiRenderTextureCore(const MULTI_RENDER_TEXTURE_CORE_DESC& desc);
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

	private:
		GLFrameBufferObject* mFB;

		/**
		 * @copydoc	MultiRenderTextureCore::getProperties
		 */
		const RenderTargetProperties& getPropertiesInternal() const { return mProperties; }

		MultiRenderTextureProperties mProperties;
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

	protected:
		friend class GLTextureManager;

		GLMultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc	MultiRenderTexture::getProperties
		 */
		const RenderTargetProperties& getPropertiesInternal() const { return mProperties; }

		MultiRenderTextureProperties mProperties;
	};
}
#pragma once

#include "BsGLPrerequisites.h"
#include "BsMultiRenderTexture.h"
#include "BsGLFrameBufferObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	OpenGL implementation of a render texture with multiple color surfaces.
	 */
	class BS_RSGL_EXPORT GLMultiRenderTexture : public MultiRenderTexture
	{
	public:
		virtual ~GLMultiRenderTexture();

		/**
		 * @copydoc MultiRenderTexture::requiresTextureFlipping
		 */
		bool requiresTextureFlipping() const { return true; }

		/**
		 * @copydoc MultiRenderTexture::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;
	protected:
		friend class GLTextureManager;

		GLMultiRenderTexture();

		/**
		 * @copydoc MultiRenderTexture::initialize_internal
		 */
		void initialize_internal();

		/**
		 * @copydoc MultiRenderTexture::destroy_internal
		 */
		void destroy_internal();
	private:
		GLFrameBufferObject* mFB;
	};
}
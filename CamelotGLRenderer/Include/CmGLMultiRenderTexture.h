#pragma once

#include "CmGLPrerequisites.h"
#include "CmMultiRenderTexture.h"
#include "CmGLFrameBufferObject.h"

namespace CamelotEngine
{
	class CM_RSGL_EXPORT GLMultiRenderTexture : public MultiRenderTexture
	{
	public:
		virtual ~GLMultiRenderTexture();

		bool requiresTextureFlipping() const { return true; }
		void getCustomAttribute(const String& name, void* pData);
	protected:
		friend class GLTextureManager;

		GLMultiRenderTexture();

		void setColorSurfaceImpl(UINT32 surfaceIdx, TexturePtr texture, UINT32 face = 0, UINT32 numFaces = 0, UINT32 mipLevel = 0);
		void setDepthStencilImpl(TexturePtr depthStencilSurface, UINT32 face = 0, UINT32 numFaces = 1, UINT32 mipLevel = 0);

		/**
		 * @copydoc MultiRenderTexture::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @copydoc MultiRenderTexture::destroy_internal().
		 */
		void destroy_internal();
	private:
		GLFrameBufferObject* mFB;
	};
}
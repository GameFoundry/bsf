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
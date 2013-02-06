#include "CmGLMultiRenderTexture.h"
#include "CmGLTexture.h"

namespace CamelotEngine
{
	GLMultiRenderTexture::GLMultiRenderTexture()
		:MultiRenderTexture(), mFB(nullptr)
	{

	}

	GLMultiRenderTexture::~GLMultiRenderTexture()
	{

	}

	void GLMultiRenderTexture::initialize_internal()
	{
		if(mFB != nullptr)
			delete mFB;

		mFB = new GLFrameBufferObject(mFSAA);

		for(size_t i = 0; i < mColorSurfaces.size(); i++)
		{
			if(mColorSurfaces[i] != nullptr)
			{
				GLTexture* glColorSurface = static_cast<GLTexture*>(mColorSurfaces[i]->getTexture().get());
				GLPixelBufferPtr colorBuffer = std::static_pointer_cast<GLPixelBuffer>(
					glColorSurface->getBuffer(mColorSurfaces[i]->getDesc().firstArraySlice, mColorSurfaces[i]->getDesc().mostDetailMip));

				GLSurfaceDesc surfaceDesc;
				surfaceDesc.numSamples = mFSAA;
				surfaceDesc.zoffset = 0;
				surfaceDesc.buffer = colorBuffer;

				mFB->bindSurface((UINT32)i, surfaceDesc);
			}
			else
			{
				mFB->unbindSurface((UINT32)i);
			}
		}

		if(mDepthStencilSurface != nullptr)
		{
			GLTexture* glDepthStencilSurface = static_cast<GLTexture*>(mDepthStencilSurface->getTexture().get());
			GLPixelBufferPtr depthStencilBuffer = std::static_pointer_cast<GLPixelBuffer>(
				glDepthStencilSurface->getBuffer(mDepthStencilSurface->getDesc().firstArraySlice, mDepthStencilSurface->getDesc().mostDetailMip));

			mFB->bindDepthStencil(depthStencilBuffer);
		}
		else
		{
			mFB->unbindDepthStencil();
		}

		MultiRenderTexture::initialize_internal();
	}

	void GLMultiRenderTexture::destroy_internal()
	{
		if(mFB != nullptr)
			delete mFB;

		MultiRenderTexture::destroy_internal();
	}

	void GLMultiRenderTexture::getCustomAttribute(const String& name, void* pData)
	{
		if(name=="FBO")
		{
			*static_cast<GLFrameBufferObject **>(pData) = mFB;
		}
		else if (name == "GL_FBOID" || name == "GL_MULTISAMPLEFBOID")
		{
			*static_cast<GLuint*>(pData) = mFB->getGLFBOID();
		}
	}
}
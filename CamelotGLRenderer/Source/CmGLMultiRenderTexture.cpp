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

		MultiRenderTexture::initialize_internal();
	}

	void GLMultiRenderTexture::destroy_internal()
	{
		if(mFB != nullptr)
			delete mFB;

		MultiRenderTexture::destroy_internal();
	}

	void GLMultiRenderTexture::setColorSurfaceImpl(UINT32 surfaceIdx, TexturePtr texture, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		if(texture != nullptr)
		{
			GLSurfaceDesc surfaceDesc;
			surfaceDesc.numSamples = mFSAA;
			surfaceDesc.zoffset = 0;

			GLTexture* glTexture = static_cast<GLTexture*>(texture.get());
			surfaceDesc.buffer = std::static_pointer_cast<GLPixelBuffer>(glTexture->getBuffer(face, mipLevel));

			mFB->bindSurface(surfaceIdx, surfaceDesc);
		}
		else
		{
			mFB->unbindSurface(surfaceIdx);
		}
	}

	void GLMultiRenderTexture::setDepthStencilImpl(TexturePtr depthStencilSurface, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		if(depthStencilSurface != nullptr)
		{
			GLTexture* glDepthStencilSurface = static_cast<GLTexture*>(depthStencilSurface.get());
			GLPixelBufferPtr depthStencilBuffer = std::static_pointer_cast<GLPixelBuffer>(glDepthStencilSurface->getBuffer(face, mipLevel));

			mFB->bindDepthStencil(depthStencilBuffer);
		}
		else
		{
			mFB->unbindDepthStencil();
		}
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
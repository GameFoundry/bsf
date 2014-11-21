#include "BsGLMultiRenderTexture.h"
#include "BsGLTexture.h"

namespace BansheeEngine
{
	GLMultiRenderTextureCore::GLMultiRenderTextureCore(const MULTI_RENDER_TEXTURE_DESC& desc)
		:MultiRenderTextureCore(desc), mProperties(desc), mFB(nullptr)
	{ }

	GLMultiRenderTextureCore::~GLMultiRenderTextureCore()
	{ 
		if (mFB != nullptr)
			bs_delete(mFB);
	}

	void GLMultiRenderTextureCore::initialize()
	{
		MultiRenderTextureCore::initialize();

		if (mFB != nullptr)
			bs_delete(mFB);

		mFB = bs_new<GLFrameBufferObject, PoolAlloc>();

		for (size_t i = 0; i < mColorSurfaces.size(); i++)
		{
			if (mColorSurfaces[i] != nullptr)
			{
				GLTextureCore* glColorSurface = static_cast<GLTextureCore*>(mColorSurfaces[i]->getTexture().get());
				GLPixelBufferPtr colorBuffer = nullptr;
				GLSurfaceDesc surfaceDesc;

				if (glColorSurface->getProperties().getTextureType() != TEX_TYPE_3D)
				{
					surfaceDesc.zoffset = 0;
					colorBuffer = glColorSurface->getBuffer(mColorSurfaces[i]->getFirstArraySlice(),
						mColorSurfaces[i]->getMostDetailedMip());
				}
				else
				{
					surfaceDesc.zoffset = mColorSurfaces[i]->getFirstArraySlice();
					colorBuffer = glColorSurface->getBuffer(0, mColorSurfaces[i]->getMostDetailedMip());
				}

				surfaceDesc.numSamples = getProperties().getMultisampleCount();
				surfaceDesc.buffer = colorBuffer;

				mFB->bindSurface((UINT32)i, surfaceDesc);
			}
			else
			{
				mFB->unbindSurface((UINT32)i);
			}
		}

		if (mDepthStencilSurface != nullptr)
		{
			GLTextureCore* glDepthStencilSurface = static_cast<GLTextureCore*>(mDepthStencilSurface->getTexture().get());
			GLPixelBufferPtr depthStencilBuffer = nullptr;

			if (glDepthStencilSurface->getProperties().getTextureType() != TEX_TYPE_3D)
			{
				depthStencilBuffer = glDepthStencilSurface->getBuffer(mDepthStencilSurface->getDesc().firstArraySlice,
					mDepthStencilSurface->getDesc().mostDetailMip);
			}

			mFB->bindDepthStencil(depthStencilBuffer);
		}
		else
		{
			mFB->unbindDepthStencil();
		}

		MultiRenderTextureCore::initialize();
	}

	void GLMultiRenderTextureCore::getCustomAttribute(const String& name, void* pData) const
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

	GLMultiRenderTexture::GLMultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC& desc)
		:MultiRenderTexture(desc), mProperties(desc)
	{ }
}
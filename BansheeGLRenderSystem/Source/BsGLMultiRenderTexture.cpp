#include "BsGLMultiRenderTexture.h"
#include "BsGLTexture.h"

namespace BansheeEngine
{
	GLMultiRenderTextureCore::GLMultiRenderTextureCore(GLMultiRenderTexture* parent, MultiRenderTextureProperties* properties, const MULTI_RENDER_TEXTURE_DESC& desc)
		:MultiRenderTextureCore(parent, properties, desc), mFB(nullptr)
	{
		if (mFB != nullptr)
			bs_delete(mFB);

		mFB = bs_new<GLFrameBufferObject, PoolAlloc>();

		for (size_t i = 0; i < mColorSurfaces.size(); i++)
		{
			if (mColorSurfaces[i] != nullptr)
			{
				GLTexture* glColorSurface = static_cast<GLTexture*>(mColorSurfaces[i]->getTexture().get());
				GLPixelBufferPtr colorBuffer = nullptr;
				GLSurfaceDesc surfaceDesc;

				if (glColorSurface->getTextureType() != TEX_TYPE_3D)
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
			GLTexture* glDepthStencilSurface = static_cast<GLTexture*>(mDepthStencilSurface->getTexture().get());
			GLPixelBufferPtr depthStencilBuffer = nullptr;

			if (glDepthStencilSurface->getTextureType() != TEX_TYPE_3D)
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
	}

	GLMultiRenderTextureCore::~GLMultiRenderTextureCore()
	{
		if (mFB != nullptr)
			bs_delete(mFB);
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

	RenderTargetProperties* GLMultiRenderTexture::createProperties() const
	{
		return bs_new<MultiRenderTextureProperties>();
	}

	MultiRenderTextureCore* GLMultiRenderTexture::createCore(MultiRenderTextureProperties* properties, const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		return bs_new<GLMultiRenderTextureCore>(this, properties, desc);
	}
}
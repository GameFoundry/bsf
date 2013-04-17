#include "CmMultiRenderTexture.h"
#include "CmTexture.h"
#include "CmException.h"
#include "CmDebug.h"

namespace CamelotFramework
{
	MultiRenderTexture::MultiRenderTexture()
	{
		mColorSurfaces.resize(CM_MAX_MULTIPLE_RENDER_TARGETS);
	}

	MultiRenderTexture::~MultiRenderTexture()
	{

	}

	void MultiRenderTexture::initialize(const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		bool colorSurfacePropertiesSet = false;
		for(size_t i = 0; i < desc.colorSurfaces.size(); i++)
		{
			if(desc.colorSurfaces[i].texture != nullptr)
			{
				if(i >= CM_MAX_MULTIPLE_RENDER_TARGETS)
				{
					LOGWRN("Render texture index is larger than the maximum number of supported render targets. Index: " + toString(i) + 
						". Max. number of render targets: " + toString(CM_MAX_MULTIPLE_RENDER_TARGETS));

					continue;
				}

				TexturePtr texture = desc.colorSurfaces[i].texture;

				if(texture->getUsage() != TU_RENDERTARGET)
					CM_EXCEPT(InvalidParametersException, "Provided texture is not created with render target usage.");

				mColorSurfaces[i] = Texture::requestView(texture, desc.colorSurfaces[i].mipLevel, 1, 
					desc.colorSurfaces[i].face, desc.colorSurfaces[i].numFaces, GVU_RENDERTARGET);

				if(!colorSurfacePropertiesSet)
				{
					mPriority = CM_REND_TO_TEX_RT_GROUP;
					mWidth = texture->getWidth();
					mHeight = texture->getWidth();
					mColorDepth = CamelotFramework::PixelUtil::getNumElemBits(texture->getFormat());
					mActive = true;
					mHwGamma = texture->isHardwareGammaEnabled();
					mFSAA = texture->getFSAA();
					mFSAAHint = texture->getFSAAHint();

					colorSurfacePropertiesSet = true;
				}
			}
		}

		if(desc.depthStencilSurface.texture != nullptr)
		{
			TexturePtr texture = desc.depthStencilSurface.texture;

			if(texture->getUsage() != TU_DEPTHSTENCIL)
				CM_EXCEPT(InvalidParametersException, "Provided texture is not created with depth stencil usage.");

			mDepthStencilSurface = Texture::requestView(texture, desc.depthStencilSurface.mipLevel, 1, 
				desc.depthStencilSurface.face, desc.depthStencilSurface.numFaces, GVU_DEPTHSTENCIL);
		}

		throwIfBuffersDontMatch();

		RenderTarget::initialize();
	}

	void MultiRenderTexture::destroy_internal()
	{
		for(auto iter = mColorSurfaces.begin(); iter != mColorSurfaces.end(); ++iter)
		{
			if(*iter != nullptr)
				Texture::releaseView(*iter);
		}

		if(mDepthStencilSurface != nullptr)
			Texture::releaseView(mDepthStencilSurface);

		RenderTarget::destroy_internal();
	}

	void MultiRenderTexture::throwIfBuffersDontMatch() const
	{
		TextureViewPtr firstSurfaceDesc = nullptr;
		for(size_t i = 0; i < mColorSurfaces.size(); i++)
		{
			if(mColorSurfaces[i] == nullptr)
				continue;

			if(firstSurfaceDesc == nullptr)
			{
				firstSurfaceDesc = mColorSurfaces[i];
				continue;
			}

			if(mColorSurfaces[i]->getTexture()->getWidth() != firstSurfaceDesc->getTexture()->getWidth() ||
				mColorSurfaces[i]->getTexture()->getHeight() != firstSurfaceDesc->getTexture()->getHeight() ||
				mColorSurfaces[i]->getTexture()->getFSAA() != firstSurfaceDesc->getTexture()->getFSAA() ||
				mColorSurfaces[i]->getTexture()->getFSAAHint() != firstSurfaceDesc->getTexture()->getFSAAHint())
			{
				String errorInfo = "\nWidth: " + toString(mColorSurfaces[i]->getTexture()->getWidth()) + "/" + toString(firstSurfaceDesc->getTexture()->getWidth());
				errorInfo += "\nHeight: " + toString(mColorSurfaces[i]->getTexture()->getHeight()) + "/" + toString(firstSurfaceDesc->getTexture()->getHeight());
				errorInfo += "\nFSAA: " + toString(mColorSurfaces[i]->getTexture()->getFSAA()) + "/" + toString(firstSurfaceDesc->getTexture()->getFSAA());
				errorInfo += "\nFSAAHint: " + mColorSurfaces[i]->getTexture()->getFSAAHint() + "/" + firstSurfaceDesc->getTexture()->getFSAAHint();

				CM_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
			}
		}

		if(firstSurfaceDesc == nullptr)
			return;

		if(firstSurfaceDesc->getTexture()->getTextureType() != TEX_TYPE_2D)
			CM_EXCEPT(NotImplementedException, "Render textures are currently only implemented for 2D surfaces.");

		if((firstSurfaceDesc->getFirstArraySlice() + firstSurfaceDesc->getNumArraySlices()) >= firstSurfaceDesc->getTexture()->getNumFaces())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of faces is out of range. Face: " + 
				toString(firstSurfaceDesc->getFirstArraySlice() + firstSurfaceDesc->getNumArraySlices()) + ". Max num faces: " + toString(firstSurfaceDesc->getTexture()->getNumFaces()));
		}

		if(firstSurfaceDesc->getMostDetailedMip() >= firstSurfaceDesc->getTexture()->getNumMipmaps())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of mip maps is out of range. Mip level: " + 
				toString(firstSurfaceDesc->getMostDetailedMip()) + ". Max num mipmaps: " + toString(firstSurfaceDesc->getTexture()->getNumMipmaps()));
		}

		if(mDepthStencilSurface == nullptr)
			return;

		if(mDepthStencilSurface->getTexture()->getWidth() != firstSurfaceDesc->getTexture()->getWidth() ||
			mDepthStencilSurface->getTexture()->getHeight() != firstSurfaceDesc->getTexture()->getHeight() ||
			mDepthStencilSurface->getTexture()->getFSAA() != firstSurfaceDesc->getTexture()->getFSAA() ||
			mDepthStencilSurface->getTexture()->getFSAAHint() != firstSurfaceDesc->getTexture()->getFSAAHint())
		{
			String errorInfo = "\nWidth: " + toString(mDepthStencilSurface->getTexture()->getWidth()) + "/" + toString(firstSurfaceDesc->getTexture()->getWidth());
			errorInfo += "\nHeight: " + toString(mDepthStencilSurface->getTexture()->getHeight()) + "/" + toString(firstSurfaceDesc->getTexture()->getHeight());
			errorInfo += "\nFSAA: " + toString(mDepthStencilSurface->getTexture()->getFSAA()) + "/" + toString(firstSurfaceDesc->getTexture()->getFSAA());
			errorInfo += "\nFSAAHint: " + mDepthStencilSurface->getTexture()->getFSAAHint() + "/" + firstSurfaceDesc->getTexture()->getFSAAHint();

			CM_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}
	}

	void MultiRenderTexture::copyContentsToMemory( const PixelData &dst, FrameBuffer buffer /*= FB_AUTO */ )
	{
		throw std::exception("The method or operation is not implemented.");
	}
}
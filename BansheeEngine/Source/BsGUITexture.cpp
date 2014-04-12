#include "BsGUITexture.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUILayoutOptions.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUITexture::getGUITypeName()
	{
		static String name = "Texture";
		return name;
	}

	GUITexture::GUITexture(const GUIElementStyle* style, const HSpriteTexture& texture, 
		GUIImageScaleMode scale, const GUILayoutOptions& layoutOptions)
		:GUIElement(style, layoutOptions), mScaleMode(scale), mUsingStyleTexture(false)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();

		if(texture != nullptr)
		{
			mActiveTexture = texture;
			mUsingStyleTexture = false;
		}
		else
			mUsingStyleTexture = true;
	}

	GUITexture::~GUITexture()
	{
		cm_delete<PoolAlloc>(mImageSprite);
	}

	GUITexture* GUITexture::create(const HSpriteTexture& texture, 
		GUIImageScaleMode scale, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUITexture, PoolAlloc>()) GUITexture(style, texture, scale, GUILayoutOptions::create(layoutOptions, style));
	}

	GUITexture* GUITexture::create(const HSpriteTexture& texture, 
		GUIImageScaleMode scale, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUITexture, PoolAlloc>()) GUITexture(style, texture, scale, GUILayoutOptions::create(style));
	}

	GUITexture* GUITexture::create(const HSpriteTexture& texture, 
		const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUITexture, PoolAlloc>()) GUITexture(style, texture, GUIImageScaleMode::StretchToFit, GUILayoutOptions::create(layoutOptions, style));
	}

	GUITexture* GUITexture::create(const HSpriteTexture& texture, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUITexture, PoolAlloc>()) GUITexture(style, texture, GUIImageScaleMode::StretchToFit, GUILayoutOptions::create(style));
	}

	GUITexture* GUITexture::create(GUIImageScaleMode scale, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUITexture, PoolAlloc>()) GUITexture(style, HTexture(), scale, GUILayoutOptions::create(layoutOptions, style));
	}

	GUITexture* GUITexture::create(GUIImageScaleMode scale, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUITexture, PoolAlloc>()) GUITexture(style, HSpriteTexture(), scale, GUILayoutOptions::create(style));
	}

	GUITexture* GUITexture::create(const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUITexture, PoolAlloc>()) GUITexture(style, HTexture(), GUIImageScaleMode::StretchToFit, GUILayoutOptions::create(layoutOptions, style));
	}

	GUITexture* GUITexture::create(const GUIElementStyle* style)
	{
		return new (cm_alloc<GUITexture, PoolAlloc>()) GUITexture(style, HTexture(), GUIImageScaleMode::StretchToFit, GUILayoutOptions::create(style));
	}

	void GUITexture::setTexture(const HSpriteTexture& texture)
	{
		mActiveTexture = texture;
		mUsingStyleTexture = false;

		markContentAsDirty();
	}

	UINT32 GUITexture::getNumRenderElements() const
	{
		return mImageSprite->getNumRenderElements();
	}

	const GUIMaterialInfo& GUITexture::getMaterial(UINT32 renderElementIdx) const
	{
		return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUITexture::getNumQuads(UINT32 renderElementIdx) const
	{
		return mImageSprite->getNumQuads(renderElementIdx);
	}

	void GUITexture::updateRenderElementsInternal()
	{		
		mDesc.width = mWidth;
		mDesc.height = mHeight;

		mDesc.borderLeft = _getStyle()->border.left;
		mDesc.borderRight = _getStyle()->border.right;
		mDesc.borderTop = _getStyle()->border.top;
		mDesc.borderBottom = _getStyle()->border.bottom;

		if(mUsingStyleTexture)
			mActiveTexture = _getStyle()->normal.texture;

		float optimalWidth = 0.0f;
		float optimalHeight = 0.0f;
		if(mActiveTexture != nullptr && mActiveTexture.isLoaded())
		{
			mDesc.texture = mActiveTexture.getInternalPtr();
			optimalWidth = (float)mDesc.texture->getTexture()->getWidth();
			optimalHeight = (float)mDesc.texture->getTexture()->getHeight();
		}

		switch (mScaleMode)
		{
		case GUIImageScaleMode::StretchToFit:
			mDesc.uvScale = Vector2(1.0f, 1.0f);
			break;
		case GUIImageScaleMode::ScaleToFit:
			mDesc.uvScale.x = optimalWidth / mWidth;
			mDesc.uvScale.y = optimalHeight / mHeight;

			if(mDesc.uvScale.x < mDesc.uvScale.y)
			{
				mDesc.uvScale.x = 1.0f;
				mDesc.uvScale.y = (mWidth * (optimalHeight / optimalWidth)) / mHeight;
			}
			else
			{
				mDesc.uvScale.x = (mHeight * (optimalWidth / optimalHeight)) / mWidth;
				mDesc.uvScale.y = 1.0f;
			}

			break;
		case GUIImageScaleMode::CropToFit:
			mDesc.uvScale.x = optimalWidth / mWidth;
			mDesc.uvScale.y = optimalHeight / mHeight;

			if(mDesc.uvScale.x < mDesc.uvScale.y)
			{
				mDesc.uvScale.x = (mHeight * (optimalWidth / optimalHeight)) / mWidth;
				mDesc.uvScale.y = 1.0f;
			}
			else
			{
				mDesc.uvScale.x = 1.0f;
				mDesc.uvScale.y = (mWidth * (optimalHeight / optimalWidth)) / mHeight;
			}

			break;
		case GUIImageScaleMode::RepeatToFit:
			mDesc.uvScale.x = mWidth / optimalWidth;
			mDesc.uvScale.y = mHeight / optimalHeight;
			break;
		default:
			break;
		}

		mImageSprite->update(mDesc);
		
		GUIElement::updateRenderElementsInternal();
	}

	void GUITexture::updateClippedBounds()
	{
		mClippedBounds = mImageSprite->getBounds(mOffset, mClipRect);
	}

	Vector2I GUITexture::_getOptimalSize() const
	{
		Vector2I optimalSize;

		if(_getLayoutOptions().fixedWidth)
			optimalSize.x = _getLayoutOptions().width;
		else
		{
			if(mActiveTexture != nullptr && mActiveTexture.isLoaded())
				optimalSize.x = mActiveTexture->getTexture()->getWidth();
			else
				optimalSize.x = _getLayoutOptions().maxWidth;
		}

		if(_getLayoutOptions().fixedHeight)
			optimalSize.y = _getLayoutOptions().height;
		else
		{
			if(mActiveTexture != nullptr && mActiveTexture.isLoaded())
				optimalSize.y = mActiveTexture->getTexture()->getHeight();
			else
				optimalSize.y = _getLayoutOptions().maxHeight;
		}

		return optimalSize;
	}

	void GUITexture::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx, mOffset, mClipRect);
	}
}
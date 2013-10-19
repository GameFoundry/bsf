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

	GUITexture::GUITexture(GUIWidget& parent, const GUIElementStyle* style, const SpriteTexturePtr& texture, 
		GUIImageScaleMode scale, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mScaleMode(scale)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();

		if(texture != nullptr)
			mDesc.texture = texture;
		else
			mDesc.texture = style->normal.texture;
	
		mDesc.borderLeft = mStyle->border.left;
		mDesc.borderRight = mStyle->border.right;
		mDesc.borderTop = mStyle->border.top;
		mDesc.borderBottom = mStyle->border.bottom;
	}

	GUITexture::~GUITexture()
	{
		cm_delete<PoolAlloc>(mImageSprite);
	}

	GUITexture* GUITexture::create(GUIWidget& parent, const SpriteTexturePtr& texture, 
		GUIImageScaleMode scale, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUITexture, PoolAlloc>()) GUITexture(parent, style, texture, scale, GUILayoutOptions::create(style));
	}

	GUITexture* GUITexture::create(GUIWidget& parent, const GUIOptions& layoutOptions, const SpriteTexturePtr& texture, 
		GUIImageScaleMode scale, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUITexture, PoolAlloc>()) GUITexture(parent, style, texture, scale, GUILayoutOptions::create(layoutOptions, style));
	}

	GUITexture* GUITexture::create(GUIWidget& parent, GUIImageScaleMode scale, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUITexture, PoolAlloc>()) GUITexture(parent, style, nullptr, scale, GUILayoutOptions::create(style));
	}

	GUITexture* GUITexture::create(GUIWidget& parent, const GUIOptions& layoutOptions, GUIImageScaleMode scale, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUITexture, PoolAlloc>()) GUITexture(parent, style, nullptr, scale, GUILayoutOptions::create(layoutOptions, style));
	}

	UINT32 GUITexture::getNumRenderElements() const
	{
		return mImageSprite->getNumRenderElements();
	}

	const HMaterial& GUITexture::getMaterial(UINT32 renderElementIdx) const
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

		float optimalWidth = 0.0f;
		float optimalHeight = 0.0f;
		if(mDesc.texture != nullptr)
		{
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

	Int2 GUITexture::_getOptimalSize() const
	{
		if(mDesc.texture != nullptr)
		{
			return Int2(mDesc.texture->getTexture()->getWidth(), mDesc.texture->getTexture()->getHeight());
		}

		return Int2(0, 0);
	}

	void GUITexture::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx, mOffset, mClipRect);
	}
}
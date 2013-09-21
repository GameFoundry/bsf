#include "BsGUIButton.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIButton::getGUITypeName()
	{
		static String name = "Button";
		return name;
	}

	GUIButton::GUIButton(GUIWidget& parent, const GUIElementStyle* style, const GUIContent& content, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mContent(content), mContentImageSprite(nullptr)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();
		mTextSprite = cm_new<TextSprite, PoolAlloc>();

		SpriteTexturePtr contentTex = content.getImage();
		if(contentTex != nullptr)
			mContentImageSprite = cm_new<ImageSprite, PoolAlloc>();

		mImageDesc.texture = mStyle->normal.texture;

		if(mImageDesc.texture != nullptr)
		{
			mImageDesc.width = mImageDesc.texture->getTexture()->getWidth();
			mImageDesc.height = mImageDesc.texture->getTexture()->getHeight();
		}

		mImageDesc.borderLeft = mStyle->border.left;
		mImageDesc.borderRight = mStyle->border.right;
		mImageDesc.borderTop = mStyle->border.top;
		mImageDesc.borderBottom = mStyle->border.bottom;
	}

	GUIButton::~GUIButton()
	{
		cm_delete<PoolAlloc>(mTextSprite);
		cm_delete<PoolAlloc>(mImageSprite);

		if(mContentImageSprite != nullptr)
			cm_delete<PoolAlloc>(mContentImageSprite);
	}

	GUIButton* GUIButton::create(GUIWidget& parent, const WString& text, const GUIElementStyle* style)
	{
		return create(parent, GUIContent(text), style);
	}

	GUIButton* GUIButton::create(GUIWidget& parent, const WString& text, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		return create(parent, GUIContent(text), layoutOptions, style);
	}

	GUIButton* GUIButton::create(GUIWidget& parent, const GUIContent& content, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIButton, PoolAlloc>()) GUIButton(parent, style, content, getDefaultLayoutOptions(style));
	}

	GUIButton* GUIButton::create(GUIWidget& parent, const GUIContent& content, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIButton, PoolAlloc>()) GUIButton(parent, style, content, layoutOptions);
	}

	void GUIButton::setContent(const GUIContent& content)
	{
		mContent = content;

		markContentAsDirty();
	}

	UINT32 GUIButton::getNumRenderElements() const
	{
		UINT32 numElements = mImageSprite->getNumRenderElements();
		numElements += mTextSprite->getNumRenderElements();

		if(mContentImageSprite != nullptr)
			numElements += mContentImageSprite->getNumRenderElements();

		return numElements;
	}

	const HMaterial& GUIButton::getMaterial(UINT32 renderElementIdx) const
	{
		UINT32 textSpriteIdx = mImageSprite->getNumRenderElements();
		UINT32 contentImgSpriteIdx = textSpriteIdx + mTextSprite->getNumRenderElements();

		if(renderElementIdx >= contentImgSpriteIdx)
			return mContentImageSprite->getMaterial(contentImgSpriteIdx - renderElementIdx);
		else if(renderElementIdx >= textSpriteIdx)
			return mTextSprite->getMaterial(textSpriteIdx - renderElementIdx);
		else
			return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUIButton::getNumQuads(UINT32 renderElementIdx) const
	{
		UINT32 textSpriteIdx = mImageSprite->getNumRenderElements();
		UINT32 contentImgSpriteIdx = textSpriteIdx + mTextSprite->getNumRenderElements();

		UINT32 numQuads = 0;
		if(renderElementIdx >= contentImgSpriteIdx)
			numQuads = mContentImageSprite->getNumQuads(contentImgSpriteIdx - renderElementIdx);
		else if(renderElementIdx >= textSpriteIdx)
			numQuads = mTextSprite->getNumQuads(textSpriteIdx - renderElementIdx);
		else
			numQuads = mImageSprite->getNumQuads(renderElementIdx);

		return numQuads;
	}

	void GUIButton::updateRenderElementsInternal()
	{		
		mImageDesc.width = mWidth;
		mImageDesc.height = mHeight;

		mImageSprite->update(mImageDesc);

		TEXT_SPRITE_DESC textDesc;
		textDesc.text = mContent.getText();
		textDesc.font = mStyle->font;
		textDesc.fontSize = mStyle->fontSize;

		Rect textBounds = getContentBounds();

		textDesc.width = textBounds.width;
		textDesc.height = textBounds.height;
		textDesc.horzAlign = mStyle->textHorzAlign;
		textDesc.vertAlign = mStyle->textVertAlign;

		mTextSprite->update(textDesc);

		if(mContentImageSprite != nullptr)
		{
			IMAGE_SPRITE_DESC contentImgDesc;
			contentImgDesc.texture = mContent.getImage();
			contentImgDesc.width = mContent.getImage()->getTexture()->getWidth();
			contentImgDesc.height = mContent.getImage()->getTexture()->getHeight();

			mContentImageSprite->update(contentImgDesc);
		}

		GUIElement::updateRenderElementsInternal();
	}

	void GUIButton::updateClippedBounds()
	{
		mClippedBounds = mImageSprite->getBounds(mOffset, mClipRect);
	}

	UINT32 GUIButton::_getOptimalWidth() const
	{
		if(mImageDesc.texture != nullptr)
		{
			return mImageDesc.texture->getTexture()->getWidth();
		}

		return 0;
	}

	UINT32 GUIButton::_getOptimalHeight() const
	{
		if(mImageDesc.texture != nullptr)
		{
			return mImageDesc.texture->getTexture()->getHeight();
		}

		return 0;
	}

	UINT32 GUIButton::_getRenderElementDepth(UINT32 renderElementIdx) const
	{
		UINT32 textSpriteIdx = mImageSprite->getNumRenderElements();
		UINT32 contentImgSpriteIdx = textSpriteIdx + mTextSprite->getNumRenderElements();

		if(renderElementIdx >= contentImgSpriteIdx)
			return _getDepth();
		else if(renderElementIdx >= textSpriteIdx)
			return _getDepth();
		else
			return _getDepth() + 1;
	}

	void GUIButton::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		UINT32 textSpriteIdx = mImageSprite->getNumRenderElements();
		UINT32 contentImgSpriteIdx = textSpriteIdx + mTextSprite->getNumRenderElements();

		if(renderElementIdx < textSpriteIdx)
		{
			mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
				vertexStride, indexStride, renderElementIdx, mOffset, mClipRect);

			return;
		}

		Rect contentBounds = getContentBounds();
		Rect contentClipRect = getContentClipRect();
		Rect textBounds = mTextSprite->getBounds(Int2(), Rect());

		Int2 textOffset;
		Rect textClipRect;

		Int2 imageOffset;
		Rect imageClipRect;
		if(mContentImageSprite != nullptr)
		{
			Rect imageBounds = mContentImageSprite->getBounds(Int2(), Rect());
			UINT32 freeWidth = (UINT32)std::max(0, contentBounds.width - textBounds.width - imageBounds.width);
			INT32 imageXOffset = (INT32)(freeWidth / 2);

			if(mStyle->imagePosition == GUIImagePosition::Right)
			{
				INT32 imageReservedWidth = std::max(0, contentBounds.width - textBounds.width);

				textOffset = Int2(contentBounds.x, contentBounds.y);
				textClipRect = contentClipRect;
				textClipRect.width = std::min(contentBounds.width - imageReservedWidth, textClipRect.width);

				imageOffset = Int2(contentBounds.x + textBounds.width + imageXOffset, contentBounds.y);
				imageClipRect = contentClipRect;
				imageClipRect.x -= textBounds.width + imageXOffset;
			}
			else
			{
				INT32 imageReservedWidth = imageBounds.width + imageXOffset;

				imageOffset = Int2(contentBounds.x + imageXOffset, contentBounds.y);
				imageClipRect = contentClipRect;
				imageClipRect.x -= imageXOffset;
				imageClipRect.width = std::min(imageReservedWidth, imageClipRect.width);

				textOffset = Int2(contentBounds.x + imageReservedWidth, contentBounds.y);
				textClipRect = contentClipRect;
				textClipRect.x -= imageReservedWidth;
			}

			INT32 imageYOffset = (contentBounds.height - imageBounds.height) / 2;
			imageClipRect.y -= imageYOffset;
			imageOffset.y += imageYOffset;
		}
		else
		{
			textOffset = Int2(contentBounds.x, contentBounds.y);
			textClipRect = contentClipRect;
		}

		if(renderElementIdx >= contentImgSpriteIdx)
		{
			mContentImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
				vertexStride, indexStride, contentImgSpriteIdx - renderElementIdx, imageOffset, imageClipRect);
		}
		else
		{
			mTextSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
			vertexStride, indexStride, textSpriteIdx - renderElementIdx, textOffset, textClipRect);
		}
	}

	bool GUIButton::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseOver)
		{
			mImageDesc.texture = mStyle->hover.texture;
			markContentAsDirty();

			if(!onHover.empty())
				onHover();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseOut)
		{
			mImageDesc.texture = mStyle->normal.texture;
			markContentAsDirty();

			if(!onOut.empty())
				onOut();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDown)
		{
			mImageDesc.texture = mStyle->active.texture;
			markContentAsDirty();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseUp)
		{
			mImageDesc.texture = mStyle->hover.texture;
			markContentAsDirty();

			if(!onClick.empty())
				onClick();

			return true;
		}
		
		return false;
	}
}
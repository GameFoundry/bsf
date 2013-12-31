#include "BsGUIButtonBase.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIHelper.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIButtonBase::GUIButtonBase(GUIWidget& parent, const GUIElementStyle* style, const GUIContent& content, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mContent(content), mContentImageSprite(nullptr), mActiveState(GUIButtonState::Normal)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();
		mTextSprite = cm_new<TextSprite, PoolAlloc>();

		HSpriteTexture contentTex = content.getImage();
		if(contentTex != nullptr && contentTex.isLoaded())
			mContentImageSprite = cm_new<ImageSprite, PoolAlloc>();

		mActiveTexture = mStyle->normal.texture;
		if(mActiveTexture != nullptr && mActiveTexture.isLoaded())
		{
			mImageDesc.width = mActiveTexture->getTexture()->getWidth();
			mImageDesc.height = mActiveTexture->getTexture()->getHeight();
		}

		mImageDesc.borderLeft = mStyle->border.left;
		mImageDesc.borderRight = mStyle->border.right;
		mImageDesc.borderTop = mStyle->border.top;
		mImageDesc.borderBottom = mStyle->border.bottom;

		mLocStringUpdatedConn = mContent.getText().addOnStringModifiedCallback(boost::bind(&GUIButtonBase::markContentAsDirty, this));
	}

	GUIButtonBase::~GUIButtonBase()
	{
		mLocStringUpdatedConn.disconnect();

		cm_delete<PoolAlloc>(mTextSprite);
		cm_delete<PoolAlloc>(mImageSprite);

		if(mContentImageSprite != nullptr)
			cm_delete<PoolAlloc>(mContentImageSprite);
	}

	void GUIButtonBase::setContent(const GUIContent& content)
	{
		mLocStringUpdatedConn.disconnect();
		mLocStringUpdatedConn = content.getText().addOnStringModifiedCallback(boost::bind(&GUIButtonBase::markContentAsDirty, this));

		mContent = content;

		markContentAsDirty();
	}

	void GUIButtonBase::_setOn(bool on) 
	{ 
		if(on)
			setState((GUIButtonState)((INT32)mActiveState | 0x10)); 
		else
			setState((GUIButtonState)((INT32)mActiveState & (~0x10))); 
	}

	bool GUIButtonBase::_isOn() const
	{
		return ((INT32)mActiveState & 0x10) != 0;
	}

	UINT32 GUIButtonBase::getNumRenderElements() const
	{
		UINT32 numElements = mImageSprite->getNumRenderElements();
		numElements += mTextSprite->getNumRenderElements();

		if(mContentImageSprite != nullptr)
			numElements += mContentImageSprite->getNumRenderElements();

		return numElements;
	}

	const GUIMaterialInfo& GUIButtonBase::getMaterial(UINT32 renderElementIdx) const
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

	UINT32 GUIButtonBase::getNumQuads(UINT32 renderElementIdx) const
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

	void GUIButtonBase::updateRenderElementsInternal()
	{		
		mImageDesc.width = mWidth;
		mImageDesc.height = mHeight;

		if(mActiveTexture != nullptr && mActiveTexture.isLoaded())
			mImageDesc.texture = mActiveTexture.getInternalPtr();

		mImageSprite->update(mImageDesc);

		mTextSprite->update(getTextDesc());

		if(mContentImageSprite != nullptr)
		{
			IMAGE_SPRITE_DESC contentImgDesc;
			contentImgDesc.texture = mContent.getImage().getInternalPtr();
			contentImgDesc.width = mContent.getImage()->getTexture()->getWidth();
			contentImgDesc.height = mContent.getImage()->getTexture()->getHeight();

			mContentImageSprite->update(contentImgDesc);
		}

		GUIElement::updateRenderElementsInternal();
	}

	void GUIButtonBase::updateClippedBounds()
	{
		mClippedBounds = mImageSprite->getBounds(mOffset, mClipRect);
	}

	Vector2I GUIButtonBase::_getOptimalSize() const
	{
		UINT32 imageWidth = 0;
		UINT32 imageHeight = 0;
		if(mActiveTexture != nullptr)
		{
			imageWidth = mActiveTexture->getTexture()->getWidth();
			imageHeight = mActiveTexture->getTexture()->getHeight();
		}

		Vector2I contentSize = GUIHelper::calcOptimalContentsSize(mContent, *mStyle, _getLayoutOptions());
		UINT32 contentWidth = std::max(imageWidth, (UINT32)contentSize.x);
		UINT32 contentHeight = std::max(imageHeight, (UINT32)contentSize.y);

		return Vector2I(contentWidth, contentHeight);
	}

	UINT32 GUIButtonBase::_getRenderElementDepth(UINT32 renderElementIdx) const
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

	void GUIButtonBase::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
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

		RectI contentBounds = getContentBounds();
		RectI contentClipRect = getContentClipRect();
		RectI textBounds = mTextSprite->getBounds(Vector2I(), RectI());

		Vector2I textOffset;
		RectI textClipRect;

		Vector2I imageOffset;
		RectI imageClipRect;
		if(mContentImageSprite != nullptr)
		{
			RectI imageBounds = mContentImageSprite->getBounds(Vector2I(), RectI());
			UINT32 freeWidth = (UINT32)std::max(0, contentBounds.width - textBounds.width - imageBounds.width);
			INT32 imageXOffset = (INT32)(freeWidth / 2);

			if(mStyle->imagePosition == GUIImagePosition::Right)
			{
				INT32 imageReservedWidth = std::max(0, contentBounds.width - textBounds.width);

				textOffset = Vector2I(contentBounds.x, contentBounds.y);
				textClipRect = contentClipRect;
				textClipRect.width = std::min(contentBounds.width - imageReservedWidth, textClipRect.width);

				imageOffset = Vector2I(contentBounds.x + textBounds.width + imageXOffset, contentBounds.y);
				imageClipRect = contentClipRect;
				imageClipRect.x -= textBounds.width + imageXOffset;
			}
			else
			{
				INT32 imageReservedWidth = imageBounds.width + imageXOffset;

				imageOffset = Vector2I(contentBounds.x + imageXOffset, contentBounds.y);
				imageClipRect = contentClipRect;
				imageClipRect.x -= imageXOffset;
				imageClipRect.width = std::min(imageReservedWidth, imageClipRect.width);

				textOffset = Vector2I(contentBounds.x + imageReservedWidth, contentBounds.y);
				textClipRect = contentClipRect;
				textClipRect.x -= imageReservedWidth;
			}

			INT32 imageYOffset = (contentBounds.height - imageBounds.height) / 2;
			imageClipRect.y -= imageYOffset;
			imageOffset.y += imageYOffset;
		}
		else
		{
			textOffset = Vector2I(contentBounds.x, contentBounds.y);
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

	bool GUIButtonBase::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseOver)
		{
			setState(_isOn() ? GUIButtonState::HoverOn : GUIButtonState::Hover);

			if(!onHover.empty())
				onHover();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseOut)
		{
			setState(_isOn() ? GUIButtonState::NormalOn : GUIButtonState::Normal);

			if(!onOut.empty())
				onOut();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDown)
		{
			setState(_isOn() ? GUIButtonState::ActiveOn : GUIButtonState::Active);

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseUp)
		{
			setState(_isOn() ? GUIButtonState::HoverOn : GUIButtonState::Hover);

			if(!onClick.empty())
				onClick();

			return true;
		}

		return false;
	}

	TEXT_SPRITE_DESC GUIButtonBase::getTextDesc() const
	{
		TEXT_SPRITE_DESC textDesc;
		textDesc.text = mContent.getText();
		textDesc.font = mStyle->font;
		textDesc.fontSize = mStyle->fontSize;

		RectI textBounds = getContentBounds();

		textDesc.width = textBounds.width;
		textDesc.height = textBounds.height;
		textDesc.horzAlign = mStyle->textHorzAlign;
		textDesc.vertAlign = mStyle->textVertAlign;

		return textDesc;
	}

	void GUIButtonBase::setState(GUIButtonState state)
	{
		switch(state)
		{
		case GUIButtonState::Normal:
			mActiveTexture = mStyle->normal.texture;
			break;
		case GUIButtonState::Hover:
			mActiveTexture = mStyle->hover.texture;
			break;
		case GUIButtonState::Active:
			mActiveTexture = mStyle->active.texture;
			break;
		case GUIButtonState::Focused:
			mActiveTexture = mStyle->focused.texture;
			break;
		case GUIButtonState::NormalOn:
			mActiveTexture = mStyle->normalOn.texture;
			break;
		case GUIButtonState::HoverOn:
			mActiveTexture = mStyle->hoverOn.texture;
			break;
		case GUIButtonState::ActiveOn:
			mActiveTexture = mStyle->activeOn.texture;
			break;
		case GUIButtonState::FocusedOn:
			mActiveTexture = mStyle->focusedOn.texture;
			break;
		}

		markContentAsDirty();
		mActiveState = state;
	}
}
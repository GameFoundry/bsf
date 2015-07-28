#include "BsGUIButtonBase.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUIDimensions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIHelper.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	GUIButtonBase::GUIButtonBase(const String& styleName, const GUIContent& content, const GUIDimensions& dimensions)
		:GUIElement(styleName, dimensions), mContent(content), mContentImageSprite(nullptr), mActiveState(GUIButtonState::Normal)
	{
		mImageSprite = bs_new<ImageSprite, PoolAlloc>();
		mTextSprite = bs_new<TextSprite, PoolAlloc>();

		HSpriteTexture contentTex = content.getImage();
		if(SpriteTexture::checkIsLoaded(contentTex))
			mContentImageSprite = bs_new<ImageSprite, PoolAlloc>();

		mLocStringUpdatedConn = mContent.getText().addOnStringModifiedCallback(std::bind(&GUIButtonBase::_markContentAsDirty, this));
	}

	GUIButtonBase::~GUIButtonBase()
	{
		mLocStringUpdatedConn.disconnect();

		bs_delete<PoolAlloc>(mTextSprite);
		bs_delete<PoolAlloc>(mImageSprite);

		if(mContentImageSprite != nullptr)
			bs_delete<PoolAlloc>(mContentImageSprite);
	}

	void GUIButtonBase::setContent(const GUIContent& content)
	{
		mLocStringUpdatedConn.disconnect();
		mLocStringUpdatedConn = content.getText().addOnStringModifiedCallback(std::bind(&GUIButtonBase::_markContentAsDirty, this));

		mContent = content;

		_markContentAsDirty();
	}

	void GUIButtonBase::setTint(const Color& color)
	{
		mColor = color;

		_markContentAsDirty();
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

	UINT32 GUIButtonBase::_getNumRenderElements() const
	{
		UINT32 numElements = mImageSprite->getNumRenderElements();
		numElements += mTextSprite->getNumRenderElements();

		if(mContentImageSprite != nullptr)
			numElements += mContentImageSprite->getNumRenderElements();

		return numElements;
	}

	const GUIMaterialInfo& GUIButtonBase::_getMaterial(UINT32 renderElementIdx) const
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

	UINT32 GUIButtonBase::_getNumQuads(UINT32 renderElementIdx) const
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
		mImageDesc.width = mLayoutData.area.width;
		mImageDesc.height = mLayoutData.area.height;

		const HSpriteTexture& activeTex = getActiveTexture();
		if(SpriteTexture::checkIsLoaded(activeTex))
		{
			mImageDesc.texture = activeTex.getInternalPtr();
		}

		mImageDesc.borderLeft = _getStyle()->border.left;
		mImageDesc.borderRight = _getStyle()->border.right;
		mImageDesc.borderTop = _getStyle()->border.top;
		mImageDesc.borderBottom = _getStyle()->border.bottom;
		mImageDesc.color = mColor;

		mImageSprite->update(mImageDesc, (UINT64)_getParentWidget());

		mTextSprite->update(getTextDesc(), (UINT64)_getParentWidget());

		if(mContentImageSprite != nullptr)
		{
			IMAGE_SPRITE_DESC contentImgDesc;
			contentImgDesc.texture = mContent.getImage().getInternalPtr();
			contentImgDesc.width = std::min(mImageDesc.width, mContent.getImage()->getWidth());
			contentImgDesc.height = std::min(mImageDesc.height, mContent.getImage()->getHeight());
			contentImgDesc.color = mColor;

			mContentImageSprite->update(contentImgDesc, (UINT64)_getParentWidget());
		}

		GUIElement::updateRenderElementsInternal();
	}

	void GUIButtonBase::updateClippedBounds()
	{
		mClippedBounds = mLayoutData.area;
		mClippedBounds.clip(mLayoutData.clipRect);
	}

	Vector2I GUIButtonBase::_getOptimalSize() const
	{
		UINT32 imageWidth = 0;
		UINT32 imageHeight = 0;

		const HSpriteTexture& activeTex = getActiveTexture();
		if(SpriteTexture::checkIsLoaded(activeTex))
		{
			imageWidth = activeTex->getWidth();
			imageHeight = activeTex->getHeight();
		}

		Vector2I contentSize = GUIHelper::calcOptimalContentsSize(mContent, *_getStyle(), _getDimensions());
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

	UINT32 GUIButtonBase::_getRenderElementDepthRange() const
	{
		return 2;
	}

	void GUIButtonBase::_fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		UINT32 textSpriteIdx = mImageSprite->getNumRenderElements();
		UINT32 contentImgSpriteIdx = textSpriteIdx + mTextSprite->getNumRenderElements();

		if(renderElementIdx < textSpriteIdx)
		{
			Vector2I offset(mLayoutData.area.x, mLayoutData.area.y);

			mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
				vertexStride, indexStride, renderElementIdx, offset, mLayoutData.getLocalClipRect());

			return;
		}

		Rect2I contentBounds = getCachedContentBounds();
		Rect2I contentClipRect = getCachedContentClipRect();
		Rect2I textBounds = mTextSprite->getBounds(Vector2I(), Rect2I());

		Vector2I textOffset;
		Rect2I textClipRect;

		Vector2I imageOffset;
		Rect2I imageClipRect;
		if(mContentImageSprite != nullptr)
		{
			Rect2I imageBounds = mContentImageSprite->getBounds(Vector2I(), Rect2I());
			UINT32 freeWidth = (UINT32)std::max(0, contentBounds.width - textBounds.width - imageBounds.width);
			INT32 imageXOffset = (INT32)(freeWidth / 2);

			if(_getStyle()->imagePosition == GUIImagePosition::Right)
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

	bool GUIButtonBase::_mouseEvent(const GUIMouseEvent& ev)
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
		else if (ev.getType() == GUIMouseEventType::MouseDoubleClick)
		{
			if (!onDoubleClick.empty())
				onDoubleClick();
		}

		return false;
	}

	TEXT_SPRITE_DESC GUIButtonBase::getTextDesc() const
	{
		TEXT_SPRITE_DESC textDesc;
		textDesc.text = mContent.getText();
		textDesc.font = _getStyle()->font;
		textDesc.fontSize = _getStyle()->fontSize;
		textDesc.color = mColor * getActiveTextColor();

		Rect2I textBounds = getCachedContentBounds();

		textDesc.width = textBounds.width;
		textDesc.height = textBounds.height;
		textDesc.horzAlign = _getStyle()->textHorzAlign;
		textDesc.vertAlign = _getStyle()->textVertAlign;

		return textDesc;
	}

	void GUIButtonBase::setState(GUIButtonState state)
	{
		mActiveState = state;

		_markContentAsDirty();
	}

	const HSpriteTexture& GUIButtonBase::getActiveTexture() const
	{
		switch(mActiveState)
		{
		case GUIButtonState::Normal:
			return _getStyle()->normal.texture;
		case GUIButtonState::Hover:
			return _getStyle()->hover.texture;
		case GUIButtonState::Active:
			return _getStyle()->active.texture;
		case GUIButtonState::Focused:
			return _getStyle()->focused.texture;
		case GUIButtonState::NormalOn:
			return _getStyle()->normalOn.texture;
		case GUIButtonState::HoverOn:
			return _getStyle()->hoverOn.texture;
		case GUIButtonState::ActiveOn:
			return _getStyle()->activeOn.texture;
		case GUIButtonState::FocusedOn:
			return _getStyle()->focusedOn.texture;
		}

		return _getStyle()->normal.texture;
	}

	Color GUIButtonBase::getActiveTextColor() const
	{
		switch (mActiveState)
		{
		case GUIButtonState::Normal:
			return _getStyle()->normal.textColor;
		case GUIButtonState::Hover:
			return _getStyle()->hover.textColor;
		case GUIButtonState::Active:
			return _getStyle()->active.textColor;
		case GUIButtonState::Focused:
			return _getStyle()->focused.textColor;
		case GUIButtonState::NormalOn:
			return _getStyle()->normalOn.textColor;
		case GUIButtonState::HoverOn:
			return _getStyle()->hoverOn.textColor;
		case GUIButtonState::ActiveOn:
			return _getStyle()->activeOn.textColor;
		case GUIButtonState::FocusedOn:
			return _getStyle()->focusedOn.textColor;
		}

		return _getStyle()->normal.textColor;
	}
}
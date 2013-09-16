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
		:GUIElement(parent, style, layoutOptions), mContent(content), mNumImageRenderElements(0)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();
		mTextSprite = cm_new<TextSprite, PoolAlloc>();

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
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIButton, PoolAlloc>()) GUIButton(parent, style, content, getDefaultLayoutOptions(style));
	}

	GUIButton* GUIButton::create(GUIWidget& parent, const GUIContent& content, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
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

		return numElements;
	}

	const HMaterial& GUIButton::getMaterial(UINT32 renderElementIdx) const
	{
		if(renderElementIdx >= mNumImageRenderElements)
			return mTextSprite->getMaterial(mNumImageRenderElements - renderElementIdx);
		else
			return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUIButton::getNumQuads(UINT32 renderElementIdx) const
	{
		UINT32 numQuads = 0;
		if(renderElementIdx >= mNumImageRenderElements)
			numQuads = mTextSprite->getNumQuads(mNumImageRenderElements - renderElementIdx);
		else
			numQuads = mImageSprite->getNumQuads(renderElementIdx);

		return numQuads;
	}

	void GUIButton::updateRenderElementsInternal()
	{		
		mImageDesc.width = mWidth;
		mImageDesc.height = mHeight;

		mImageSprite->update(mImageDesc);
		mNumImageRenderElements = mImageSprite->getNumRenderElements();

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
		if(renderElementIdx >= mNumImageRenderElements)
			return _getDepth();
		else
			return _getDepth() + 1;
	}

	void GUIButton::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		if(renderElementIdx >= mNumImageRenderElements)
		{
			Rect textBounds = getContentBounds();
			Int2 offset(textBounds.x, textBounds.y);
			Rect textClipRect = getContentClipRect();

			mTextSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
			vertexStride, indexStride, mNumImageRenderElements - renderElementIdx, offset, textClipRect);
		}
		else
		{
			mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
			vertexStride, indexStride, renderElementIdx, mOffset, mClipRect);
		}
	}

	bool GUIButton::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseOver)
		{
			mImageDesc.texture = mStyle->hover.texture;
			markContentAsDirty();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseOut)
		{
			mImageDesc.texture = mStyle->normal.texture;
			markContentAsDirty();

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
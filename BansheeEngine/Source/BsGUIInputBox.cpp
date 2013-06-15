#include "BsGUIInputBox.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIKeyEvent.h"
#include "BsGUIMouseEvent.h"
#include "CmTexture.h"
#include "CmCursor.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIInputBox::getGUITypeName()
	{
		static String name = "InputBox";
		return name;
	}

	GUIInputBox::GUIInputBox(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mNumImageRenderElements(0), mInputCursorSet(false), mDragInProgress(false)
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

	GUIInputBox::~GUIInputBox()
	{
		cm_delete<PoolAlloc>(mTextSprite);
		cm_delete<PoolAlloc>(mImageSprite);
	}

	GUIInputBox* GUIInputBox::create(GUIWidget& parent, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIInputBox, PoolAlloc>()) GUIInputBox(parent, style, getDefaultLayoutOptions(style));
	}

	GUIInputBox* GUIInputBox::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIInputBox, PoolAlloc>()) GUIInputBox(parent, style, layoutOptions);
	}

	UINT32 GUIInputBox::getNumRenderElements() const
	{
		UINT32 numElements = mImageSprite->getNumRenderElements();
		numElements += mTextSprite->getNumRenderElements();

		return numElements;
	}

	const HMaterial& GUIInputBox::getMaterial(UINT32 renderElementIdx) const
	{
		if(renderElementIdx >= mNumImageRenderElements)
			return mTextSprite->getMaterial(mNumImageRenderElements - renderElementIdx);
		else
			return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUIInputBox::getNumQuads(UINT32 renderElementIdx) const
	{
		UINT32 numQuads = 0;
		if(renderElementIdx >= mNumImageRenderElements)
			numQuads = mTextSprite->getNumQuads(mNumImageRenderElements - renderElementIdx);
		else
			numQuads = mImageSprite->getNumQuads(renderElementIdx);

		return numQuads;
	}

	void GUIInputBox::updateRenderElementsInternal()
	{		
		mImageDesc.offset = mOffset;
		mImageDesc.width = mWidth;
		mImageDesc.height = mHeight;
		mImageDesc.clipRect = mClipRect;

		mImageSprite->update(mImageDesc);
		mBounds = mImageSprite->getBounds();
		mNumImageRenderElements = mImageSprite->getNumRenderElements();

		TEXT_SPRITE_DESC textDesc;
		textDesc.text = mText;
		textDesc.font = mStyle->font;
		textDesc.fontSize = mStyle->fontSize;

		Rect contentBounds = mBounds;

		contentBounds.x += mStyle->margins.left + mStyle->contentOffset.left;
		contentBounds.y += mStyle->margins.top + mStyle->contentOffset.top;
		contentBounds.width = (UINT32)std::max(0, (INT32)contentBounds.width - 
			(INT32)(mStyle->margins.left + mStyle->margins.right + mStyle->contentOffset.left + mStyle->contentOffset.right));
		contentBounds.height = (UINT32)std::max(0, (INT32)contentBounds.height - 
			(INT32)(mStyle->margins.top + mStyle->margins.bottom + mStyle->contentOffset.top + mStyle->contentOffset.bottom));

		textDesc.offset = Int2(contentBounds.x, contentBounds.y);
		textDesc.width = contentBounds.width;
		textDesc.height = contentBounds.height;
		textDesc.clipRect = Rect(0, 0, textDesc.width, textDesc.height);
		textDesc.horzAlign = mStyle->textHorzAlign;
		textDesc.vertAlign = mStyle->textVertAlign;

		mTextSprite->update(textDesc);
	}

	UINT32 GUIInputBox::_getOptimalWidth() const
	{
		if(mImageDesc.texture != nullptr)
		{
			return mImageDesc.texture->getTexture()->getWidth();
		}

		return 0;
	}

	UINT32 GUIInputBox::_getOptimalHeight() const
	{
		if(mImageDesc.texture != nullptr)
		{
			return mImageDesc.texture->getTexture()->getHeight();
		}

		return 0;
	}

	UINT32 GUIInputBox::_getRenderElementDepth(UINT32 renderElementIdx) const
	{
		if(renderElementIdx >= mNumImageRenderElements)
			return _getDepth();
		else
			return _getDepth() + 1;
	}

	void GUIInputBox::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		if(renderElementIdx >= mNumImageRenderElements)
			mTextSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, mNumImageRenderElements - renderElementIdx);
		else
			mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx);
	}

	bool GUIInputBox::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseOver)
		{
			mImageDesc.texture = mStyle->hover.texture;
			markAsDirty();

			if(!mInputCursorSet)
			{
				Cursor::setCursor(CursorType::IBeam);
				mInputCursorSet = true;
			}

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseOut)
		{
			mImageDesc.texture = mStyle->normal.texture;
			markAsDirty();

			if(!mDragInProgress && mInputCursorSet)
			{
				Cursor::setCursor(CursorType::Arrow);
				mInputCursorSet = false;
			}

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDown)
		{
			mImageDesc.texture = mStyle->active.texture;
			markAsDirty();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseUp)
		{
			mImageDesc.texture = mStyle->hover.texture;
			markAsDirty();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDragStart)
		{
			mDragInProgress = true;

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDragEnd)
		{
			mDragInProgress = false;

			if(ev.getMouseOverElement() != this && mInputCursorSet)
			{
				Cursor::setCursor(CursorType::Arrow);
				mInputCursorSet = false;
			}

			return true;
		}

		return false;
	}

	bool GUIInputBox::keyEvent(const GUIKeyEvent& ev)
	{
		if(ev.getType() == GUIKeyEventType::KeyDown)
		{
			if(ev.getKey() == BC_BACK)
			{
				if(mText.size() > 0)
				{
					mText.erase(mText.end() - 1);
					markAsDirty();
				}

				return true;
			}
		}
		else if(ev.getType() == GUIKeyEventType::TextInput)
		{
			// TODO - How are backspace, caps and enter handled?
			mText += ev.getInputChar();

			markAsDirty();

			return true;
		}

		return false;
	}

	void GUIInputBox::_setFocus(bool focus)
	{
		if(focus)
		{
			mImageDesc.texture = mStyle->focused.texture;
			markAsDirty();
		}
		else
		{
			mImageDesc.texture = mStyle->normal.texture;
			markAsDirty();
		}
	}
}
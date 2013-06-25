#include "BsGUIInputBox.h"
#include "BsGUIManager.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIButtonEvent.h"
#include "BsGUIMouseEvent.h"
#include "BsGUICommandEvent.h"
#include "CmTextUtility.h"
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
		:GUIElement(parent, style, layoutOptions), mNumImageRenderElements(0), mInputCursorSet(false), mDragInProgress(false),
		mSelectionStart(0), mSelectionEnd(0), mCaretSprite(nullptr), mCaretShown(false), mSelectionShown(false), mCaretPos(0),
		mIsMultiline(false)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();
		mCaretSprite = cm_new<ImageSprite, PoolAlloc>();
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
		cm_delete<PoolAlloc>(mCaretSprite);
		cm_delete<PoolAlloc>(mImageSprite);

		for(auto& sprite : mSelectionSprites)
			cm_delete(sprite);
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

		if(mCaretShown && GUIManager::instance().getCaretBlinkState())
			numElements += mTextSprite->getNumRenderElements();

		if(mSelectionShown)
		{
			for(auto& selectionSprite : mSelectionSprites)
			{
				numElements += selectionSprite->getNumRenderElements();
			}
		}

		return numElements;
	}

	const HMaterial& GUIInputBox::getMaterial(UINT32 renderElementIdx) const
	{
		UINT32 localRenderElementIdx;
		Sprite* sprite = renderElemToSprite(renderElementIdx, localRenderElementIdx);

		return sprite->getMaterial(localRenderElementIdx);
	}

	UINT32 GUIInputBox::getNumQuads(UINT32 renderElementIdx) const
	{
		UINT32 localRenderElementIdx;
		Sprite* sprite = renderElemToSprite(renderElementIdx, localRenderElementIdx);

		return sprite->getNumQuads(localRenderElementIdx);
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

		Rect textBounds = getTextBounds();
		textDesc.offset = Int2(textBounds.x, textBounds.y);
		textDesc.width = textBounds.width;
		textDesc.height = textBounds.height;
		textDesc.clipRect = Rect(0, 0, textDesc.width, textDesc.height);
		textDesc.horzAlign = mStyle->textHorzAlign;
		textDesc.vertAlign = mStyle->textVertAlign;

		mTextSprite->update(textDesc);

		if(mCaretShown && GUIManager::instance().getCaretBlinkState())
		{
			IMAGE_SPRITE_DESC mCaretDesc;
			mCaretDesc.offset = getCaretPosition();
			mCaretDesc.width = 1;
			mCaretDesc.height = getCaretHeight();
			mCaretDesc.clipRect = Rect(0, 0, textDesc.width, textDesc.height);
			mCaretDesc.texture = GUIManager::instance().getCaretTexture();

			mCaretSprite->update(mCaretDesc);
		}

		if(mSelectionShown)
		{
			Vector<Rect>::type selectionRects = getSelectionRects();

			INT32 diff = (INT32)(mSelectionSprites.size() - selectionRects.size());

			if(diff > 0)
			{
				for(INT32 i = 0; i < diff; i++)
					cm_delete(mSelectionSprites[i]);

				mSelectionSprites.erase(mSelectionSprites.begin() + selectionRects.size(), mSelectionSprites.end());
			}
			else if(diff < 0)
			{
				for(INT32 i = diff; i < 0; i++)
				{
					ImageSprite* newSprite = cm_new<ImageSprite>();
					mSelectionSprites.push_back(newSprite);
				}
			}

			UINT32 idx = 0;
			for(auto& sprite : mSelectionSprites)
			{
				IMAGE_SPRITE_DESC desc;
				desc.offset = Int2(selectionRects[idx].x, selectionRects[idx].y);
				desc.width = selectionRects[idx].width;
				desc.height = selectionRects[idx].height;
				desc.clipRect = Rect(0, 0, textDesc.width, textDesc.height);
				desc.texture = GUIManager::instance().getTextSelectionTexture();

				sprite->update(desc);
				idx++;
			}
		}
	}

	Sprite* GUIInputBox::renderElemToSprite(UINT32 renderElemIdx, UINT32& localRenderElemIdx) const
	{
		UINT32 oldNumElements = 0;
		UINT32 newNumElements = oldNumElements + mTextSprite->getNumRenderElements();
		if(renderElemIdx < newNumElements)
		{
			localRenderElemIdx = renderElemIdx - oldNumElements;
			return mTextSprite;
		}

		oldNumElements = newNumElements;
		newNumElements += mImageSprite->getNumRenderElements();

		if(renderElemIdx < newNumElements)
		{
			localRenderElemIdx = renderElemIdx - oldNumElements;
			return mImageSprite;
		}

		if(mCaretShown && GUIManager::instance().getCaretBlinkState())
		{
			oldNumElements = newNumElements;
			newNumElements += mImageSprite->getNumRenderElements();

			if(renderElemIdx < newNumElements)
			{
				localRenderElemIdx = renderElemIdx - oldNumElements;
				return mImageSprite;
			}
		}

		if(mSelectionShown)
		{
			for(auto& selectionSprite : mSelectionSprites)
			{
				oldNumElements = newNumElements;
				newNumElements += selectionSprite->getNumRenderElements();

				if(renderElemIdx < newNumElements)
				{
					localRenderElemIdx = renderElemIdx - oldNumElements;
					return selectionSprite;
				}
			}
		}

		localRenderElemIdx = renderElemIdx;
		return nullptr;
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
		UINT32 localRenderElementIdx;
		Sprite* sprite = renderElemToSprite(renderElementIdx, localRenderElementIdx);

		if(sprite == mImageSprite)
			return _getDepth();
		else if(sprite == mTextSprite || sprite == mCaretSprite)
			return _getDepth() + 2;
		else // Selection sprites
			return _getDepth() + 1;
	}

	void GUIInputBox::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		UINT32 localRenderElementIdx;
		Sprite* sprite = renderElemToSprite(renderElementIdx, localRenderElementIdx);

		sprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, localRenderElementIdx);
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
			showCaret(getCharAtPosition(ev.getPosition()));
			clearSelection();
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
		else if(ev.getType() == GUIMouseEventType::MouseDrag)
		{
			// TODO - Update selection
			//  - If mouse is over control, place selection marker there (make sure start < end)
			//  - Else move the selection by a certain amount of pixels depending on drag amount
			markAsDirty();
		}

		return false;
	}

	bool GUIInputBox::buttonEvent(const GUIButtonEvent& ev)
	{
		if(ev.getType() == GUIKeyEventType::KeyDown)
		{
			if(ev.getKey() == BC_BACK)
			{
				if(mText.size() > 0)
				{
					if(mSelectionShown)
					{
						mText.erase(mSelectionStart, mSelectionEnd);
						mCaretPos = mSelectionStart;
						clearSelection();
					}
					else
					{
						mText.erase(mCaretPos);
					}

					markAsDirty();
				}

				return true;
			}

			// TODO - Handle newline if it's a multiline control
			// TODO - left+right arrow to move the cursor
			// TODO - shift + left + right arrow to select
			// TODO - ctrl + a to select all
			// TODO - ctrl + c, ctrl + v, ctrl + x to copy/cut/paste
		}
		else if(ev.getType() == GUIKeyEventType::TextInput)
		{
			if(mSelectionShown)
			{
				mText.erase(mSelectionStart, mSelectionEnd);
				mCaretPos = mSelectionStart;
				clearSelection();
			}

			mText.insert(mText.begin() + mCaretPos, ev.getInputChar());
			mCaretPos++;

			markAsDirty();

			return true;
		}

		return false;
	}

	bool GUIInputBox::commandEvent(const GUICommandEvent& ev)
	{
		if(ev.getType() == GUICommandEventType::Redraw)
		{
			markAsDirty();
			return true;
		}

		return false;
	}

	void GUIInputBox::showCaret(CM::UINT32 charIdx)
	{
		mCaretPos = charIdx;
		mCaretShown = true;
		markAsDirty();
	}

	void GUIInputBox::clearCaret()
	{
		mCaretShown = false;
		markAsDirty();
	}

	Int2 GUIInputBox::getCaretPosition() const
	{
		// TODO
		return Int2(0, 0);
	}

	UINT32 GUIInputBox::getCaretHeight() const
	{
		// TODO
		return 8;
	}

	void GUIInputBox::showSelection(UINT32 startChar, UINT32 endChar)
	{
		mSelectionStart = startChar;
		mSelectionEnd = endChar;
		mSelectionShown = true;
		markAsDirty();
	}

	void GUIInputBox::clearSelection()
	{
		for(auto& sprite : mSelectionSprites)
			cm_delete(sprite);
		
		mSelectionSprites.clear();
		mSelectionShown = false;
		markAsDirty();
	}

	Vector<Rect>::type GUIInputBox::getSelectionRects() const
	{
		// TODO
		return Vector<Rect>::type();
	}

	UINT32 GUIInputBox::getCharAtPosition(const Int2& pos) const
	{
		Rect textBounds = getTextBounds();

		//std::shared_ptr<TextUtility::TextData> textData = 
		//	TextUtility::getTextData(mText, mStyle->font, mStyle->fontSize, 
		//	textBounds.width, textBounds.height, mIsMultiline);

		//textData->getLines()

		// TODO
		return 0;
	}

	CM::Rect GUIInputBox::getTextBounds() const
	{
		Rect textBounds = mBounds;

		textBounds.x += mStyle->margins.left + mStyle->contentOffset.left;
		textBounds.y += mStyle->margins.top + mStyle->contentOffset.top;
		textBounds.width = (UINT32)std::max(0, (INT32)textBounds.width - 
			(INT32)(mStyle->margins.left + mStyle->margins.right + mStyle->contentOffset.left + mStyle->contentOffset.right));
		textBounds.height = (UINT32)std::max(0, (INT32)textBounds.height - 
			(INT32)(mStyle->margins.top + mStyle->margins.bottom + mStyle->contentOffset.top + mStyle->contentOffset.bottom));


		return textBounds;
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
			clearCaret();
			clearSelection();
			markAsDirty();
		}
	}
}
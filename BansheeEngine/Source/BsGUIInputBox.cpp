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
#include "CmFont.h"
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

	GUIInputBox::GUIInputBox(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions, bool multiline)
		:GUIElement(parent, style, layoutOptions), mInputCursorSet(false), mDragInProgress(false),
		mInputCaret(nullptr), mInputSelection(nullptr), mCaretShown(false), mSelectionShown(false), mIsMultiline(multiline)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();
		mTextSprite = cm_new<TextSprite, PoolAlloc>();
		mInputCaret = cm_new<GUIInputCaret, PoolAlloc>(getTextDesc(), getTextOffset(), mTextOffset);
		mInputSelection = cm_new<GUIInputSelection, PoolAlloc>(getTextDesc(), getTextOffset(), mTextOffset);

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

		cm_delete<PoolAlloc>(mInputCaret);
		cm_delete<PoolAlloc>(mInputSelection);
	}

	GUIInputBox* GUIInputBox::create(GUIWidget& parent, bool multiline, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIInputBox, PoolAlloc>()) GUIInputBox(parent, style, getDefaultLayoutOptions(style), multiline);
	}

	GUIInputBox* GUIInputBox::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, bool multiline, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIInputBox, PoolAlloc>()) GUIInputBox(parent, style, layoutOptions, multiline);
	}

	UINT32 GUIInputBox::getNumRenderElements() const
	{
		UINT32 numElements = mImageSprite->getNumRenderElements();
		numElements += mTextSprite->getNumRenderElements();

		if(mCaretShown && GUIManager::instance().getCaretBlinkState())
			numElements += mInputCaret->getSprite()->getNumRenderElements();

		if(mSelectionShown)
		{
			const Vector<ImageSprite*>::type& sprites = mInputSelection->getSprites();
			for(auto& selectionSprite : sprites)
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
		mImageDesc.width = mWidth;
		mImageDesc.height = mHeight;

		mImageSprite->update(mImageDesc);
		mBounds = mImageSprite->getBounds(mOffset, mClipRect);

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		mTextSprite->update(textDesc);

		if(mCaretShown && GUIManager::instance().getCaretBlinkState())
		{
			mInputCaret->updateText(textDesc, getTextOffset(), mTextOffset); // TODO - These shouldn't be here. Only call this when one of these parameters changes.
			mInputCaret->updateSprite();
		}

		if(mSelectionShown)
		{
			mInputSelection->updateText(textDesc, getTextOffset(), mTextOffset); // TODO - These shouldn't be here. Only call this when one of these parameters changes.
			mInputSelection->updateSprite();
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
			newNumElements += mInputCaret->getSprite()->getNumRenderElements();

			if(renderElemIdx < newNumElements)
			{
				localRenderElemIdx = renderElemIdx - oldNumElements;
				return mInputCaret->getSprite();
			}
		}

		if(mSelectionShown)
		{
			const Vector<ImageSprite*>::type& sprites = mInputSelection->getSprites();
			for(auto& selectionSprite : sprites)
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

	Int2 GUIInputBox::renderElemToOffset(UINT32 renderElemIdx) const
	{
		UINT32 oldNumElements = 0;
		UINT32 newNumElements = oldNumElements + mTextSprite->getNumRenderElements();
		if(renderElemIdx < newNumElements)
			return getTextOffset();

		oldNumElements = newNumElements;
		newNumElements += mImageSprite->getNumRenderElements();

		if(renderElemIdx < newNumElements)
			return mOffset;

		if(mCaretShown && GUIManager::instance().getCaretBlinkState())
		{
			oldNumElements = newNumElements;
			newNumElements += mInputCaret->getSprite()->getNumRenderElements();

			if(renderElemIdx < newNumElements)
				return mInputCaret->getSpriteOffset();
		}

		if(mSelectionShown)
		{
			UINT32 spriteIdx = 0;
			const Vector<ImageSprite*>::type& sprites = mInputSelection->getSprites();
			for(auto& selectionSprite : sprites)
			{
				oldNumElements = newNumElements;
				newNumElements += selectionSprite->getNumRenderElements();

				if(renderElemIdx < newNumElements)
					return mInputSelection->getSelectionSpriteOffset(spriteIdx);

				spriteIdx++;
			}
		}

		return Int2();
	}

	Rect GUIInputBox::renderElemToClipRect(UINT32 renderElemIdx) const
	{
		UINT32 oldNumElements = 0;
		UINT32 newNumElements = oldNumElements + mTextSprite->getNumRenderElements();
		if(renderElemIdx < newNumElements)
			return getTextClipRect();

		oldNumElements = newNumElements;
		newNumElements += mImageSprite->getNumRenderElements();

		if(renderElemIdx < newNumElements)
			return mClipRect;

		if(mCaretShown && GUIManager::instance().getCaretBlinkState())
		{
			oldNumElements = newNumElements;
			newNumElements += mInputCaret->getSprite()->getNumRenderElements();

			if(renderElemIdx < newNumElements)
				return mInputCaret->getSpriteClipRect();
		}

		if(mSelectionShown)
		{
			UINT32 spriteIdx = 0;
			const Vector<ImageSprite*>::type& sprites = mInputSelection->getSprites();
			for(auto& selectionSprite : sprites)
			{
				oldNumElements = newNumElements;
				newNumElements += selectionSprite->getNumRenderElements();

				if(renderElemIdx < newNumElements)
					return mInputSelection->getSelectionSpriteClipRect(spriteIdx);

				spriteIdx++;
			}
		}

		return Rect();
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
		else if(sprite == mTextSprite)
			return _getDepth() - 2;
		else if(sprite == mInputCaret->getSprite())
			return _getDepth() - 3;
		else // Selection sprites
			return _getDepth() - 1;
	}

	void GUIInputBox::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		UINT32 localRenderElementIdx;
		Sprite* sprite = renderElemToSprite(renderElementIdx, localRenderElementIdx);
		Int2 offset = renderElemToOffset(renderElementIdx);
		Rect clipRect = renderElemToClipRect(renderElementIdx);

		sprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, localRenderElementIdx, offset, clipRect);
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

			showCaret();

			if(mText.size() > 0)
				mInputCaret->moveCaretToPos(ev.getPosition());
			else
				mInputCaret->moveCaretToStart();

			scrollTextToCaret();

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

			UINT32 caretPos = mInputCaret->getCaretPos();
			showSelection(caretPos);
			mInputSelection->selectionDragStart(caretPos);

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

			mInputSelection->selectionDragEnd();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDrag)
		{
			Rect bounds = getTextBounds();
			if(mText.size() > 0)
				mInputCaret->moveCaretToPos(ev.getPosition());
			else
				mInputCaret->moveCaretToStart();

			mInputSelection->selectionDragUpdate(mInputCaret->getCaretPos());

			scrollTextToCaret();

			markAsDirty();
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
					if(mSelectionShown)
					{
						deleteSelectedText();
					}
					else
					{
						UINT32 charIdx = mInputCaret->getCharIdxAtCaretPos() - 1;

						if(charIdx < (UINT32)mText.size())
						{
							eraseChar(charIdx);

							if(charIdx > 0)
								charIdx--;

							mInputCaret->moveCaretToChar(charIdx, CARET_AFTER);

							scrollTextToCaret();
						}
					}

					markAsDirty();
				}

				return true;
			}

			if(ev.getKey() == BC_DELETE)
			{
				if(mText.size() > 0)
				{
					if(mSelectionShown)
					{
						deleteSelectedText();
					}
					else
					{
						UINT32 charIdx = mInputCaret->getCharIdxAtCaretPos();
						if(charIdx < (UINT32)mText.size())
						{
							eraseChar(charIdx);

							if(charIdx > 0)
								charIdx--;

							mInputCaret->moveCaretToChar(charIdx, CARET_AFTER);

							scrollTextToCaret();
						}
					}

					markAsDirty();
				}

				return true;
			}
			
			if(ev.getKey() == BC_LEFT)
			{
				if(ev.isShiftDown())
				{
					if(!mSelectionShown)
						showSelection(mInputCaret->getCaretPos());
				}
				else
					clearSelection();

				mInputCaret->moveCaretLeft();

				if(ev.isShiftDown())
					mInputSelection->moveSelectionToCaret(mInputCaret->getCaretPos());

				scrollTextToCaret();
				markAsDirty();
				return true;
			}

			if(ev.getKey() == BC_RIGHT)
			{
				if(ev.isShiftDown())
				{
					if(!mSelectionShown)
						showSelection(mInputCaret->getCaretPos());
				}
				else
					clearSelection();

				mInputCaret->moveCaretRight();

				if(ev.isShiftDown())
					mInputSelection->moveSelectionToCaret(mInputCaret->getCaretPos());

				scrollTextToCaret();
				markAsDirty();
				return true;
			}

			if(ev.getKey() == BC_UP)
			{
				if(ev.isShiftDown())
				{
					if(!mSelectionShown)
						showSelection(mInputCaret->getCaretPos());
				}
				else
					clearSelection();

				mInputCaret->moveCaretUp();
				
				if(ev.isShiftDown())
					mInputSelection->moveSelectionToCaret(mInputCaret->getCaretPos());

				scrollTextToCaret();
				markAsDirty();
				return true;
			}

			if(ev.getKey() == BC_DOWN)
			{
				if(ev.isShiftDown())
				{
					if(!mSelectionShown)
						showSelection(mInputCaret->getCaretPos());
				}
				else
					clearSelection();

				mInputCaret->moveCaretDown();
				
				if(ev.isShiftDown())
					mInputSelection->moveSelectionToCaret(mInputCaret->getCaretPos());

				scrollTextToCaret();
				markAsDirty();
				return true;
			}

			if(ev.getKey() == BC_RETURN)
			{
				if(mIsMultiline)
				{
					if(mSelectionShown)
						deleteSelectedText();

					insertChar(mInputCaret->getCharIdxAtCaretPos(), '\n');

					mInputCaret->moveCaretRight();
					scrollTextToCaret();

					markAsDirty();
					return true;
				}
				
			}

			if(ev.getKey() == BC_A && ev.isCtrlDown())
			{
				showSelection(0);
				mInputSelection->selectAll();

				markAsDirty();
				return true;
			}
		}
		else if(ev.getType() == GUIKeyEventType::TextInput)
		{
			if(mSelectionShown)
				deleteSelectedText();

			UINT32 charIdx = mInputCaret->getCharIdxAtCaretPos();
			insertChar(charIdx, ev.getInputChar());

			mInputCaret->moveCaretToChar(charIdx, CARET_AFTER);

			scrollTextToCaret();

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

	void GUIInputBox::showCaret()
	{
		mCaretShown = true;
		markAsDirty();
	}


	void GUIInputBox::hideCaret()
	{
		mCaretShown = false;
		markAsDirty();
	}

	void GUIInputBox::showSelection(CM::UINT32 anchorCaretPos)
	{
		mInputSelection->showSelection(anchorCaretPos);
		mSelectionShown = true;
		markAsDirty();
	}

	void GUIInputBox::clearSelection()
	{
		mInputSelection->clearSelection();
		mSelectionShown = false;
		markAsDirty();
	}

	void GUIInputBox::scrollTextToCaret()
	{
		TEXT_SPRITE_DESC textDesc = getTextDesc();

		Int2 textOffset = getTextOffset();
		Int2 caretPos = mInputCaret->getCaretPosition(textOffset);
		UINT32 caretHeight = mInputCaret->getCaretHeight();
		UINT32 caretWidth = 1;
		INT32 caretRight = caretPos.x + (INT32)caretWidth;
		INT32 caretBottom = caretPos.y + (INT32)caretHeight;

		INT32 left = textOffset.x - mTextOffset.x;
		// Include caret width here because we don't want to scroll if just the caret is outside the bounds
		// (Possible if the text width is exactly the maximum width)
		INT32 right = left + (INT32)textDesc.width + caretWidth; 
		INT32 top = textOffset.y - mTextOffset.y;
		INT32 bottom = top + (INT32)textDesc.height;

		Int2 offset;
		if(caretPos.x < left)
		{
			offset.x = left - caretPos.x;
		}
		else if(caretRight > right)
		{
			offset.x = -(caretRight - right);
		}

		if(caretPos.y < top)
		{
			offset.y = top - caretPos.y;
		}
		else if(caretBottom > bottom)
		{
			offset.y = -(caretBottom - bottom);
		}

		mTextOffset += offset;

		Int2 newOffset = getTextOffset();
		mInputCaret->updateText(textDesc, newOffset, mTextOffset);
		mInputSelection->updateText(textDesc, newOffset, mTextOffset);

		markAsDirty();
	}

	void GUIInputBox::insertChar(CM::UINT32 charIdx, CM::UINT32 charCode)
	{
		mText.insert(mText.begin() + charIdx, charCode);

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Int2 offset = getTextOffset();

		mInputCaret->updateText(textDesc, offset, mTextOffset);
		mInputSelection->updateText(textDesc, offset, mTextOffset);
	}

	void GUIInputBox::eraseChar(CM::UINT32 charIdx)
	{
		mText.erase(charIdx, 1);

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Int2 offset = getTextOffset();

		mInputCaret->updateText(textDesc, offset, mTextOffset);
		mInputSelection->updateText(textDesc, offset, mTextOffset);
	}

	void GUIInputBox::deleteSelectedText()
	{
		UINT32 selStart = mInputSelection->getSelectionStart();
		mText.erase(mText.begin() + selStart, mText.begin() + mInputSelection->getSelectionEnd());

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Int2 offset = getTextOffset();
		mInputCaret->updateText(textDesc, offset, mTextOffset);
		mInputSelection->updateText(textDesc, offset, mTextOffset);

		if(selStart > 0)
		{
			UINT32 newCaretPos = selStart - 1;
			mInputCaret->moveCaretToChar(newCaretPos, CARET_AFTER);
		}
		else
		{
			mInputCaret->moveCaretToChar(0, CARET_BEFORE);
		}

		scrollTextToCaret();
		clearSelection();
	}

	CM::Int2 GUIInputBox::getTextOffset() const
	{
		Rect textBounds = getTextBounds();
		return Int2(textBounds.x, textBounds.y) + mTextOffset;
	}

	CM::Rect GUIInputBox::getTextClipRect() const
	{
		Rect textBounds = getTextBounds();
		return Rect(-mTextOffset.x, -mTextOffset.y, textBounds.width, textBounds.height);
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

	TEXT_SPRITE_DESC GUIInputBox::getTextDesc() const
	{
		TEXT_SPRITE_DESC textDesc;
		textDesc.text = mText;
		textDesc.font = mStyle->font;
		textDesc.fontSize = mStyle->fontSize;

		Rect textBounds = getTextBounds();
		textDesc.width = textBounds.width;
		textDesc.height = textBounds.height;
		textDesc.horzAlign = mStyle->textHorzAlign;
		textDesc.vertAlign = mStyle->textVertAlign;
		textDesc.wordWrap = mIsMultiline;

		return textDesc;
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
			hideCaret();
			clearSelection();
			markAsDirty();
		}
	}
}
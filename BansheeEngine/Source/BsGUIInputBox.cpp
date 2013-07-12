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
		mSelectionStart(0), mSelectionEnd(0), mSelectionAnchor(0), mInputCaret(nullptr), mCaretShown(false), 
		mSelectionShown(false), mIsMultiline(multiline), mSelectionDragAnchor(0)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();
		mTextSprite = cm_new<TextSprite, PoolAlloc>();
		mInputCaret = cm_new<GUIInputCaret, PoolAlloc>(getTextDesc(), getTextOffset(), mTextOffset);

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

		for(auto& sprite : mSelectionSprites)
			cm_delete(sprite);
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
		mImageDesc.width = mWidth;
		mImageDesc.height = mHeight;

		mImageSprite->update(mImageDesc);
		mBounds = mImageSprite->getBounds(mOffset, mClipRect);

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		mTextSprite->update(textDesc);

		if(mCaretShown && GUIManager::instance().getCaretBlinkState())
		{
			mInputCaret->updateText(textDesc, getTextOffset(), mTextOffset);
			mInputCaret->updateSprite(mTextOffset);
		}

		if(mSelectionShown)
		{
			Vector<Rect>::type selectionRects = getSelectionRects();

			INT32 diff = (INT32)(mSelectionSprites.size() - selectionRects.size());

			if(diff > 0)
			{
				for(UINT32 i = (UINT32)selectionRects.size(); i < (UINT32)mSelectionSprites.size(); i++)
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
				desc.width = selectionRects[idx].width;
				desc.height = selectionRects[idx].height;
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
			newNumElements += mInputCaret->getSprite()->getNumRenderElements();

			if(renderElemIdx < newNumElements)
			{
				localRenderElemIdx = renderElemIdx - oldNumElements;
				return mInputCaret->getSprite();
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
			for(auto& selectionSprite : mSelectionSprites)
			{
				oldNumElements = newNumElements;
				newNumElements += selectionSprite->getNumRenderElements();

				if(renderElemIdx < newNumElements)
					return getSelectionSpriteOffset(spriteIdx);

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
			for(auto& selectionSprite : mSelectionSprites)
			{
				oldNumElements = newNumElements;
				newNumElements += selectionSprite->getNumRenderElements();

				if(renderElemIdx < newNumElements)
					return getSelectionSpriteClipRect(spriteIdx);

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

			selectionDragStart(mInputCaret->getCaretPos());

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

			selectionDragEnd();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDrag)
		{
			Rect bounds = getTextBounds();
			if(mText.size() > 0)
				mInputCaret->moveCaretToPos(ev.getPosition());
			else
				mInputCaret->moveCaretToStart();

			selectionDragUpdate(mInputCaret->getCaretPos());

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
						UINT32 selStart = getSelectionStart();
						mText.erase(mText.begin() + selStart, mText.begin() + getSelectionEnd());
						mInputCaret->updateText(getTextDesc(), getTextOffset(), mTextOffset);

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
					else
					{
						UINT32 charIdx = mInputCaret->getCharIdxAtCaretPos() - 1;

						if(charIdx < (UINT32)mText.size())
						{
							mText.erase(charIdx, 1);
							mInputCaret->updateText(getTextDesc(), getTextOffset(), mTextOffset);

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
						UINT32 selStart = getSelectionStart();
						mText.erase(mText.begin() + selStart, mText.begin() + getSelectionEnd());
						mInputCaret->updateText(getTextDesc(), getTextOffset(), mTextOffset);

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
					else
					{
						UINT32 charIdx = mInputCaret->getCharIdxAtCaretPos();
						if(charIdx < (UINT32)mText.size())
						{
							mText.erase(charIdx, 1);
							mInputCaret->updateText(getTextDesc(), getTextOffset(), mTextOffset);

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
					bool caretMovedDueToNewline = false;
					if(!mSelectionShown)
					{
						if(isNewlineChar(caretPosToSelectionChar(mInputCaret->getCaretPos(), SelectionDir::Right)))
						{
							mInputCaret->moveCaretLeft();
							caretMovedDueToNewline = true;
						}

						showSelection(mInputCaret->getCaretPos(), SelectionDir::Left);
					}

					moveSelectionLeft(caretMovedDueToNewline);
					scrollTextToCaret();

					markAsDirty();
					return true;
				}
				else
				{
					clearSelection();
					mInputCaret->moveCaretLeft();
					scrollTextToCaret();

					markAsDirty();
					return true;
				}
			}

			if(ev.getKey() == BC_RIGHT)
			{
				if(ev.isShiftDown())
				{
					bool caretMovedDueToNewline = false;
					if(!mSelectionShown)
					{
						if(isNewlineChar(caretPosToSelectionChar(mInputCaret->getCaretPos(), SelectionDir::Left)))
						{
							mInputCaret->moveCaretRight();
							caretMovedDueToNewline = true;
						}

						showSelection(mInputCaret->getCaretPos(), SelectionDir::Left);
					}
						
					moveSelectionRight(caretMovedDueToNewline);
					scrollTextToCaret();

					markAsDirty();
					return true;
				}
				else
				{
					clearSelection();
					mInputCaret->moveCaretRight();
					scrollTextToCaret();

					markAsDirty();
					return true;
				}
			}

			if(ev.getKey() == BC_UP)
			{
				if(ev.isShiftDown())
				{
					if(!mSelectionShown)
					{
						if(isNewlineChar(caretPosToSelectionChar(mInputCaret->getCaretPos(), SelectionDir::Right)))
						{
							mInputCaret->moveCaretLeft();
						}

						showSelection(mInputCaret->getCaretPos(), SelectionDir::Left);
					}
				}
				else
				{
					clearSelection();
				}

				mInputCaret->moveCaretUp();
				scrollTextToCaret();

				if(ev.isShiftDown())
				{
					moveSelectionToCaret(mInputCaret->getCaretPos());
				}

				markAsDirty();
				return true;
			}

			if(ev.getKey() == BC_DOWN)
			{
				if(ev.isShiftDown())
				{
					if(!mSelectionShown)
					{
						if(isNewlineChar(caretPosToSelectionChar(mInputCaret->getCaretPos(), SelectionDir::Left)))
						{
							mInputCaret->moveCaretRight();
						}

						showSelection(mInputCaret->getCaretPos(), SelectionDir::Left);
					}
				}
				else
				{
					clearSelection();
				}

				mInputCaret->moveCaretDown();
				scrollTextToCaret();

				if(ev.isShiftDown())
				{
					moveSelectionToCaret(mInputCaret->getCaretPos());
				}

				markAsDirty();
				return true;
			}

			if(ev.getKey() == BC_RETURN)
			{
				if(mIsMultiline)
				{
					if(mSelectionShown)
					{
						UINT32 selStart = getSelectionStart();
						mText.erase(mText.begin() + selStart, mText.begin() + getSelectionEnd());
						mInputCaret->updateText(getTextDesc(), getTextOffset(), mTextOffset);

						mInputCaret->moveCaretToChar(selStart, CARET_BEFORE);
						scrollTextToCaret();
						clearSelection();
					}

					mText.insert(mText.begin() + mInputCaret->getCharIdxAtCaretPos(), '\n');
					mInputCaret->updateText(getTextDesc(), getTextOffset(), mTextOffset);

					mInputCaret->moveCaretRight();
					scrollTextToCaret();

					markAsDirty();
					return true;
				}
				
			}

			if(ev.getKey() == BC_A && ev.isCtrlDown())
			{
				showSelection(0, SelectionDir::Left);
				selectAll();

				markAsDirty();
				return true;
			}
		}
		else if(ev.getType() == GUIKeyEventType::TextInput)
		{
			if(mSelectionShown)
			{
				UINT32 selStart = getSelectionStart();
				mText.erase(mText.begin() + selStart, mText.begin() + getSelectionEnd());
				mInputCaret->updateText(getTextDesc(), getTextOffset(), mTextOffset);

				mInputCaret->moveCaretToChar(selStart, CARET_BEFORE);
				clearSelection();
			}

			UINT32 charIdx = mInputCaret->getCharIdxAtCaretPos();
			mText.insert(mText.begin() + charIdx, ev.getInputChar());
			mInputCaret->updateText(getTextDesc(), getTextOffset(), mTextOffset);

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
		mInputCaret->updateText(getTextDesc(), getTextOffset(), mTextOffset);

		markAsDirty();
	}

	void GUIInputBox::showSelection(CM::UINT32 caretPos, SelectionDir dir)
	{
		UINT32 charIdx = mInputCaret->getCharIdxAtCaretPos(caretPos);

		if(dir == SelectionDir::Right)
			charIdx = (UINT32)std::max(0, (INT32)(charIdx - 1));

		mSelectionStart = charIdx;
		mSelectionEnd = charIdx;
		mSelectionAnchor = charIdx;
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

	UINT32 GUIInputBox::caretPosToSelectionChar(UINT32 caretPos, SelectionDir dir) const
	{
		UINT32 charIdx = mInputCaret->getCharIdxAtCaretPos(caretPos);

		if(dir == SelectionDir::Right)
			charIdx = (UINT32)std::max(0, (INT32)(charIdx - 1));

		return charIdx;
	}

	bool GUIInputBox::isNewlineChar(CM::UINT32 charIdx) const
	{
		if(mText[charIdx] == '\n')
			return true;

		return false;
	}

	void GUIInputBox::selectionDragStart(UINT32 caretPos)
	{
		clearSelection();

		showSelection(caretPos, SelectionDir::Left); 
		mSelectionDragAnchor = caretPos;
	}

	void GUIInputBox::selectionDragUpdate(UINT32 caretPos)
	{
		if(caretPos < mSelectionDragAnchor)
		{
			mSelectionStart = mInputCaret->getCharIdxAtCaretPos(caretPos);
			mSelectionEnd = mInputCaret->getCharIdxAtCaretPos(mSelectionDragAnchor);

			mSelectionAnchor = mSelectionStart;
		}

		if(caretPos > mSelectionDragAnchor)
		{
			mSelectionStart = mInputCaret->getCharIdxAtCaretPos(mSelectionDragAnchor);
			mSelectionEnd = mInputCaret->getCharIdxAtCaretPos(caretPos);

			mSelectionAnchor = mSelectionEnd;
		}

		if(caretPos == mSelectionDragAnchor)
		{
			mSelectionStart = mSelectionAnchor;
			mSelectionEnd = mSelectionAnchor;
		}
	}

	void GUIInputBox::selectionDragEnd()
	{
		if(isSelectionEmpty())
			clearSelection();
	}

	void GUIInputBox::moveSelectionLeft(bool skipNewline) 
	{
		SelectionDir newlineTestSelectionDir;
		if(mSelectionAnchor == mSelectionEnd)
			newlineTestSelectionDir = SelectionDir::Left;
		else
			newlineTestSelectionDir = SelectionDir::Right;

		if(mInputCaret->getCaretPos() > 0)
		{
			mInputCaret->moveCaretLeft();

			if(!skipNewline) // Move one more if we moved to a new line (we can't select newline char so we skip it)
			{
				if (isNewlineChar(caretPosToSelectionChar(mInputCaret->getCaretPos(), newlineTestSelectionDir)) && mInputCaret->getCaretPos() > 0)
				{
					mInputCaret->moveCaretLeft();

					// Reverse caret movement if previous char was a newline, and this one is as well.
					// Otherwise we skip an entire line which is not what we want.
					if (isNewlineChar(caretPosToSelectionChar(mInputCaret->getCaretPos(), newlineTestSelectionDir))) 
						mInputCaret->moveCaretRight();
				} 
			}
			else
			{
				// Reverse caret movement if previous char was a newline, and this one is as well
				// so we don't skip a line
				if (isNewlineChar(caretPosToSelectionChar(mInputCaret->getCaretPos(), newlineTestSelectionDir))) 
					mInputCaret->moveCaretRight();
			}
		}

		UINT32 charIdx = caretPosToSelectionChar(mInputCaret->getCaretPos(), SelectionDir::Left);
		
		if(mSelectionAnchor == mSelectionEnd)
			mSelectionStart = std::min(mSelectionEnd, charIdx); 
		else
			mSelectionEnd = std::max(mSelectionStart, charIdx);

		if(mSelectionStart == mSelectionEnd)
			clearSelection();
	}

	void GUIInputBox::moveSelectionRight(bool skipNewline) 
	{
		SelectionDir newlineTestSelectionDir;
		if(mSelectionAnchor == mSelectionStart)
			newlineTestSelectionDir = SelectionDir::Right;
		else
			newlineTestSelectionDir = SelectionDir::Left;

		UINT32 maxCaretPos = mInputCaret->getMaxCaretPos();
		if(mInputCaret->getCaretPos() < maxCaretPos)
		{
			mInputCaret->moveCaretRight();

			if(!skipNewline) // Move one more if we moved to a new line (we can't select newline char so we skip it)
			{
				if (isNewlineChar(caretPosToSelectionChar(mInputCaret->getCaretPos(), newlineTestSelectionDir)) && mInputCaret->getCaretPos() < maxCaretPos)
				{
					mInputCaret->moveCaretRight();

					// Reverse caret movement if previous char was a newline, and this one is as well.
					// Otherwise we skip an entire line which is not what we want.
					if (isNewlineChar(caretPosToSelectionChar(mInputCaret->getCaretPos(), newlineTestSelectionDir))) 
						mInputCaret->moveCaretLeft();
				} 
			}
			else
			{
				// Reverse caret movement if previous char was a newline, and this one is as well.
				// Otherwise we skip an entire line which is not what we want.
				if (isNewlineChar(caretPosToSelectionChar(mInputCaret->getCaretPos(), newlineTestSelectionDir))) 
					mInputCaret->moveCaretLeft();
			}
		}

		UINT32 charIdx = caretPosToSelectionChar(mInputCaret->getCaretPos(), SelectionDir::Left);

		if(mSelectionAnchor == mSelectionStart)
			mSelectionEnd = std::max(mSelectionStart, charIdx);
		else
			mSelectionStart = std::min(mSelectionEnd, charIdx);

		if(mSelectionStart == mSelectionEnd)
			clearSelection();
	}

	void GUIInputBox::moveSelectionUp()
	{
		UINT32 charIdx = mInputCaret->getCharIdxAtCaretPos();
		if(charIdx > 0)
			charIdx--;

		UINT32 lineIdx = mTextSprite->getLineForChar(charIdx, true);

		if(lineIdx == 0)
		{
			mInputCaret->moveCaretToStart();
			mSelectionStart = 0; 
			mSelectionEnd = mSelectionAnchor;
		}
		else
		{
			mInputCaret->moveCaretUp();
			UINT32 charIdx = caretPosToSelectionChar(mInputCaret->getCaretPos(), SelectionDir::Left);

			if(charIdx > mSelectionAnchor)
			{
				mSelectionStart = mSelectionAnchor;
				mSelectionEnd = charIdx;
			}
			else
			{
				mSelectionStart = charIdx;
				mSelectionEnd = mSelectionAnchor;
			}
		}

		if(mSelectionStart == mSelectionEnd)
			clearSelection();
	}

	void GUIInputBox::moveSelectionDown()
	{
		UINT32 charIdx = mInputCaret->getCharIdxAtCaretPos();
		if(charIdx > 0)
			charIdx--;

		UINT32 lineIdx = mTextSprite->getLineForChar(charIdx, true);

		if(lineIdx == (mTextSprite->getNumLines() - 1))
		{
			mInputCaret->moveCaretToEnd();
			mSelectionStart = mSelectionAnchor;
			mSelectionEnd = (UINT32)mText.size();
		}
		else
		{
			mInputCaret->moveCaretDown();
			UINT32 charIdx = caretPosToSelectionChar(mInputCaret->getCaretPos(), SelectionDir::Left);

			if(charIdx > mSelectionAnchor)
			{
				mSelectionStart = mSelectionAnchor;
				mSelectionEnd = charIdx;
			}
			else
			{
				mSelectionStart = charIdx;
				mSelectionEnd = mSelectionAnchor;
			}
		}

		if(mSelectionStart == mSelectionEnd)
			clearSelection();
	}

	void GUIInputBox::moveSelectionToCaret(UINT32 caretPos)
	{
		UINT32 charIdx = caretPosToSelectionChar(caretPos, SelectionDir::Left);

		if(charIdx > mSelectionAnchor)
		{
			mSelectionStart = mSelectionAnchor;
			mSelectionEnd = charIdx;
		}
		else
		{
			mSelectionStart = charIdx;
			mSelectionEnd = mSelectionAnchor;
		}

		if(mSelectionStart == mSelectionEnd)
			clearSelection();
	}

	void GUIInputBox::selectAll()
	{
		mSelectionStart = 0;
		mSelectionEnd = (UINT32)mText.size();
	}

	bool GUIInputBox::isSelectionEmpty() const
	{
		return mSelectionStart == mSelectionEnd;
	}

	Vector<Rect>::type GUIInputBox::getSelectionRects() const
	{
		Vector<Rect>::type selectionRects;

		if(mSelectionStart == mSelectionEnd)
			return selectionRects;

		UINT32 startLine = mTextSprite->getLineForChar(mSelectionStart);

		UINT32 endLine = startLine;
		if(mSelectionEnd > 0)
			endLine = mTextSprite->getLineForChar(mSelectionEnd - 1, true);

		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(startLine);
			
			UINT32 startCharIdx = mSelectionStart;

			UINT32 endCharIdx = mSelectionEnd - 1;
			if(startLine != endLine)
			{
				endCharIdx = lineDesc.getEndChar(false);
				if(endCharIdx > 0)
					endCharIdx = endCharIdx - 1;
			}

			if(!isNewlineChar(startCharIdx) && !isNewlineChar(endCharIdx))
			{
				Rect startChar = mTextSprite->getCharRect(startCharIdx);
				Rect endChar = mTextSprite->getCharRect(endCharIdx);

				Rect selectionRect;
				selectionRect.x = startChar.x;
				selectionRect.y = lineDesc.getLineYStart();
				selectionRect.height = lineDesc.getLineHeight();
				selectionRect.width = (endChar.x + endChar.width) - startChar.x;

				selectionRects.push_back(selectionRect);
			}
		}

		for(UINT32 i = startLine + 1; i < endLine; i++)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);
			if(lineDesc.getStartChar() == lineDesc.getEndChar() || isNewlineChar(lineDesc.getStartChar()))
				continue;

			UINT32 endCharIdx = lineDesc.getEndChar(false);
			if(endCharIdx > 0)
				endCharIdx = endCharIdx - 1;

			Rect startChar = mTextSprite->getCharRect(lineDesc.getStartChar());
			Rect endChar = mTextSprite->getCharRect(endCharIdx);

			Rect selectionRect;
			selectionRect.x = startChar.x;
			selectionRect.y = lineDesc.getLineYStart();
			selectionRect.height = lineDesc.getLineHeight();
			selectionRect.width = (endChar.x + endChar.width) - startChar.x;

			selectionRects.push_back(selectionRect);
		}

		if(startLine != endLine)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(endLine);

			if(lineDesc.getStartChar() != lineDesc.getEndChar() && !isNewlineChar(lineDesc.getStartChar()))
			{
				UINT32 endCharIdx = mSelectionEnd - 1;

				if(!isNewlineChar(endCharIdx))
				{
					Rect startChar = mTextSprite->getCharRect(lineDesc.getStartChar());
					Rect endChar = mTextSprite->getCharRect(endCharIdx);

					Rect selectionRect;
					selectionRect.x = startChar.x;
					selectionRect.y = lineDesc.getLineYStart();
					selectionRect.height = lineDesc.getLineHeight();
					selectionRect.width = (endChar.x + endChar.width) - startChar.x;

					selectionRects.push_back(selectionRect);
				}
			}
		}
		
		return selectionRects;
	}

	Int2 GUIInputBox::getSelectionSpriteOffset(UINT32 spriteIdx) const
	{
		Vector<Rect>::type selectionRects = getSelectionRects(); // TODO - Cache these?

		return Int2(selectionRects[spriteIdx].x, selectionRects[spriteIdx].y);
	}

	Rect GUIInputBox::getSelectionSpriteClipRect(UINT32 spriteIdx) const
	{
		Vector<Rect>::type selectionRects = getSelectionRects(); // TODO - Cache these?

		return Rect(getTextBounds().x - selectionRects[spriteIdx].x, 
			getTextBounds().y - selectionRects[spriteIdx].y, 
			selectionRects[spriteIdx].width, selectionRects[spriteIdx].height);
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
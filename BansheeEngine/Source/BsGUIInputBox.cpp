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
		mSelectionShown(false), mIsMultiline(multiline)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();
		mTextSprite = cm_new<TextSprite, PoolAlloc>();
		mInputCaret = cm_new<GUIInputCaret, PoolAlloc>(getTextDesc());

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
		mImageDesc.offset = mOffset;
		mImageDesc.width = mWidth;
		mImageDesc.height = mHeight;
		mImageDesc.clipRect = mClipRect;

		mImageSprite->update(mImageDesc);
		mBounds = mImageSprite->getBounds();

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		mTextSprite->update(textDesc);

		if(mCaretShown && GUIManager::instance().getCaretBlinkState())
		{
			mInputCaret->updateText(textDesc);
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
						mText.erase(mText.begin() + mSelectionStart, mText.begin() + mSelectionEnd);
						mInputCaret->updateText(getTextDesc());

						if(mSelectionStart > 0)
						{
							UINT32 newCaretPos = mSelectionStart - 1;
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
							mInputCaret->updateText(getTextDesc());

							mInputCaret->moveCaretLeft();
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
						mText.erase(mText.begin() + mSelectionStart, mText.begin() + mSelectionEnd);
						mInputCaret->updateText(getTextDesc());

						if(mSelectionStart > 0)
						{
							UINT32 newCaretPos = mSelectionStart - 1;
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
							mInputCaret->updateText(getTextDesc());
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
						showSelection(getCaretSelectionCharIdx(SelectionDir::Left));

					mInputCaret->moveCaretLeft();
					UINT32 charIdx = getCaretSelectionCharIdx(SelectionDir::Left);

					if (isNewlineChar(charIdx) && mInputCaret->getCaretPos() > 0)
					{
						mInputCaret->moveCaretLeft();
						charIdx = getCaretSelectionCharIdx(SelectionDir::Left);
					} 

					scrollTextToCaret();

					if(mSelectionAnchor == mSelectionEnd)
						mSelectionStart = charIdx;
					else
						mSelectionEnd = charIdx;

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
					if(!mSelectionShown)
						showSelection(getCaretSelectionCharIdx(SelectionDir::Left));

					if(mSelectionAnchor == mSelectionStart)
					{
						mInputCaret->moveCaretRight();
						UINT32 charIdx = getCaretSelectionCharIdx(SelectionDir::Left);

						UINT32 maxCaretPos = mInputCaret->getMaxCaretPos();
						if (isNewlineChar(getCaretSelectionCharIdx(SelectionDir::Right)) && mInputCaret->getCaretPos() <= maxCaretPos)
						{
							mInputCaret->moveCaretRight();
							charIdx = getCaretSelectionCharIdx(SelectionDir::Left);
						} 

						mSelectionEnd = charIdx;
					}
					else
					{
						mInputCaret->moveCaretRight();
						UINT32 charIdx = getCaretSelectionCharIdx(SelectionDir::Left);

						UINT32 maxCaretPos = mInputCaret->getMaxCaretPos();
						if (isNewlineChar(charIdx) && mInputCaret->getCaretPos() <= maxCaretPos)
						{
							mInputCaret->moveCaretRight();
							charIdx = getCaretSelectionCharIdx(SelectionDir::Left);
						} 

						mSelectionStart = charIdx;
					}
						
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
					// TODO

					markAsDirty();
					return true;
				}
				else
				{
					clearSelection();
					mInputCaret->moveCaretUp();
					scrollTextToCaret();

					markAsDirty();
					return true;
				}
			}

			if(ev.getKey() == BC_DOWN)
			{
				if(ev.isShiftDown())
				{
					// TODO

					markAsDirty();
					return true;
				}
				else
				{
					clearSelection();
					mInputCaret->moveCaretDown();
					scrollTextToCaret();

					markAsDirty();
					return true;
				}
			}

			if(ev.getKey() == BC_RETURN)
			{
				if(mIsMultiline)
				{
					if(mSelectionShown)
					{
						mText.erase(mText.begin() + mSelectionStart, mText.begin() + mSelectionEnd);
						mInputCaret->updateText(getTextDesc());

						mInputCaret->moveCaretToChar(mSelectionStart, CARET_BEFORE);
						scrollTextToCaret();
						clearSelection();
					}

					mText.insert(mText.begin() + mInputCaret->getCharIdxAtCaretPos(), '\n');
					mInputCaret->updateText(getTextDesc());

					mInputCaret->moveCaretRight();
					scrollTextToCaret();

					markAsDirty();
					return true;
				}
				
			}

			if(ev.getKey() == BC_A && ev.isCtrlDown())
			{
				showSelection(0);

				mSelectionStart = 0;
				mSelectionEnd = (UINT32)mText.size();

				markAsDirty();
				return true;
			}
		}
		else if(ev.getType() == GUIKeyEventType::TextInput)
		{
			if(mSelectionShown)
			{
				mText.erase(mText.begin() + mSelectionStart, mText.begin() + mSelectionEnd);
				mInputCaret->updateText(getTextDesc());

				mInputCaret->moveCaretToChar(mSelectionStart, CARET_BEFORE);
				clearSelection();
			}

			mText.insert(mText.begin() + mInputCaret->getCharIdxAtCaretPos(), ev.getInputChar());
			mInputCaret->updateText(getTextDesc());

			mInputCaret->moveCaretRight();
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

		Int2 caretPos = mInputCaret->getCaretPosition(textDesc.offset);
		UINT32 caretHeight = mInputCaret->getCaretHeight();
		UINT32 caretWidth = 1;
		INT32 caretRight = caretPos.x + (INT32)caretWidth;
		INT32 caretBottom = caretPos.y + (INT32)caretHeight;

		INT32 left = textDesc.offset.x - mTextOffset.x;
		INT32 right = left + (INT32)textDesc.width;
		INT32 top = textDesc.offset.y - mTextOffset.y;
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
		mInputCaret->updateText(getTextDesc());

		markAsDirty();
	}

	void GUIInputBox::showSelection(UINT32 startChar)
	{
		mSelectionStart = startChar;
		mSelectionEnd = startChar;
		mSelectionAnchor = startChar;
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

	UINT32 GUIInputBox::getCaretSelectionCharIdx(SelectionDir dir)
	{
		UINT32 charIdx = mInputCaret->getCharIdxAtCaretPos();

		if(dir == SelectionDir::Right)
			charIdx = (UINT32)std::max(0, (INT32)(charIdx - 1));

		return charIdx;
	}

	bool GUIInputBox::isNewlineChar(CM::UINT32 charIdx)
	{
		if(mText[charIdx] == '\n')
			return true;

		return false;
	}

	Vector<Rect>::type GUIInputBox::getSelectionRects() const
	{
		Vector<Rect>::type selectionRects;

		if(mSelectionStart == mSelectionEnd)
			return selectionRects;

		UINT32 startLine = mTextSprite->getLineForChar(mSelectionStart);

		UINT32 endLine = startLine;
		if(mSelectionEnd > 0)
			endLine = mTextSprite->getLineForChar(mSelectionEnd - 1);

		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(startLine);
			
			UINT32 startCharIdx = mSelectionStart;

			UINT32 endCharIdx = mSelectionEnd - 1;
			if(startLine != endLine)
				endCharIdx = lineDesc.endChar - 1;

			if(startCharIdx != endCharIdx)
			{
				if(startCharIdx == (lineDesc.endChar - 1) && startCharIdx < ((UINT32)mText.size() - 1)) // Ignore newline char
				{
					if(startLine != (mTextSprite->getNumLines() - 1))
						startCharIdx += 1;
				}

				if(endCharIdx == (lineDesc.endChar - 1) && endCharIdx > 0) // Ignore newline char
				{
					if(startLine != (mTextSprite->getNumLines() - 1))
						endCharIdx -= 1;
				}

				Rect startChar = mTextSprite->getCharRect(startCharIdx);
				Rect endChar = mTextSprite->getCharRect(endCharIdx);

				Rect selectionRect;
				selectionRect.x = startChar.x;
				selectionRect.y = lineDesc.lineYStart;
				selectionRect.height = lineDesc.lineHeight;
				selectionRect.width = (endChar.x + endChar.width) - startChar.x;

				selectionRects.push_back(selectionRect);
			}
		}

		for(UINT32 i = startLine + 1; i < endLine; i++)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);
			if(lineDesc.startChar == lineDesc.endChar)
				continue;

			UINT32 startCharIdx = lineDesc.startChar;
			if(startCharIdx == (lineDesc.endChar - 1) && startCharIdx > 0) // Ignore newline char
			{
				startCharIdx -= 1;
			}

			UINT32 endCharIdx = lineDesc.endChar - 1;
			if(endCharIdx > 0) // Ignore newline char
				endCharIdx -= 1;

			Rect startChar = mTextSprite->getCharRect(lineDesc.startChar);
			Rect endChar = mTextSprite->getCharRect(endCharIdx);

			Rect selectionRect;
			selectionRect.x = startChar.x;
			selectionRect.y = lineDesc.lineYStart;
			selectionRect.height = lineDesc.lineHeight;
			selectionRect.width = (endChar.x + endChar.width) - startChar.x;

			selectionRects.push_back(selectionRect);
		}

		if(startLine != endLine)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(endLine);

			if(lineDesc.startChar != lineDesc.endChar)
			{
				UINT32 startCharIdx = lineDesc.startChar;
				if(startCharIdx == (lineDesc.endChar - 1) && startCharIdx > 0) // Ignore newline char
				{
					if(startLine != (mTextSprite->getNumLines() - 1))
						startCharIdx -= 1;
				}

				UINT32 endCharIdx = mSelectionEnd - 1;
				if(endCharIdx == (lineDesc.endChar - 1) && endCharIdx > 0)
				{
					if(endLine != (mTextSprite->getNumLines() - 1)) // Ignore newline char
						endCharIdx -= 1;
				}

				Rect startChar = mTextSprite->getCharRect(lineDesc.startChar);
				Rect endChar = mTextSprite->getCharRect(endCharIdx);

				Rect selectionRect;
				selectionRect.x = startChar.x;
				selectionRect.y = lineDesc.lineYStart;
				selectionRect.height = lineDesc.lineHeight;
				selectionRect.width = (endChar.x + endChar.width) - startChar.x;

				selectionRects.push_back(selectionRect);
			}
		}
		
		return selectionRects;
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
		textDesc.offset = Int2(textBounds.x, textBounds.y) + mTextOffset;
		textDesc.width = textBounds.width;
		textDesc.height = textBounds.height;
		textDesc.clipRect = Rect(-mTextOffset.x, -mTextOffset.y, textDesc.width, textDesc.height);
		textDesc.horzAlign = mStyle->textHorzAlign;
		textDesc.vertAlign = mStyle->textVertAlign;

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
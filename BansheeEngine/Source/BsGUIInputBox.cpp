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
		mSelectionStart(0), mSelectionEnd(0), mSelectionAnchor(0), mCaretSprite(nullptr), mCaretShown(false), 
		mSelectionShown(false), mCaretPos(0), mIsMultiline(multiline)
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
			numElements += mCaretSprite->getNumRenderElements();

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
			newNumElements += mCaretSprite->getNumRenderElements();

			if(renderElemIdx < newNumElements)
			{
				localRenderElemIdx = renderElemIdx - oldNumElements;
				return mCaretSprite;
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
		else if(sprite == mCaretSprite)
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

			if(mText.size() > 0)
				showCaretAtPos(ev.getPosition());
			else
				showCaret(0);

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
						moveCaretToChar(mSelectionStart, CARET_BEFORE);

						clearSelection();
					}
					else
					{
						INT32 charIdx = getCharIdxAtCaretPos();

						if(charIdx < (UINT32)mText.size())
						{
							mText.erase(charIdx, 1);
							moveCaretLeft();
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
						moveCaretToChar(mSelectionStart, CARET_BEFORE);

						clearSelection();
					}
					else
					{
						UINT32 charIdx = getCharIdxAtCaretPos() + 1;
						if(charIdx < (UINT32)mText.size())
						{
							mText.erase(charIdx, 1);
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
						showSelection(getCharIdxAtCaretPos() + 1, getCharIdxAtCaretPos() + 1);

					if(mSelectionAnchor == mSelectionEnd)
						mSelectionStart = (UINT32)std::max(0, (INT32)mSelectionStart - 1);
					else
						mSelectionEnd = (UINT32)std::max(0, (INT32)mSelectionEnd - 1);

					markAsDirty();
					return true;
				}
				else
				{
					clearSelection();
					moveCaretLeft();

					markAsDirty();
					return true;
				}
			}

			if(ev.getKey() == BC_RIGHT)
			{
				if(ev.isShiftDown())
				{
					if(!mSelectionShown)
						showSelection(getCharIdxAtCaretPos() + 1, getCharIdxAtCaretPos() + 1);

					if(mSelectionAnchor == mSelectionStart)
						mSelectionEnd = std::min((UINT32)mText.size(), mSelectionEnd + 1);
					else
						mSelectionStart = std::min((UINT32)mText.size(), mSelectionStart + 1);

					markAsDirty();
					return true;
				}
				else
				{
					clearSelection();
					moveCaretRight();

					markAsDirty();
					return true;
				}
			}

			if(ev.getKey() == BC_UP)
			{
				//Int2 caretCoords = getCaretPosition();
				//UINT32 curLine = 0; mTextSprite->getLineForChar(mCaretPos);

				//if(mCaretPos > 0)
				//	curLine = 0; mTextSprite->getLineForChar(mCaretPos - 1);

				//if(curLine > 0)
				//{
				//	if(ev.isShiftDown())
				//	{
				//		if(!mSelectionShown)
				//			showSelection(mCaretPos, mCaretPos);

				//		// TODO
				//		//if(mSelectionAnchor == mSelectionEnd)
				//		//	mSelectionStart = (UINT32)std::max(0, (INT32)mSelectionStart - 1);
				//		//else
				//		//	mSelectionEnd = (UINT32)std::max(0, (INT32)mSelectionEnd - 1);

				//		markAsDirty();
				//		return true;
				//	}
				//	else
				//	{
				//		clearSelection();

				//		const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(curLine);
				//		Int2 newCaretCoords = caretCoords;
				//		newCaretCoords.y -= lineDesc.lineHeight / 2;

				//		showCaretAtPos(newCaretCoords);
				//	}
				//}
			}

			if(ev.getKey() == BC_DOWN)
			{
				// TODO
			}

			if(ev.getKey() == BC_RETURN)
			{
				if(mIsMultiline)
				{
					if(mSelectionShown)
					{
						mText.erase(mText.begin() + mSelectionStart, mText.begin() + mSelectionEnd);
						moveCaretToChar(mSelectionStart, CARET_BEFORE);
						clearSelection();
					}

					if(mText.size() > 0)
						mText.insert(mText.begin() + getCharIdxAtCaretPos() + 1, '\n');
					else
						mText.insert(mText.begin(), '\n');

					moveCaretRight();

					markAsDirty();
					return true;
				}
				
			}

			if(ev.getKey() == BC_A && ev.isCtrlDown())
			{
				showSelection(0, (UINT32)mText.size());
			}
		}
		else if(ev.getType() == GUIKeyEventType::TextInput)
		{
			if(mSelectionShown)
			{
				mText.erase(mText.begin() + mSelectionStart, mText.begin() + mSelectionEnd);
				moveCaretToChar(mSelectionStart, CARET_BEFORE);
				clearSelection();
			}

			if(mText.size() > 0)
				mText.insert(mText.begin() + getCharIdxAtCaretPos() + 1, ev.getInputChar());
			else
				mText.insert(mText.begin(), ev.getInputChar());

			moveCaretRight();

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

	void GUIInputBox::showCaret(CM::UINT32 caretPos)
	{
		mCaretPos = caretPos;
		mCaretShown = true;
		markAsDirty();
	}

	void GUIInputBox::showCaretAtPos(const CM::Int2& pos)
	{
		INT32 charIdx = mTextSprite->getCharIdxAtPos(pos);

		if(charIdx != -1)
		{
			Rect charRect = mTextSprite->getCharRect(charIdx);

			float xCenter = charRect.x + charRect.width * 0.5f;
			if(pos.x <= xCenter)
				moveCaretToChar(charIdx, CARET_BEFORE);
			else
				moveCaretToChar(charIdx, CARET_AFTER);
		}
		else
		{
			UINT32 numLines = mTextSprite->getNumLines();

			UINT32 curPos = 0;
			for(UINT32 i = 0; i < numLines; i++)
			{
				const SpriteLineDesc& line = mTextSprite->getLineDesc(i);

				if(pos.y >= line.lineYStart && pos.y < (line.lineYStart + (INT32)line.lineHeight))
				{
					mCaretPos = curPos;
					return;
				}

				UINT32 numChars = line.endChar - line.startChar;
				curPos += numChars;
			}

			mCaretPos = curPos;
		}
	}

	void GUIInputBox::clearCaret()
	{
		mCaretShown = false;
		markAsDirty();
	}

	Int2 GUIInputBox::getCaretPosition() const
	{
		if(mText.size() > 0)
		{
			UINT32 curPos = 0;
			UINT32 numLines = mTextSprite->getNumLines();
			for(UINT32 i = 0; i < numLines; i++)
			{
				if(mCaretPos == curPos)
				{
					// Caret is on line start
					Rect contentBounds = getTextBounds();
					return Int2(contentBounds.x, mTextSprite->getLineDesc(i).lineYStart);
				}

				const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);
				UINT32 numChars = lineDesc.endChar - lineDesc.startChar;
				curPos += numChars;
			}

			UINT32 charIdx = getCharIdxAtCaretPos();
			charIdx = std::min((UINT32)(mText.size() - 1), charIdx);

			Rect charRect = mTextSprite->getCharRect(charIdx);
			UINT32 lineIdx = mTextSprite->getLineForChar(charIdx);
			UINT32 yOffset = mTextSprite->getLineDesc(lineIdx).lineYStart;

			return Int2(charRect.x + charRect.width + 1, yOffset);
		}
		else
		{
			Rect contentBounds = getTextBounds();
			return Int2(contentBounds.x, contentBounds.y);
		}		
	}

	UINT32 GUIInputBox::getCaretHeight() const
	{
		UINT32 charIdx = getCharIdxAtCaretPos();

		if(charIdx < (UINT32)mText.size())
		{
			UINT32 lineIdx = mTextSprite->getLineForChar(charIdx);
			return mTextSprite->getLineDesc(lineIdx).lineHeight;
		}
		else
		{
			if(mStyle->font != nullptr)
			{
				UINT32 nearestSize = mStyle->font->getClosestAvailableSize(mStyle->fontSize);
				const FontData* fontData = mStyle->font->getFontDataForSize(nearestSize);

				if(fontData != nullptr)
					return fontData->fontDesc.lineHeight;
			}
		}

		return 0;
	}

	void GUIInputBox::moveCaretLeft()
	{
		mCaretPos = (UINT32)std::max(0, (INT32)mCaretPos - 1);

		if(isCaretAtLineStart()) // Skip line start char as well
			mCaretPos = (UINT32)std::max(0, (INT32)mCaretPos - 1);
	}

	void GUIInputBox::moveCaretRight()
	{
		//if(isCaretAtLineStart())
		//	mCaretPos = mCaretPos + 2; // Skip line start char as well
		//else
			mCaretPos = mCaretPos + 1; // TODO - Limit this
	}

	void GUIInputBox::moveCaretToChar(UINT32 charIdx, CaretPos caretPos)
	{
		if(charIdx >= (UINT32)mText.size())
		{
			mCaretPos = 0;
			return;
		}

		UINT32 numLines = mTextSprite->getNumLines();
		UINT32 curPos = 0;
		UINT32 curCharIdx = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			if(i == 0)
				curPos++; // Beginning of the line has a special caret position, primarily so we can
			// still place a caret on an empty line

			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);
			UINT32 numChars = lineDesc.endChar - lineDesc.startChar;
			if(charIdx > (curCharIdx + numChars))
			{
				curCharIdx += numChars;
				curPos += numChars;
				continue;
			}

			UINT32 diff = charIdx - curCharIdx;

			if(caretPos == CARET_BEFORE)
				curPos += diff - 1;
			else
				curPos += diff;

			break;
		}

		showCaret(curPos);
	}

	UINT32 GUIInputBox::getCharIdxAtCaretPos() const
	{
		UINT32 numLines = mTextSprite->getNumLines();
		UINT32 curPos = 0;
		UINT32 curCharIdx = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);

			if(curPos == mCaretPos)
			{
				return lineDesc.startChar;
			}

			if(i == 0)
				curPos++; // Beginning of the line has a special caret position, primarily so we can
						  // still place a caret on an empty line

			UINT32 numChars = lineDesc.endChar - lineDesc.startChar;
			if(mCaretPos > (curPos + numChars))
			{
				curCharIdx += numChars;
				curPos += numChars;
				continue;
			}

			UINT32 diff = mCaretPos - curPos; 
			curCharIdx += diff;

			return curCharIdx;
		}

		return 0;
	}

	bool GUIInputBox::isCaretAtLineStart() const
	{
		UINT32 numLines = mTextSprite->getNumLines();
		UINT32 curPos = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);

			if(curPos == mCaretPos)
				return true;

			if(i == 0)
				curPos++; // Beginning of the line has a special caret position, primarily so we can
						  // still place a caret on an empty line

			UINT32 numChars = lineDesc.endChar - lineDesc.startChar;
			curPos += numChars;

			if(curPos > mCaretPos)
				return false;
		}

		return false;
	}

	void GUIInputBox::showSelection(UINT32 startChar, UINT32 endChar)
	{
		mSelectionStart = startChar;
		mSelectionEnd = endChar;
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

	Vector<Rect>::type GUIInputBox::getSelectionRects() const
	{
		Vector<Rect>::type selectionRects;

		if(mSelectionStart == mSelectionEnd)
			return selectionRects;

		UINT32 startLine = mTextSprite->getLineForChar(mSelectionStart);
		UINT32 endLine = mTextSprite->getLineForChar(mSelectionEnd - 1);

		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(startLine);
			Rect startChar = mTextSprite->getCharRect(mSelectionStart);
			UINT32 endCharIdx = mSelectionEnd - 1;
			if(endCharIdx >= lineDesc.endChar)
				endCharIdx = lineDesc.endChar - 1;
			
			Rect endChar = mTextSprite->getCharRect(endCharIdx);

			Rect selectionRect;
			selectionRect.x = startChar.x;
			selectionRect.y = lineDesc.lineYStart;
			selectionRect.height = lineDesc.lineHeight;
			selectionRect.width = (endChar.x + endChar.width) - startChar.x;

			selectionRects.push_back(selectionRect);
		}

		for(UINT32 i = startLine + 1; i < endLine; i++)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);
			if(lineDesc.startChar == lineDesc.endChar)
				continue;

			Rect startChar = mTextSprite->getCharRect(lineDesc.startChar);
			Rect endChar = mTextSprite->getCharRect(lineDesc.endChar);

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
				Rect startChar = mTextSprite->getCharRect(lineDesc.startChar);
				Rect endChar = mTextSprite->getCharRect(mSelectionEnd - 1);

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
		textDesc.offset = Int2(textBounds.x, textBounds.y);
		textDesc.width = textBounds.width;
		textDesc.height = textBounds.height;
		textDesc.clipRect = Rect(0, 0, textDesc.width, textDesc.height);
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
			clearCaret();
			clearSelection();
			markAsDirty();
		}
	}
}
#include "BsGUIInputBox.h"
#include "BsGUIManager.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUITextInputEvent.h"
#include "BsGUIMouseEvent.h"
#include "BsGUICommandEvent.h"
#include "CmFont.h"
#include "CmTextData.h"
#include "CmTexture.h"
#include "CmPlatform.h"
#include "BsGUIInputCaret.h"
#include "BsGUIInputSelection.h"
#include "BsDragAndDropManager.h"
#include "BsGUIContextMenu.h"
#include "BsGUIHelper.h"

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
		mCaretShown(false), mSelectionShown(false), mIsMultiline(multiline), mHasFocus(false)
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

	GUIInputBox* GUIInputBox::create(GUIWidget& parent, bool multiline, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIInputBox, PoolAlloc>()) GUIInputBox(parent, style, GUILayoutOptions::create(style), multiline);
	}

	GUIInputBox* GUIInputBox::create(GUIWidget& parent, const GUIOptions& layoutOptions, bool multiline, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIInputBox, PoolAlloc>()) GUIInputBox(parent, style, GUILayoutOptions::create(layoutOptions, style), multiline);
	}

	UINT32 GUIInputBox::getNumRenderElements() const
	{
		UINT32 numElements = mImageSprite->getNumRenderElements();
		numElements += mTextSprite->getNumRenderElements();

		if(mCaretShown && gGUIManager().getCaretBlinkState())
			numElements += gGUIManager().getInputCaretTool()->getSprite()->getNumRenderElements();

		if(mSelectionShown)
		{
			const Vector<ImageSprite*>::type& sprites = gGUIManager().getInputSelectionTool()->getSprites();
			for(auto& selectionSprite : sprites)
			{
				numElements += selectionSprite->getNumRenderElements();
			}
		}

		return numElements;
	}

	const GUIMaterialInfo& GUIInputBox::getMaterial(UINT32 renderElementIdx) const
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

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		mTextSprite->update(textDesc);

		if(mCaretShown && gGUIManager().getCaretBlinkState())
		{
			gGUIManager().getInputCaretTool()->updateText(this, textDesc); // TODO - These shouldn't be here. Only call this when one of these parameters changes.
			gGUIManager().getInputCaretTool()->updateSprite();
		}

		if(mSelectionShown)
		{
			gGUIManager().getInputSelectionTool()->updateText(this, textDesc); // TODO - These shouldn't be here. Only call this when one of these parameters changes.
			gGUIManager().getInputSelectionTool()->updateSprite();
		}

		// When text bounds are reduced the scroll needs to be adjusted so that
		// input box isn't filled with mostly empty space.
		clampScrollToBounds(mTextSprite->getBounds(mOffset, RectI()));

		GUIElement::updateRenderElementsInternal();
	}

	void GUIInputBox::updateClippedBounds()
	{
		mClippedBounds = mImageSprite->getBounds(mOffset, mClipRect);
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

		if(mCaretShown && gGUIManager().getCaretBlinkState())
		{
			oldNumElements = newNumElements;
			newNumElements += gGUIManager().getInputCaretTool()->getSprite()->getNumRenderElements();

			if(renderElemIdx < newNumElements)
			{
				localRenderElemIdx = renderElemIdx - oldNumElements;
				return gGUIManager().getInputCaretTool()->getSprite();
			}
		}

		if(mSelectionShown)
		{
			const Vector<ImageSprite*>::type& sprites = gGUIManager().getInputSelectionTool()->getSprites();
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

	Vector2I GUIInputBox::renderElemToOffset(UINT32 renderElemIdx) const
	{
		UINT32 oldNumElements = 0;
		UINT32 newNumElements = oldNumElements + mTextSprite->getNumRenderElements();
		if(renderElemIdx < newNumElements)
			return getTextOffset();

		oldNumElements = newNumElements;
		newNumElements += mImageSprite->getNumRenderElements();

		if(renderElemIdx < newNumElements)
			return mOffset;

		if(mCaretShown && gGUIManager().getCaretBlinkState())
		{
			oldNumElements = newNumElements;
			newNumElements += gGUIManager().getInputCaretTool()->getSprite()->getNumRenderElements();

			if(renderElemIdx < newNumElements)
				return gGUIManager().getInputCaretTool()->getSpriteOffset();
		}

		if(mSelectionShown)
		{
			UINT32 spriteIdx = 0;
			const Vector<ImageSprite*>::type& sprites = gGUIManager().getInputSelectionTool()->getSprites();
			for(auto& selectionSprite : sprites)
			{
				oldNumElements = newNumElements;
				newNumElements += selectionSprite->getNumRenderElements();

				if(renderElemIdx < newNumElements)
					return gGUIManager().getInputSelectionTool()->getSelectionSpriteOffset(spriteIdx);

				spriteIdx++;
			}
		}

		return Vector2I();
	}

	RectI GUIInputBox::renderElemToClipRect(UINT32 renderElemIdx) const
	{
		UINT32 oldNumElements = 0;
		UINT32 newNumElements = oldNumElements + mTextSprite->getNumRenderElements();
		if(renderElemIdx < newNumElements)
			return getTextClipRect();

		oldNumElements = newNumElements;
		newNumElements += mImageSprite->getNumRenderElements();

		if(renderElemIdx < newNumElements)
			return mClipRect;

		if(mCaretShown && gGUIManager().getCaretBlinkState())
		{
			oldNumElements = newNumElements;
			newNumElements += gGUIManager().getInputCaretTool()->getSprite()->getNumRenderElements();

			if(renderElemIdx < newNumElements)
			{
				return gGUIManager().getInputCaretTool()->getSpriteClipRect(getTextClipRect());
			}
		}

		if(mSelectionShown)
		{
			UINT32 spriteIdx = 0;
			const Vector<ImageSprite*>::type& sprites = gGUIManager().getInputSelectionTool()->getSprites();
			for(auto& selectionSprite : sprites)
			{
				oldNumElements = newNumElements;
				newNumElements += selectionSprite->getNumRenderElements();

				if(renderElemIdx < newNumElements)
					return gGUIManager().getInputSelectionTool()->getSelectionSpriteClipRect(spriteIdx, getTextClipRect());

				spriteIdx++;
			}
		}

		return RectI();
	}

	Vector2I GUIInputBox::_getOptimalSize() const
	{
		UINT32 imageWidth = 0;
		UINT32 imageHeight = 0;
		if(mImageDesc.texture != nullptr)
		{
			imageWidth = mImageDesc.texture->getTexture()->getWidth();
			imageHeight = mImageDesc.texture->getTexture()->getHeight();
		}

		Vector2I contentSize = GUIHelper::calcOptimalContentsSize(mText, *mStyle, _getLayoutOptions());
		UINT32 contentWidth = std::max(imageWidth, (UINT32)contentSize.x);
		UINT32 contentHeight = std::max(imageHeight, (UINT32)contentSize.y);

		return Vector2I(contentWidth, contentHeight);
	}

	CM::Vector2I GUIInputBox::_getTextInputOffset() const
	{
		return mTextOffset;	
	}

	CM::RectI GUIInputBox::_getTextInputRect() const
	{
		RectI textBounds = getContentBounds();
		textBounds.x -= mOffset.x;
		textBounds.y -= mOffset.y;

		return textBounds;
	}

	UINT32 GUIInputBox::_getRenderElementDepth(UINT32 renderElementIdx) const
	{
		UINT32 localRenderElementIdx;
		Sprite* sprite = renderElemToSprite(renderElementIdx, localRenderElementIdx);

		if(sprite == mImageSprite)
			return _getDepth();
		else if(sprite == mTextSprite)
			return _getDepth() - 2;
		else if(sprite == gGUIManager().getInputCaretTool()->getSprite())
			return _getDepth() - 3;
		else // Selection sprites
			return _getDepth() - 1;
	}

	void GUIInputBox::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		UINT32 localRenderElementIdx;
		Sprite* sprite = renderElemToSprite(renderElementIdx, localRenderElementIdx);
		Vector2I offset = renderElemToOffset(renderElementIdx);
		RectI clipRect = renderElemToClipRect(renderElementIdx);

		sprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, localRenderElementIdx, offset, clipRect);
	}

	bool GUIInputBox::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseOver)
		{
			if(!mHasFocus)
			{
				mImageDesc.texture = mStyle->hover.texture;
				markContentAsDirty();
			}

			if(!mInputCursorSet)
			{
				Platform::setCursor(CursorType::IBeam);
				mInputCursorSet = true;
			}

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseOut)
		{
			if(!mHasFocus)
			{
				mImageDesc.texture = mStyle->normal.texture;
				markContentAsDirty();
			}

			if(!mDragInProgress && mInputCursorSet)
			{
				Platform::setCursor(CursorType::Arrow);
				mInputCursorSet = false;
			}

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDoubleClick && ev.getButton() == GUIMouseButton::Left)
		{
			showSelection(0);
			gGUIManager().getInputSelectionTool()->selectAll();

			markContentAsDirty();
			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDown && ev.getButton() == GUIMouseButton::Left)
		{
			if(mHasFocus)
			{
				if(ev.isShiftDown())
				{
					if(!mSelectionShown)
						showSelection(gGUIManager().getInputCaretTool()->getCaretPos());
				}
				else
					clearSelection();

				if(mText.size() > 0)
					gGUIManager().getInputCaretTool()->moveCaretToPos(ev.getPosition());
				else
					gGUIManager().getInputCaretTool()->moveCaretToStart();

				if(ev.isShiftDown())
					gGUIManager().getInputSelectionTool()->moveSelectionToCaret(gGUIManager().getInputCaretTool()->getCaretPos());
			}
			else
			{
				clearSelection();
				showCaret();

				if(mText.size() > 0)
					gGUIManager().getInputCaretTool()->moveCaretToPos(ev.getPosition());
				else
					gGUIManager().getInputCaretTool()->moveCaretToStart();
			}

			scrollTextToCaret();

			markContentAsDirty();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDragStart)
		{
			if(!ev.isShiftDown())
			{
				mDragInProgress = true;

				UINT32 caretPos = gGUIManager().getInputCaretTool()->getCaretPos();
				showSelection(caretPos);
				gGUIManager().getInputSelectionTool()->selectionDragStart(caretPos);

				return true;
			}
		}
		else if(ev.getType() == GUIMouseEventType::MouseDragEnd)
		{
			if(!ev.isShiftDown())
			{
				mDragInProgress = false;

				if(ev.getMouseOverElement() != this && mInputCursorSet)
				{
					Platform::setCursor(CursorType::Arrow);
					mInputCursorSet = false;
				}

				gGUIManager().getInputSelectionTool()->selectionDragEnd();

				return true;
			}
		}
		else if(ev.getType() == GUIMouseEventType::MouseDrag)
		{
			if(!ev.isShiftDown())
			{
				if(mText.size() > 0)
					gGUIManager().getInputCaretTool()->moveCaretToPos(ev.getPosition());
				else
					gGUIManager().getInputCaretTool()->moveCaretToStart();

				gGUIManager().getInputSelectionTool()->selectionDragUpdate(gGUIManager().getInputCaretTool()->getCaretPos());

				scrollTextToCaret();

				markContentAsDirty();
				return true;
			}
		}

		return false;
	}

	bool GUIInputBox::textInputEvent(const GUITextInputEvent& ev)
	{
		if(mSelectionShown)
			deleteSelectedText();

		UINT32 charIdx = gGUIManager().getInputCaretTool()->getCharIdxAtCaretPos();
		insertChar(charIdx, ev.getInputChar());

		gGUIManager().getInputCaretTool()->moveCaretToChar(charIdx, CARET_AFTER);

		scrollTextToCaret();

		markContentAsDirty();
		return true;
	}

	bool GUIInputBox::commandEvent(const GUICommandEvent& ev)
	{
		if(ev.getType() == GUICommandEventType::Redraw)
		{
			markMeshAsDirty();
			return true;
		}

		if(ev.getType() == GUICommandEventType::Backspace)
		{
			if(mText.size() > 0)
			{
				if(mSelectionShown)
				{
					deleteSelectedText();
				}
				else
				{
					UINT32 charIdx = gGUIManager().getInputCaretTool()->getCharIdxAtCaretPos() - 1;

					if(charIdx < (UINT32)mText.size())
					{
						eraseChar(charIdx);

						if(charIdx > 0)
							charIdx--;

						gGUIManager().getInputCaretTool()->moveCaretToChar(charIdx, CARET_AFTER);

						scrollTextToCaret();
					}
				}

				markContentAsDirty();
			}

			return true;
		}

		if(ev.getType() == GUICommandEventType::Delete)
		{
			if(mText.size() > 0)
			{
				if(mSelectionShown)
				{
					deleteSelectedText();
				}
				else
				{
					UINT32 charIdx = gGUIManager().getInputCaretTool()->getCharIdxAtCaretPos();
					if(charIdx < (UINT32)mText.size())
					{
						eraseChar(charIdx);

						if(charIdx > 0)
							charIdx--;

						gGUIManager().getInputCaretTool()->moveCaretToChar(charIdx, CARET_AFTER);

						scrollTextToCaret();
					}
				}

				markContentAsDirty();
			}

			return true;
		}

		if(ev.getType() == GUICommandEventType::CursorMoveLeft)
		{
			if(mSelectionShown)
			{
				UINT32 selStart = gGUIManager().getInputSelectionTool()->getSelectionStart();
				clearSelection();

				if(selStart > 0)
					gGUIManager().getInputCaretTool()->moveCaretToChar(selStart - 1, CARET_AFTER);
				else
					gGUIManager().getInputCaretTool()->moveCaretToChar(0, CARET_BEFORE);
			}
			else
				gGUIManager().getInputCaretTool()->moveCaretLeft();

			scrollTextToCaret();
			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUICommandEventType::SelectLeft)
		{
			if(!mSelectionShown)
				showSelection(gGUIManager().getInputCaretTool()->getCaretPos());

			gGUIManager().getInputCaretTool()->moveCaretLeft();
			gGUIManager().getInputSelectionTool()->moveSelectionToCaret(gGUIManager().getInputCaretTool()->getCaretPos());

			scrollTextToCaret();
			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUICommandEventType::CursorMoveRight)
		{
			if(mSelectionShown)
			{
				UINT32 selEnd = gGUIManager().getInputSelectionTool()->getSelectionEnd();
				clearSelection();

				if(selEnd > 0)
					gGUIManager().getInputCaretTool()->moveCaretToChar(selEnd - 1, CARET_AFTER);
				else
					gGUIManager().getInputCaretTool()->moveCaretToChar(0, CARET_BEFORE);
			}
			else
				gGUIManager().getInputCaretTool()->moveCaretRight();

			scrollTextToCaret();
			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUICommandEventType::SelectRight)
		{
			if(!mSelectionShown)
				showSelection(gGUIManager().getInputCaretTool()->getCaretPos());

			gGUIManager().getInputCaretTool()->moveCaretRight();
			gGUIManager().getInputSelectionTool()->moveSelectionToCaret(gGUIManager().getInputCaretTool()->getCaretPos());

			scrollTextToCaret();
			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUICommandEventType::CursorMoveUp)
		{
			clearSelection();

			gGUIManager().getInputCaretTool()->moveCaretUp();

			scrollTextToCaret();
			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUICommandEventType::SelectUp)
		{
			if(!mSelectionShown)
				showSelection(gGUIManager().getInputCaretTool()->getCaretPos());;

			gGUIManager().getInputCaretTool()->moveCaretUp();
			gGUIManager().getInputSelectionTool()->moveSelectionToCaret(gGUIManager().getInputCaretTool()->getCaretPos());

			scrollTextToCaret();
			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUICommandEventType::CursorMoveDown)
		{
			clearSelection();

			gGUIManager().getInputCaretTool()->moveCaretDown();

			scrollTextToCaret();
			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUICommandEventType::SelectDown)
		{
			if(!mSelectionShown)
				showSelection(gGUIManager().getInputCaretTool()->getCaretPos());

			gGUIManager().getInputCaretTool()->moveCaretDown();
			gGUIManager().getInputSelectionTool()->moveSelectionToCaret(gGUIManager().getInputCaretTool()->getCaretPos());

			scrollTextToCaret();
			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUICommandEventType::Return)
		{
			if(mIsMultiline)
			{
				if(mSelectionShown)
					deleteSelectedText();

				insertChar(gGUIManager().getInputCaretTool()->getCharIdxAtCaretPos(), '\n');

				gGUIManager().getInputCaretTool()->moveCaretRight();
				scrollTextToCaret();

				markContentAsDirty();
				return true;
			}

		}

		if(ev.getType() == GUICommandEventType::SelectAll)
		{
			showSelection(0);
			gGUIManager().getInputSelectionTool()->selectAll();

			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUICommandEventType::Cut)
		{
			cutText();

			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUICommandEventType::Copy)
		{
			copyText();

			return true;
		}

		if(ev.getType() == GUICommandEventType::Paste)
		{
			pasteText();

			markContentAsDirty();
			return true;
		}

		return false;
	}

	void GUIInputBox::showCaret()
	{
		mCaretShown = true;

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Vector2I offset = getTextOffset();
		gGUIManager().getInputCaretTool()->updateText(this, textDesc);
		markContentAsDirty();
	}


	void GUIInputBox::hideCaret()
	{
		mCaretShown = false;
		markContentAsDirty();
	}

	void GUIInputBox::showSelection(CM::UINT32 anchorCaretPos)
	{
		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Vector2I offset = getTextOffset();
		gGUIManager().getInputSelectionTool()->updateText(this, textDesc);

		gGUIManager().getInputSelectionTool()->showSelection(anchorCaretPos);
		mSelectionShown = true;
		markContentAsDirty();
	}

	void GUIInputBox::clearSelection()
	{
		gGUIManager().getInputSelectionTool()->clearSelection();
		mSelectionShown = false;
		markContentAsDirty();
	}

	void GUIInputBox::scrollTextToCaret()
	{
		TEXT_SPRITE_DESC textDesc = getTextDesc();

		Vector2I textOffset = getTextOffset();
		Vector2I caretPos = gGUIManager().getInputCaretTool()->getCaretPosition(textOffset);
		UINT32 caretHeight = gGUIManager().getInputCaretTool()->getCaretHeight();
		UINT32 caretWidth = 1;
		INT32 caretRight = caretPos.x + (INT32)caretWidth;
		INT32 caretBottom = caretPos.y + (INT32)caretHeight;

		INT32 left = textOffset.x - mTextOffset.x;
		// Include caret width here because we don't want to scroll if just the caret is outside the bounds
		// (Possible if the text width is exactly the maximum width)
		INT32 right = left + (INT32)textDesc.width + caretWidth; 
		INT32 top = textOffset.y - mTextOffset.y;
		INT32 bottom = top + (INT32)textDesc.height;

		Vector2I offset;
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

		gGUIManager().getInputCaretTool()->updateText(this, textDesc);
		gGUIManager().getInputSelectionTool()->updateText(this, textDesc);

		markContentAsDirty();
	}

	void GUIInputBox::clampScrollToBounds(RectI unclippedTextBounds)
	{
		TEXT_SPRITE_DESC textDesc = getTextDesc();

		Vector2I newTextOffset;
		INT32 maxScrollableWidth = std::max(0, (INT32)unclippedTextBounds.width - (INT32)textDesc.width);
		INT32 maxScrollableHeight = std::max(0, (INT32)unclippedTextBounds.height - (INT32)textDesc.height);
		newTextOffset.x = Math::clamp(mTextOffset.x, -maxScrollableWidth, 0);
		newTextOffset.y = Math::clamp(mTextOffset.y, -maxScrollableHeight, 0);

		if(newTextOffset != mTextOffset)
		{
			mTextOffset = newTextOffset;

			gGUIManager().getInputCaretTool()->updateText(this, textDesc);
			gGUIManager().getInputSelectionTool()->updateText(this, textDesc);

			markContentAsDirty();
		}
	}

	void GUIInputBox::insertString(CM::UINT32 charIdx, const WString& string)
	{
		mText.insert(mText.begin() + charIdx, string.begin(), string.end());

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Vector2I offset = getTextOffset();

		gGUIManager().getInputCaretTool()->updateText(this, textDesc);
		gGUIManager().getInputSelectionTool()->updateText(this, textDesc);
	}

	void GUIInputBox::insertChar(CM::UINT32 charIdx, CM::UINT32 charCode)
	{
		mText.insert(mText.begin() + charIdx, charCode);

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Vector2I offset = getTextOffset();

		gGUIManager().getInputCaretTool()->updateText(this, textDesc);
		gGUIManager().getInputSelectionTool()->updateText(this, textDesc);
	}

	void GUIInputBox::eraseChar(CM::UINT32 charIdx)
	{
		mText.erase(charIdx, 1);

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Vector2I offset = getTextOffset();

		gGUIManager().getInputCaretTool()->updateText(this, textDesc);
		gGUIManager().getInputSelectionTool()->updateText(this, textDesc);
	}

	void GUIInputBox::deleteSelectedText()
	{
		UINT32 selStart = gGUIManager().getInputSelectionTool()->getSelectionStart();
		mText.erase(mText.begin() + selStart, mText.begin() + gGUIManager().getInputSelectionTool()->getSelectionEnd());

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Vector2I offset = getTextOffset();
		gGUIManager().getInputCaretTool()->updateText(this, textDesc);
		gGUIManager().getInputSelectionTool()->updateText(this, textDesc);

		if(selStart > 0)
		{
			UINT32 newCaretPos = selStart - 1;
			gGUIManager().getInputCaretTool()->moveCaretToChar(newCaretPos, CARET_AFTER);
		}
		else
		{
			gGUIManager().getInputCaretTool()->moveCaretToChar(0, CARET_BEFORE);
		}

		scrollTextToCaret();
		clearSelection();
	}

	WString GUIInputBox::getSelectedText()
	{
		UINT32 selStart = gGUIManager().getInputSelectionTool()->getSelectionStart();
		UINT32 selEnd = gGUIManager().getInputSelectionTool()->getSelectionEnd();

		return mText.substr(selStart, selEnd - selStart);

		mText.erase(mText.begin() + selStart, mText.begin() + gGUIManager().getInputSelectionTool()->getSelectionEnd());
	}

	CM::Vector2I GUIInputBox::getTextOffset() const
	{
		RectI textBounds = getContentBounds();
		return Vector2I(textBounds.x, textBounds.y) + mTextOffset;
	}

	CM::RectI GUIInputBox::getTextClipRect() const
	{
		RectI contentClipRect = getContentClipRect();
		return RectI(contentClipRect.x - mTextOffset.x, contentClipRect.y - mTextOffset.y, contentClipRect.width, contentClipRect.height);
	}

	TEXT_SPRITE_DESC GUIInputBox::getTextDesc() const
	{
		TEXT_SPRITE_DESC textDesc;
		textDesc.text = mText;
		textDesc.font = mStyle->font;
		textDesc.fontSize = mStyle->fontSize;

		RectI textBounds = getContentBounds();
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
			markContentAsDirty();
		}
		else
		{
			mImageDesc.texture = mStyle->normal.texture;
			hideCaret();
			clearSelection();
			markContentAsDirty();
		}

		mHasFocus = focus;
	}

	GUIContextMenu* GUIInputBox::getContextMenu() const
	{
		static bool initialized = false;
		static GUIContextMenu mContextMenu;

		if(!initialized)
		{
			mContextMenu.addMenuItem(L"Cut", boost::bind(&GUIInputBox::cutText, const_cast<GUIInputBox*>(this)));
			mContextMenu.addMenuItem(L"Copy", boost::bind(&GUIInputBox::copyText, const_cast<GUIInputBox*>(this)));
			mContextMenu.addMenuItem(L"Paste", boost::bind(&GUIInputBox::pasteText, const_cast<GUIInputBox*>(this)));

			mContextMenu.setLocalizedName(L"Cut", HString(L"Cut"));
			mContextMenu.setLocalizedName(L"Copy", HString(L"Copy"));
			mContextMenu.setLocalizedName(L"Paste", HString(L"Paste"));

			initialized = true;
		}

		return &mContextMenu;
	}

	void GUIInputBox::cutText()
	{
		copyText();
		deleteSelectedText();
	}

	void GUIInputBox::copyText()
	{
		Platform::copyToClipboard(getSelectedText());
	}

	void GUIInputBox::pasteText()
	{
		WString textInClipboard = Platform::copyFromClipboard();

		UINT32 charIdx = gGUIManager().getInputCaretTool()->getCharIdxAtCaretPos();
		insertString(charIdx, textInClipboard);

		if(textInClipboard.size() > 0)
			gGUIManager().getInputCaretTool()->moveCaretToChar(charIdx + ((UINT32)textInClipboard.size() - 1), CARET_AFTER);

		scrollTextToCaret();

		markContentAsDirty();
	}
}
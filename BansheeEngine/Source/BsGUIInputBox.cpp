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

		return new (cm_alloc<GUIInputBox, PoolAlloc>()) GUIInputBox(parent, style, getDefaultLayoutOptions(style), multiline);
	}

	GUIInputBox* GUIInputBox::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, bool multiline, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIInputBox, PoolAlloc>()) GUIInputBox(parent, style, layoutOptions, multiline);
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

		return Rect();
	}

	Int2 GUIInputBox::_getOptimalSize() const
	{
		UINT32 imageWidth = 0;
		UINT32 imageHeight = 0;
		if(mImageDesc.texture != nullptr)
		{
			imageWidth = mImageDesc.texture->getTexture()->getWidth();
			imageHeight = mImageDesc.texture->getTexture()->getHeight();
		}

		Int2 contentSize = GUIHelper::calcOptimalContentsSize(mText, *mStyle, _getLayoutOptions());
		UINT32 contentWidth = std::max(imageWidth, (UINT32)contentSize.x);
		UINT32 contentHeight = std::max(imageHeight, (UINT32)contentSize.y);

		return Int2(contentWidth, contentHeight);
	}

	CM::Int2 GUIInputBox::_getTextInputOffset() const
	{
		return mTextOffset;	
	}

	CM::Rect GUIInputBox::_getTextInputRect() const
	{
		Rect textBounds = getContentBounds();
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
		Int2 offset = renderElemToOffset(renderElementIdx);
		Rect clipRect = renderElemToClipRect(renderElementIdx);

		sprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, localRenderElementIdx, offset, clipRect);
	}

	bool GUIInputBox::mouseEvent(const GUIMouseEvent& ev)
	{
		static UINT32 dbg = 0;

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
		else if(ev.getType() == GUIMouseEventType::MouseDown)
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
			
			if(ev.getKey() == BC_LEFT)
			{
				if(ev.isShiftDown())
				{
					if(!mSelectionShown)
						showSelection(gGUIManager().getInputCaretTool()->getCaretPos());
				}
				else
					clearSelection();

				gGUIManager().getInputCaretTool()->moveCaretLeft();

				if(ev.isShiftDown())
					gGUIManager().getInputSelectionTool()->moveSelectionToCaret(gGUIManager().getInputCaretTool()->getCaretPos());

				scrollTextToCaret();
				markContentAsDirty();
				return true;
			}

			if(ev.getKey() == BC_RIGHT)
			{
				if(ev.isShiftDown())
				{
					if(!mSelectionShown)
						showSelection(gGUIManager().getInputCaretTool()->getCaretPos());
				}
				else
					clearSelection();

				gGUIManager().getInputCaretTool()->moveCaretRight();

				if(ev.isShiftDown())
					gGUIManager().getInputSelectionTool()->moveSelectionToCaret(gGUIManager().getInputCaretTool()->getCaretPos());

				scrollTextToCaret();
				markContentAsDirty();
				return true;
			}

			if(ev.getKey() == BC_UP)
			{
				if(ev.isShiftDown())
				{
					if(!mSelectionShown)
						showSelection(gGUIManager().getInputCaretTool()->getCaretPos());
				}
				else
					clearSelection();

				gGUIManager().getInputCaretTool()->moveCaretUp();
				
				if(ev.isShiftDown())
					gGUIManager().getInputSelectionTool()->moveSelectionToCaret(gGUIManager().getInputCaretTool()->getCaretPos());

				scrollTextToCaret();
				markContentAsDirty();
				return true;
			}

			if(ev.getKey() == BC_DOWN)
			{
				if(ev.isShiftDown())
				{
					if(!mSelectionShown)
						showSelection(gGUIManager().getInputCaretTool()->getCaretPos());
				}
				else
					clearSelection();

				gGUIManager().getInputCaretTool()->moveCaretDown();
				
				if(ev.isShiftDown())
					gGUIManager().getInputSelectionTool()->moveSelectionToCaret(gGUIManager().getInputCaretTool()->getCaretPos());

				scrollTextToCaret();
				markContentAsDirty();
				return true;
			}

			if(ev.getKey() == BC_RETURN)
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

			if(ev.getKey() == BC_A && ev.isCtrlDown())
			{
				showSelection(0);
				gGUIManager().getInputSelectionTool()->selectAll();

				markContentAsDirty();
				return true;
			}
		}
		else if(ev.getType() == GUIKeyEventType::TextInput)
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

		return false;
	}

	bool GUIInputBox::commandEvent(const GUICommandEvent& ev)
	{
		if(ev.getType() == GUICommandEventType::Redraw)
		{
			markMeshAsDirty();
			return true;
		}

		return false;
	}

	void GUIInputBox::showCaret()
	{
		mCaretShown = true;

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Int2 offset = getTextOffset();
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
		Int2 offset = getTextOffset();
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

		Int2 textOffset = getTextOffset();
		Int2 caretPos = gGUIManager().getInputCaretTool()->getCaretPosition(textOffset);
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
		gGUIManager().getInputCaretTool()->updateText(this, textDesc);
		gGUIManager().getInputSelectionTool()->updateText(this, textDesc);

		markContentAsDirty();
	}

	void GUIInputBox::insertChar(CM::UINT32 charIdx, CM::UINT32 charCode)
	{
		mText.insert(mText.begin() + charIdx, charCode);

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Int2 offset = getTextOffset();

		gGUIManager().getInputCaretTool()->updateText(this, textDesc);
		gGUIManager().getInputSelectionTool()->updateText(this, textDesc);
	}

	void GUIInputBox::eraseChar(CM::UINT32 charIdx)
	{
		mText.erase(charIdx, 1);

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Int2 offset = getTextOffset();

		gGUIManager().getInputCaretTool()->updateText(this, textDesc);
		gGUIManager().getInputSelectionTool()->updateText(this, textDesc);
	}

	void GUIInputBox::deleteSelectedText()
	{
		UINT32 selStart = gGUIManager().getInputSelectionTool()->getSelectionStart();
		mText.erase(mText.begin() + selStart, mText.begin() + gGUIManager().getInputSelectionTool()->getSelectionEnd());

		TEXT_SPRITE_DESC textDesc = getTextDesc();
		Int2 offset = getTextOffset();
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

	CM::Int2 GUIInputBox::getTextOffset() const
	{
		Rect textBounds = getContentBounds();
		return Int2(textBounds.x, textBounds.y) + mTextOffset;
	}

	CM::Rect GUIInputBox::getTextClipRect() const
	{
		Rect contentClipRect = getContentClipRect();
		return Rect(contentClipRect.x - mTextOffset.x, contentClipRect.y - mTextOffset.y, contentClipRect.width, contentClipRect.height);
	}

	TEXT_SPRITE_DESC GUIInputBox::getTextDesc() const
	{
		TEXT_SPRITE_DESC textDesc;
		textDesc.text = mText;
		textDesc.font = mStyle->font;
		textDesc.fontSize = mStyle->fontSize;

		Rect textBounds = getContentBounds();
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

	const GUIContextMenu* GUIInputBox::getContextMenu() const
	{
		static bool initialized = false;
		static GUIContextMenu mContextMenu;

		if(!initialized)
		{
			mContextMenu.addMenuItem(L"Cut", boost::bind(&GUIInputBox::cutText, const_cast<GUIInputBox*>(this)));
			mContextMenu.addMenuItem(L"Copy", boost::bind(&GUIInputBox::copyText, const_cast<GUIInputBox*>(this)));
			mContextMenu.addMenuItem(L"Paste", boost::bind(&GUIInputBox::pasteText, const_cast<GUIInputBox*>(this)));

			// DEBUG ONLY
			
			mContextMenu.addSeparator(L"");
			mContextMenu.addMenuItem(L"DebugBox/Test1", boost::bind(&GUIInputBox::pasteText, const_cast<GUIInputBox*>(this)));
			mContextMenu.addMenuItem(L"DebugBox/Test2", boost::bind(&GUIInputBox::pasteText, const_cast<GUIInputBox*>(this)));
			mContextMenu.addMenuItem(L"Zzzz/Test1", boost::bind(&GUIInputBox::pasteText, const_cast<GUIInputBox*>(this)));

			initialized = true;
		}

		return &mContextMenu;
	}

	void GUIInputBox::cutText()
	{
		// TODO
	}

	void GUIInputBox::copyText()
	{
		// TODO
	}

	void GUIInputBox::pasteText()
	{
		// TODO
	}
}
#include "BsGUIInputCaret.h"
#include "BsGUIManager.h"
#include "BsImageSprite.h"
#include "CmFont.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIInputCaret::GUIInputCaret(const TEXT_SPRITE_DESC& textDesc)
		:mCaretPos(0), mTextDesc(textDesc)
	{
		mCaretSprite = cm_new<ImageSprite, PoolAlloc>();
		mTextSprite = cm_new<TextSprite, PoolAlloc>();

		mTextSprite->update(mTextDesc);
	}

	GUIInputCaret::~GUIInputCaret()
	{
		cm_delete<PoolAlloc>(mCaretSprite);
		cm_delete<PoolAlloc>(mTextSprite);
	}

	void GUIInputCaret::updateText(const TEXT_SPRITE_DESC& textDesc)
	{
		mTextDesc = textDesc;
		mTextDesc.clipRect = Rect(0, 0, 0, 0); // No clipping otherwise we don't know position of chars
		// outside of the element, which is something we need when moving the cursor

		mTextSprite->update(mTextDesc);
	}

	void GUIInputCaret::updateSprite(const CM::Int2& offset)
	{
		IMAGE_SPRITE_DESC mCaretDesc;
		mCaretDesc.offset = getCaretPosition(mTextDesc.offset);
		mCaretDesc.width = 1;
		mCaretDesc.height = getCaretHeight();
		mCaretDesc.clipRect = Rect(-mCaretDesc.offset.x + mTextDesc.offset.x - offset.x, -mCaretDesc.offset.y + mTextDesc.offset.y - offset.y, 
			mTextDesc.width + 1, mTextDesc.height); // Increase clip size by 1, so we can fit the caret in case it is fully at the end of the text
		mCaretDesc.texture = GUIManager::instance().getCaretTexture();

		mCaretSprite->update(mCaretDesc);
	}

	void GUIInputCaret::moveCaretToStart()
	{
		mCaretPos = 0;
	}

	void GUIInputCaret::moveCaretToEnd()
	{
		mCaretPos = getMaxCaretPos();
	}

	void GUIInputCaret::moveCaretLeft()
	{
		mCaretPos = (UINT32)std::max(0, (INT32)mCaretPos - 1);
	}

	void GUIInputCaret::moveCaretRight()
	{
		UINT32 maxCaretPos = getMaxCaretPos();

		mCaretPos = std::min(maxCaretPos, mCaretPos + 1);
	}

	void GUIInputCaret::moveCaretUp()
	{
		UINT32 charIdx = getCharIdxAtCaretPos();
		if(charIdx > 0)
			charIdx -= 1;	

		UINT32 lineIdx = mTextSprite->getLineForChar(charIdx);
		const SpriteLineDesc& desc = mTextSprite->getLineDesc(lineIdx);
		// If char is a newline, I want that to count as being on the next line because that's
		// how user sees it
		if(desc.isNewline(charIdx))
			lineIdx++;	

		if(lineIdx == 0)
			return;

		Int2 caretCoords = getCaretPosition(mTextDesc.offset);
		caretCoords.y -= getCaretHeight();

		moveCaretToPos(caretCoords);
	}

	void GUIInputCaret::moveCaretDown()
	{
		UINT32 charIdx = getCharIdxAtCaretPos();
		if(charIdx > 0)
			charIdx -= 1;	

		UINT32 lineIdx = mTextSprite->getLineForChar(charIdx);
		const SpriteLineDesc& desc = mTextSprite->getLineDesc(lineIdx);
		// If char is a newline, I want that to count as being on the next line because that's
		// how user sees it
		if(desc.isNewline(charIdx))
			lineIdx++;					  

		if(lineIdx == (mTextSprite->getNumLines() - 1))
			return;

		Int2 caretCoords = getCaretPosition(mTextDesc.offset);
		caretCoords.y += getCaretHeight();

		moveCaretToPos(caretCoords);
	}

	void GUIInputCaret::moveCaretToPos(const CM::Int2& pos)
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

				if(pos.y >= line.getLineYStart() && pos.y < (line.getLineYStart() + (INT32)line.getLineHeight()))
				{
					mCaretPos = curPos;
					return;
				}

				UINT32 numChars = line.getEndChar(false) - line.getStartChar() + 1; // +1 For extra line start position
				curPos += numChars;
			}

			mCaretPos = curPos;
		}
	}

	void GUIInputCaret::moveCaretToChar(UINT32 charIdx, CaretPos caretPos)
	{
		if(charIdx >= (UINT32)mTextDesc.text.size())
		{
			mCaretPos = 0;
			return;
		}

		UINT32 numLines = mTextSprite->getNumLines();
		UINT32 curPos = 0;
		UINT32 curCharIdx = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);
		
			curPos++; // Move past line start position

			UINT32 numChars = lineDesc.getEndChar() - lineDesc.getStartChar();
			UINT32 numCaretPositions = lineDesc.getEndChar(false) - lineDesc.getStartChar();
			if(charIdx >= (curCharIdx + numChars))
			{
				curCharIdx += numChars;
				curPos += numCaretPositions;
				continue;
			}

			UINT32 diff = charIdx - curCharIdx;

			if(caretPos == CARET_BEFORE)
				curPos += diff - 1;
			else
				curPos += diff;

			break;
		}

		mCaretPos = curPos;
	}

	UINT32 GUIInputCaret::getCharIdxAtCaretPos() const
	{
		if(mTextDesc.text.size() == 0)
			return 0;

		UINT32 numLines = mTextSprite->getNumLines();
		UINT32 curPos = 0;
		UINT32 curCharIdx = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);

			if(curPos == mCaretPos)
				return lineDesc.getStartChar();

			curPos++; // Move past line start position

			UINT32 numChars = lineDesc.getEndChar() - lineDesc.getStartChar();
			UINT32 numCaretPositions = lineDesc.getEndChar(false) - lineDesc.getStartChar();
			if(mCaretPos >= (curPos + numCaretPositions))
			{
				curCharIdx += numChars;
				curPos += numCaretPositions;
				continue;
			}

			UINT32 diff = mCaretPos - curPos; 
			curCharIdx += diff + 1; // Character after the caret

			return curCharIdx;
		}

		return 0;
	}

	Int2 GUIInputCaret::getCaretPosition(const CM::Int2& offset) const
	{
		if(mTextDesc.text.size() > 0)
		{
			UINT32 curPos = 0;
			UINT32 numLines = mTextSprite->getNumLines();
			for(UINT32 i = 0; i < numLines; i++)
			{
				const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);

				if(mCaretPos == curPos)
				{
					// Caret is on line start
					return Int2(offset.x, lineDesc.getLineYStart());
				}

				curPos += lineDesc.getEndChar(false) - lineDesc.getStartChar() + 1; // + 1 for special line start position
			}

			UINT32 charIdx = getCharIdxAtCaretPos();
			if(charIdx > 0)
				charIdx -= 1;			

			charIdx = std::min((UINT32)(mTextDesc.text.size() - 1), charIdx);

			Rect charRect = mTextSprite->getCharRect(charIdx);
			UINT32 lineIdx = mTextSprite->getLineForChar(charIdx);
			UINT32 yOffset = mTextSprite->getLineDesc(lineIdx).getLineYStart();

			return Int2(charRect.x + charRect.width, yOffset);
		}
		else
		{
			return offset;
		}		
	}

	UINT32 GUIInputCaret::getCaretHeight() const
	{
		UINT32 charIdx = getCharIdxAtCaretPos();
		if(charIdx > 0)
			charIdx -= 1;	

		if(charIdx < (UINT32)mTextDesc.text.size())
		{
			UINT32 lineIdx = mTextSprite->getLineForChar(charIdx);
			return mTextSprite->getLineDesc(lineIdx).getLineHeight();
		}
		else
		{
			if(mTextDesc.font != nullptr)
			{
				UINT32 nearestSize = mTextDesc.font->getClosestAvailableSize(mTextDesc.fontSize);
				const FontData* fontData = mTextDesc.font->getFontDataForSize(nearestSize);

				if(fontData != nullptr)
					return fontData->fontDesc.lineHeight;
			}
		}

		return 0;
	}

	bool GUIInputCaret::isCaretAtNewline() const
	{
		if(mTextDesc.text.size() == 0)
			return true;

		UINT32 numLines = mTextSprite->getNumLines();
		UINT32 curPos = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);

			if(curPos == mCaretPos)
				return true;

			UINT32 numChars = lineDesc.getEndChar(false) - lineDesc.getStartChar();
			curPos += numChars;
		}

		return false;
	}

	UINT32 GUIInputCaret::getMaxCaretPos() const
	{
		if(mTextDesc.text.size() == 0)
			return 0;

		UINT32 numLines = mTextSprite->getNumLines();
		UINT32 maxPos = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);

			UINT32 numChars = lineDesc.getEndChar(false) - lineDesc.getStartChar() + 1; // + 1 for special line start position
			maxPos += numChars;
		}

		return maxPos - 1;
	}
}
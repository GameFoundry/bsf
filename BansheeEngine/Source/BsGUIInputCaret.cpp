#include "BsGUIInputCaret.h"
#include "BsGUIManager.h"
#include "BsImageSprite.h"
#include "CmFont.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIInputCaret::GUIInputCaret()
		:mCaretPos(0)
	{
		mCaretSprite = cm_new<ImageSprite, PoolAlloc>();
	}

	GUIInputCaret::~GUIInputCaret()
	{
		cm_delete<PoolAlloc>(mCaretSprite);
	}

	Int2 GUIInputCaret::getSpriteOffset() const
	{
		return getCaretPosition(mTextOffset);
	}

	Rect GUIInputCaret::getSpriteClipRect(const CM::Rect& parentClipRect) const
	{
		Int2 offset = getSpriteOffset();

		Rect clipRect(-offset.x + mTextOffset.x - mClipOffset.x, -offset.y + mTextOffset.y - mClipOffset.y, 
			mTextDesc.width, mTextDesc.height);

		Rect localParentCliprect = parentClipRect;

		// Move parent rect to our space
		localParentCliprect.x += clipRect.x;
		localParentCliprect.y += clipRect.y;

		// Clip our rectangle so its not larger then the parent
		clipRect.clip(localParentCliprect);

		// Increase clip size by 1, so we can fit the caret in case it is fully at the end of the text
		clipRect.width += 1;

		return clipRect;
	}

	void GUIInputCaret::updateSprite()
	{
		IMAGE_SPRITE_DESC mCaretDesc;
		mCaretDesc.width = 1;
		mCaretDesc.height = getCaretHeight();
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

		UINT32 lineIdx = getLineForChar(charIdx);
		const GUIInputLineDesc& desc = getLineDesc(lineIdx);
		// If char is a newline, I want that to count as being on the next line because that's
		// how user sees it
		if(desc.isNewline(charIdx))
			lineIdx++;	

		if(lineIdx == 0)
		{
			moveCaretToStart();
			return;
		}

		Int2 caretCoords = getCaretPosition(mTextOffset);
		caretCoords.y -= getCaretHeight();

		moveCaretToPos(caretCoords);
	}

	void GUIInputCaret::moveCaretDown()
	{
		UINT32 charIdx = getCharIdxAtCaretPos();
		if(charIdx > 0)
			charIdx -= 1;	

		UINT32 lineIdx = getLineForChar(charIdx);
		const GUIInputLineDesc& desc = getLineDesc(lineIdx);
		// If char is a newline, I want that to count as being on the next line because that's
		// how user sees it
		if(desc.isNewline(charIdx))
			lineIdx++;					  

		if(lineIdx == (getNumLines() - 1))
		{
			moveCaretToEnd();
			return;
		}

		Int2 caretCoords = getCaretPosition(mTextOffset);
		caretCoords.y += getCaretHeight();

		moveCaretToPos(caretCoords);
	}

	void GUIInputCaret::moveCaretToPos(const CM::Int2& pos)
	{
		INT32 charIdx = getCharIdxAtPos(pos);

		if(charIdx != -1)
		{
			Rect charRect = getCharRect(charIdx);

			float xCenter = charRect.x + charRect.width * 0.5f;
			if(pos.x <= xCenter)
				moveCaretToChar(charIdx, CARET_BEFORE);
			else
				moveCaretToChar(charIdx, CARET_AFTER);
		}
		else
		{
			UINT32 numLines = getNumLines();

			if(numLines == 0)
			{
				mCaretPos = 0;
				return;
			}

			UINT32 curPos = 0;
			for(UINT32 i = 0; i < numLines; i++)
			{
				const GUIInputLineDesc& line = getLineDesc(i);

				if(pos.y >= line.getLineYStart() && pos.y < (line.getLineYStart() + (INT32)line.getLineHeight()))
				{
					mCaretPos = curPos;
					return;
				}

				UINT32 numChars = line.getEndChar(false) - line.getStartChar() + 1; // +1 For extra line start position
				curPos += numChars;
			}

			const GUIInputLineDesc& firstLine = getLineDesc(0);

			if(pos.y < firstLine.getLineYStart()) // Before first line
				mCaretPos = 0;
			else // After last line
				mCaretPos = curPos - 1;
		}
	}

	void GUIInputCaret::moveCaretToChar(UINT32 charIdx, CaretPos caretPos)
	{
		if(charIdx >= (UINT32)mTextDesc.text.size())
		{
			mCaretPos = 0;
			return;
		}

		UINT32 numLines = getNumLines();
		UINT32 curPos = 0;
		UINT32 curCharIdx = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const GUIInputLineDesc& lineDesc = getLineDesc(i);
		
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
		return getCharIdxAtInputIdx(mCaretPos);
	}

	Int2 GUIInputCaret::getCaretPosition(const CM::Int2& offset) const
	{
		if(mTextDesc.text.size() > 0)
		{
			UINT32 curPos = 0;
			UINT32 numLines = getNumLines();
			for(UINT32 i = 0; i < numLines; i++)
			{
				const GUIInputLineDesc& lineDesc = getLineDesc(i);

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

			Rect charRect = getCharRect(charIdx);
			UINT32 lineIdx = getLineForChar(charIdx);
			UINT32 yOffset = getLineDesc(lineIdx).getLineYStart();

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
			UINT32 lineIdx = getLineForChar(charIdx);
			return getLineDesc(lineIdx).getLineHeight();
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
		return isNewline(mCaretPos);
	}

	UINT32 GUIInputCaret::getMaxCaretPos() const
	{
		if(mTextDesc.text.size() == 0)
			return 0;

		UINT32 numLines = getNumLines();
		UINT32 maxPos = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const GUIInputLineDesc& lineDesc = getLineDesc(i);

			UINT32 numChars = lineDesc.getEndChar(false) - lineDesc.getStartChar() + 1; // + 1 for special line start position
			maxPos += numChars;
		}

		return maxPos - 1;
	}
}
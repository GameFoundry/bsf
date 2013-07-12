#include "BsGUIInputCaret.h"
#include "BsGUIManager.h"
#include "BsImageSprite.h"
#include "CmFont.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIInputCaret::GUIInputCaret(const TEXT_SPRITE_DESC& textDesc, const Int2& offset, const Int2 clipOffset)
		:mCaretPos(0), mTextDesc(textDesc), mTextOffset(offset), mClipOffset(clipOffset), mQuads(nullptr), mNumQuads(0)
	{
		mCaretSprite = cm_new<ImageSprite, PoolAlloc>();

		updateText(textDesc, offset, clipOffset);
	}

	GUIInputCaret::~GUIInputCaret()
	{
		cm_delete<PoolAlloc>(mCaretSprite);
	}

	void GUIInputCaret::updateText(const TEXT_SPRITE_DESC& textDesc, const Int2& offset, const Int2 clipOffset)
	{
		mTextDesc = textDesc;
		mTextOffset = offset;
		mClipOffset = clipOffset;

		mLineDescs.clear();

		std::shared_ptr<TextUtility::TextData> textData = TextUtility::getTextData(mTextDesc.text, mTextDesc.font, mTextDesc.fontSize, 
			mTextDesc.width, mTextDesc.height, mTextDesc.wordWrap);

		if(textData == nullptr)
			return;

		const CM::Vector<TextUtility::TextLine>::type& lines = textData->getLines();
		const CM::Vector<UINT32>::type& quadsPerPage = textData->getNumQuadsPerPage();

		mNumQuads = 0;
		for(auto& numQuads : quadsPerPage)
			mNumQuads += numQuads;

		if(mQuads != nullptr)
			cm_delete<ScratchAlloc>(mQuads);
		
		mQuads = cm_newN<Vector2, ScratchAlloc>(mNumQuads * 4);

		TextSprite::genTextQuads(*textData, mTextDesc.width, mTextDesc.height, mTextDesc.horzAlign, mTextDesc.vertAlign, mTextDesc.anchor, 
			mQuads, nullptr, nullptr, mNumQuads);

		UINT32 numVerts = mNumQuads * 4;
		Vector2 vecOffset(mTextOffset.x, mTextOffset.y);
		for(UINT32 i = 0; i < numVerts; i++)
			mQuads[i] = mQuads[i] + vecOffset;

		// Store cached line data
		UINT32 curCharIdx = 0;
		UINT32 cachedLineY = 0;
		UINT32 curLineIdx = 0;
		Vector<Int2>::type alignmentOffsets = TextSprite::getAlignmentOffsets(lines, mTextDesc.width, 
			mTextDesc.height, mTextDesc.horzAlign, mTextDesc.vertAlign);

		for(auto& line : lines)
		{
			// Line has a newline char only if it wasn't created by word wrap and it isn't the last line
			bool hasNewline = line.hasNewlineChar() && (curLineIdx != ((UINT32)lines.size() - 1));

			UINT32 startChar = curCharIdx;
			UINT32 endChar = curCharIdx + line.getNumChars() + (hasNewline ? 1 : 0);
			UINT32 lineHeight = line.getYOffset();
			INT32 lineYStart = alignmentOffsets[curLineIdx].y + mTextOffset.y;

			GUIInputLineDesc lineDesc(startChar, endChar, lineHeight, lineYStart, hasNewline);
			mLineDescs.push_back(lineDesc);

			curCharIdx = lineDesc.getEndChar();
			cachedLineY += lineDesc.getLineHeight();
			curLineIdx++;
		}
	}

	Int2 GUIInputCaret::getSpriteOffset() const
	{
		return getCaretPosition(mTextOffset);
	}

	Rect GUIInputCaret::getSpriteClipRect() const
	{
		Int2 offset = getSpriteOffset();

		return Rect(-offset.x + mTextOffset.x - mClipOffset.x, -offset.y + mTextOffset.y - mClipOffset.y, 
			mTextDesc.width + 1, mTextDesc.height); // Increase clip size by 1, so we can fit the caret in case it is fully at the end of the text
	}

	void GUIInputCaret::updateSprite(const CM::Int2& offset)
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
		return getCharIdxAtCaretPos(mCaretPos);
	}

	UINT32 GUIInputCaret::getCharIdxAtCaretPos(UINT32 caretPos) const
	{
		if(mTextDesc.text.size() == 0)
			return 0;

		UINT32 numLines = getNumLines();
		UINT32 curPos = 0;
		UINT32 curCharIdx = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const GUIInputLineDesc& lineDesc = getLineDesc(i);

			if(curPos == caretPos)
				return lineDesc.getStartChar();

			curPos++; // Move past line start position

			UINT32 numChars = lineDesc.getEndChar() - lineDesc.getStartChar();
			UINT32 numCaretPositions = lineDesc.getEndChar(false) - lineDesc.getStartChar();
			if(caretPos >= (curPos + numCaretPositions))
			{
				curCharIdx += numChars;
				curPos += numCaretPositions;
				continue;
			}

			UINT32 diff = caretPos - curPos; 
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
		if(mTextDesc.text.size() == 0)
			return true;

		UINT32 numLines = getNumLines();
		UINT32 curPos = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const GUIInputLineDesc& lineDesc = getLineDesc(i);

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

	CM::Rect GUIInputCaret::getCharRect(UINT32 charIdx) const
	{
		UINT32 lineIdx = getLineForChar(charIdx);

		// If char is newline we don't have any geometry to return
		const GUIInputLineDesc& lineDesc = getLineDesc(lineIdx);
		if(lineDesc.isNewline(charIdx))
			return Rect();

		UINT32 numNewlineChars = 0;
		for(UINT32 i = 0; i < lineIdx; i++)
			numNewlineChars += (getLineDesc(i).hasNewlineChar() ? 1 : 0);

		UINT32 quadIdx = charIdx - numNewlineChars;
		if(quadIdx >= 0 && quadIdx < mNumQuads)
		{
			UINT32 vertIdx = quadIdx * 4;

			Rect charRect;
			charRect.x = Math::RoundToInt(mQuads[vertIdx + 0].x);
			charRect.y = Math::RoundToInt(mQuads[vertIdx + 0].y);
			charRect.width = Math::RoundToInt(mQuads[vertIdx + 3].x - charRect.x);
			charRect.height = Math::RoundToInt(mQuads[vertIdx + 3].y - charRect.y);

			return charRect;
		}

		CM_EXCEPT(InternalErrorException, "Invalid character index: " + toString(charIdx));
	}

	INT32 GUIInputCaret::getCharIdxAtPos(const Int2& pos) const
	{
		Vector2 vecPos((float)pos.x, (float)pos.y);

		UINT32 lineStartChar = 0;
		UINT32 lineEndChar = 0;
		UINT32 numNewlineChars = 0;
		UINT32 lineIdx = 0;
		for(auto& line : mLineDescs)
		{
			if(pos.y >= line.getLineYStart() && pos.y < (line.getLineYStart() + (INT32)line.getLineHeight()))
			{
				lineStartChar = line.getStartChar();
				lineEndChar = line.getEndChar(false);
				break;
			}

			// Newline chars count in the startChar/endChar variables, but don't actually exist in the buffers
			// so we need to filter them out
			numNewlineChars += (line.hasNewlineChar() ? 1 : 0); 

			lineIdx++;
		}

		UINT32 lineStartQuad = lineStartChar - numNewlineChars;
		UINT32 lineEndQuad = lineEndChar - numNewlineChars;

		float nearestDist = std::numeric_limits<float>::max();
		UINT32 nearestChar = 0;
		bool foundChar = false;

		for(UINT32 i = lineStartQuad; i < lineEndQuad; i++)
		{
			UINT32 curVert = i * 4;

			float centerX = mQuads[curVert + 0].x + mQuads[curVert + 1].x;
			centerX *= 0.5f;

			float dist = Math::Abs(centerX - vecPos.x);
			if(dist < nearestDist)
			{
				nearestChar = i + numNewlineChars;
				nearestDist = dist;
				foundChar = true;
			}
		}

		if(!foundChar)
			return -1;

		return nearestChar;
	}

	CM::UINT32 GUIInputCaret::getLineForChar(CM::UINT32 charIdx, bool newlineCountsOnNextLine) const
	{
		UINT32 idx = 0;
		for(auto& line : mLineDescs)
		{
			if(charIdx >= line.getStartChar() && charIdx < line.getEndChar())
			{
				if(line.isNewline(charIdx) && newlineCountsOnNextLine)
					return idx + 1; // Incrementing is safe because next line must exist, since we just found a newline char

				return idx;
			}

			idx++;
		}

		CM_EXCEPT(InternalErrorException, "Invalid character index: " + toString(charIdx));
	}

	GUIInputLineDesc::GUIInputLineDesc(CM::UINT32 startChar, CM::UINT32 endChar, CM::UINT32 lineHeight, CM::INT32 lineYStart, bool includesNewline)
		:mStartChar(startChar), mEndChar(endChar), mLineHeight(lineHeight), mLineYStart(lineYStart), mIncludesNewline(includesNewline)
	{

	}

	UINT32 GUIInputLineDesc::getEndChar(bool includeNewline) const
	{
		if(mIncludesNewline)
		{
			if(includeNewline)
				return mEndChar;
			else
			{
				if(mEndChar > 0)
					return mEndChar - 1;
				else
					return mStartChar;
			}
		}
		else
			return mEndChar;
	}

	bool GUIInputLineDesc::isNewline(UINT32 charIdx) const
	{
		if(mIncludesNewline)
		{
			return (mEndChar - 1) == charIdx;
		}
		else
			return false;
	}
}
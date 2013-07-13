#include "BsGUIInputTool.h"
#include "CmMath.h"
#include "CmVector2.h"
#include "CmFont.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIInputTool::GUIInputTool(const TEXT_SPRITE_DESC& textDesc, const Int2& offset, const Int2 clipOffset)
		:mTextDesc(textDesc), mTextOffset(offset), mClipOffset(clipOffset), mQuads(nullptr), mNumQuads(0)
	{
		updateText(textDesc, offset, clipOffset);
	}

	GUIInputTool::~GUIInputTool()
	{ }

	void GUIInputTool::updateText(const TEXT_SPRITE_DESC& textDesc, const Int2& offset, const Int2 clipOffset)
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
		Vector2 vecOffset((float)mTextOffset.x, (float)mTextOffset.y);
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

	CM::Rect GUIInputTool::getCharRect(UINT32 charIdx) const
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

	INT32 GUIInputTool::getCharIdxAtPos(const Int2& pos) const
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

	CM::UINT32 GUIInputTool::getLineForChar(CM::UINT32 charIdx, bool newlineCountsOnNextLine) const
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

	UINT32 GUIInputTool::getCharIdxAtInputIdx(UINT32 inputIdx) const
	{
		if(mTextDesc.text.size() == 0)
			return 0;

		UINT32 numLines = getNumLines();
		UINT32 curPos = 0;
		UINT32 curCharIdx = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const GUIInputLineDesc& lineDesc = getLineDesc(i);

			if(curPos == inputIdx)
				return lineDesc.getStartChar();

			curPos++; // Move past line start position

			UINT32 numChars = lineDesc.getEndChar() - lineDesc.getStartChar();
			UINT32 numCaretPositions = lineDesc.getEndChar(false) - lineDesc.getStartChar();
			if(inputIdx >= (curPos + numCaretPositions))
			{
				curCharIdx += numChars;
				curPos += numCaretPositions;
				continue;
			}

			UINT32 diff = inputIdx - curPos; 
			curCharIdx += diff + 1; // Character after the caret

			return curCharIdx;
		}

		return 0;
	}
	 
	bool GUIInputTool::isNewlineBefore(CM::UINT32 inputIdx) const
	{
		UINT32 charIdx = getCharIdxAtInputIdx(inputIdx);
		charIdx = (UINT32)std::max(0, (INT32)(charIdx - 1));

		return isNewlineChar(charIdx);
	}

	bool GUIInputTool::isNewlineAfter(CM::UINT32 inputIdx) const
	{
		UINT32 charIdx = getCharIdxAtInputIdx(inputIdx);
		return isNewlineChar(charIdx);
	}

	bool GUIInputTool::isNewline(CM::UINT32 inputIdx) const
	{
		if(mTextDesc.text.size() == 0)
			return true;

		UINT32 numLines = getNumLines();
		UINT32 curPos = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const GUIInputLineDesc& lineDesc = getLineDesc(i);

			if(curPos == inputIdx)
				return true;

			UINT32 numChars = lineDesc.getEndChar(false) - lineDesc.getStartChar();
			curPos += numChars;
		}

		return false;
	}

	bool GUIInputTool::isNewlineChar(CM::UINT32 charIdx) const
	{
		if(mTextDesc.text[charIdx] == '\n')
			return true;

		return false;
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
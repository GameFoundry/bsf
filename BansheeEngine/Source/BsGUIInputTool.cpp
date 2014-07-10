//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGUIInputTool.h"
#include "BsGUIElement.h"
#include "BsMath.h"
#include "BsVector2.h"
#include "BsFont.h"

namespace BansheeEngine
{
	GUIInputTool::GUIInputTool()
		:mQuads(nullptr), mNumQuads(0), mElement(nullptr)
	{ }

	GUIInputTool::~GUIInputTool()
	{ }

	void GUIInputTool::updateText(const GUIElement* element, const TEXT_SPRITE_DESC& textDesc)
	{
		mElement = element;
		mTextDesc = textDesc;

		mLineDescs.clear();

		TextData textData(mTextDesc.text, mTextDesc.font, mTextDesc.fontSize, 
			mTextDesc.width, mTextDesc.height, mTextDesc.wordWrap);

		UINT32 numLines = textData.getNumLines();
		UINT32 numPages = textData.getNumPages();

		mNumQuads = 0;
		for(UINT32 i = 0; i < numPages; i++)
			mNumQuads += textData.getNumQuadsForPage(i);

		if(mQuads != nullptr)
			bs_delete<ScratchAlloc>(mQuads);

		mQuads = bs_newN<Vector2, ScratchAlloc>(mNumQuads * 4);

		TextSprite::genTextQuads(textData, mTextDesc.width, mTextDesc.height, mTextDesc.horzAlign, mTextDesc.vertAlign, mTextDesc.anchor, 
			mQuads, nullptr, nullptr, mNumQuads);

		UINT32 numVerts = mNumQuads * 4;

		// Store cached line data
		UINT32 curCharIdx = 0;
		UINT32 curLineIdx = 0;
		Vector<Vector2I> alignmentOffsets = TextSprite::getAlignmentOffsets(textData, mTextDesc.width, 
			mTextDesc.height, mTextDesc.horzAlign, mTextDesc.vertAlign);

		for(UINT32 i = 0; i < numLines; i++)
		{
			const TextData::TextLine& line = textData.getLine(i);

			// Line has a newline char only if it wasn't created by word wrap and it isn't the last line
			bool hasNewline = line.hasNewlineChar() && (curLineIdx != (numLines - 1));

			UINT32 startChar = curCharIdx;
			UINT32 endChar = curCharIdx + line.getNumChars() + (hasNewline ? 1 : 0);
			UINT32 lineHeight = line.getYOffset();
			INT32 lineYStart = alignmentOffsets[curLineIdx].y;

			GUIInputLineDesc lineDesc(startChar, endChar, lineHeight, lineYStart, hasNewline);
			mLineDescs.push_back(lineDesc);

			curCharIdx = lineDesc.getEndChar();
			curLineIdx++;
		}
	}

	Vector2I GUIInputTool::getTextOffset() const
	{
		return mElement->_getOffset() + mElement->_getTextInputOffset() + Vector2I(mElement->_getTextInputRect().x, mElement->_getTextInputRect().y);
	}

	RectI GUIInputTool::getCharRect(UINT32 charIdx) const
	{
		RectI charRect = getLocalCharRect(charIdx);
		Vector2I textOffset = getTextOffset();

		charRect.x += textOffset.x;
		charRect.y += textOffset.y;

		return charRect;
	}

	RectI GUIInputTool::getLocalCharRect(UINT32 charIdx) const
	{
		UINT32 lineIdx = getLineForChar(charIdx);

		// If char is newline we don't have any geometry to return
		const GUIInputLineDesc& lineDesc = getLineDesc(lineIdx);
		if(lineDesc.isNewline(charIdx))
			return RectI();

		UINT32 numNewlineChars = 0;
		for(UINT32 i = 0; i < lineIdx; i++)
			numNewlineChars += (getLineDesc(i).hasNewlineChar() ? 1 : 0);

		UINT32 quadIdx = charIdx - numNewlineChars;
		if(quadIdx >= 0 && quadIdx < mNumQuads)
		{
			UINT32 vertIdx = quadIdx * 4;

			RectI charRect;
			charRect.x = Math::roundToInt(mQuads[vertIdx + 0].x);
			charRect.y = Math::roundToInt(mQuads[vertIdx + 0].y);
			charRect.width = Math::roundToInt(mQuads[vertIdx + 3].x - charRect.x);
			charRect.height = Math::roundToInt(mQuads[vertIdx + 3].y - charRect.y);

			return charRect;
		}

		BS_EXCEPT(InternalErrorException, "Invalid character index: " + toString(charIdx));
	}

	INT32 GUIInputTool::getCharIdxAtPos(const Vector2I& pos) const
	{
		Vector2 vecPos((float)pos.x, (float)pos.y);

		UINT32 lineStartChar = 0;
		UINT32 lineEndChar = 0;
		UINT32 numNewlineChars = 0;
		UINT32 lineIdx = 0;
		for(auto& line : mLineDescs)
		{
			INT32 lineStart = line.getLineYStart() + getTextOffset().y;
			if(pos.y >= lineStart && pos.y < (lineStart + (INT32)line.getLineHeight()))
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

		Vector2I textOffset = getTextOffset();
		for(UINT32 i = lineStartQuad; i < lineEndQuad; i++)
		{
			UINT32 curVert = i * 4;

			float centerX = mQuads[curVert + 0].x + mQuads[curVert + 1].x;
			centerX *= 0.5f;
			centerX += textOffset.x;

			float dist = Math::abs(centerX - vecPos.x);
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

	UINT32 GUIInputTool::getLineForChar(UINT32 charIdx, bool newlineCountsOnNextLine) const
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

		BS_EXCEPT(InternalErrorException, "Invalid character index: " + toString(charIdx));
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

	bool GUIInputTool::isNewline(UINT32 inputIdx) const
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

	bool GUIInputTool::isNewlineChar(UINT32 charIdx) const
	{
		if(mTextDesc.text[charIdx] == '\n')
			return true;

		return false;
	}

	bool GUIInputTool::isDescValid() const
	{
		// We we have some text but line descs are empty we may assume
		// something went wrong when creating the line descs, therefore it is
		// not valid and no text is displayed.
		if(mTextDesc.text.size() > 0)
			return mLineDescs.size() > 0;

		return true;
	}

	GUIInputLineDesc::GUIInputLineDesc(UINT32 startChar, UINT32 endChar, UINT32 lineHeight, INT32 lineYStart, bool includesNewline)
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
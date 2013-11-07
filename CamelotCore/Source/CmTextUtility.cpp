#include "CmTextUtility.h"
#include "CmFont.h"
#include "CmVector2.h"
#include "CmDebug.h"

namespace CamelotFramework
{
	const int SPACE_CHAR = 32;

	void TextUtility::TextWord::init(bool spacer)
	{
		mWidth = mHeight = 0;
		mSpacer = spacer;
		mSpaceWidth = 0;
		mCharsStart = 0;
		mCharsEnd = 0;
		mLastChar = nullptr;
	}

	// Assumes charIdx is an index right after last char in the list (if any). All chars need to be sequential.
	UINT32 TextUtility::TextWord::addChar(UINT32 charIdx, const CHAR_DESC& desc)
	{
		UINT32 charWidth = desc.xAdvance;
		if(mLastChar != nullptr)
		{
			UINT32 kerning = 0;
			for(size_t j = 0; j < mLastChar->kerningPairs.size(); j++)
			{
				if(mLastChar->kerningPairs[j].otherCharId == desc.charId)
				{
					kerning = mLastChar->kerningPairs[j].amount;
					break;
				}
			}

			charWidth += kerning;
		}

		mWidth += charWidth;
		mHeight = std::max(mHeight, desc.height);

		if(mLastChar == nullptr) // First char
			mCharsStart = mCharsEnd = charIdx;
		else
			mCharsEnd = charIdx;

		mLastChar = &desc;

		return charWidth;
	}

	void TextUtility::TextWord::addSpace(UINT32 spaceWidth)
	{
		mSpaceWidth += spaceWidth;
		mWidth = mSpaceWidth;
		mHeight = 0;
	}

	void TextUtility::TextLine::init(TextData* textData)
	{
		mWidth = 0;
		mHeight = 0;
		mIsEmpty = true; 
		mTextData = textData;
		mWordsStart = mWordsEnd = 0;
	}

	void TextUtility::TextLine::finalize(bool hasNewlineChar)
	{
		mHasNewline = hasNewlineChar;
	}

	void TextUtility::TextLine::add(UINT32 charIdx, const CHAR_DESC& charDesc)
	{
		UINT32 charWidth = 0;
		if(mIsEmpty)
		{
			mWordsStart = mWordsEnd = allocWord(false);
			mIsEmpty = false;
		}
		else
		{
			if(TextUtility::WordBuffer[mWordsEnd].isSpacer())
				mWordsEnd = allocWord(false);
		}

		TextWord& lastWord = TextUtility::WordBuffer[mWordsEnd];
		charWidth = lastWord.addChar(charIdx, charDesc);

		mWidth += charWidth;
		mHeight = std::max(mHeight, lastWord.getHeight());
	}

	void TextUtility::TextLine::addSpace()
	{
		if(mIsEmpty)
		{
			mWordsStart = mWordsEnd = allocWord(true);
			mIsEmpty = false;
		}
		else
			mWordsEnd = allocWord(true); // Each space is counted as its own word, to make certain operations easier

		TextWord& lastWord = TextUtility::WordBuffer[mWordsEnd];
		lastWord.addSpace(mTextData->getSpaceWidth());

		mWidth += mTextData->getSpaceWidth();
	}

	// Assumes wordIdx is an index right after last word in the list (if any). All words need to be sequential.
	void TextUtility::TextLine::addWord(UINT32 wordIdx, const TextWord& word)
	{
		if(mIsEmpty)
		{
			mWordsStart = mWordsEnd = wordIdx;
			mIsEmpty = false;
		}
		else
			mWordsEnd = wordIdx;

		mWidth += word.getWidth();
		mHeight = std::max(mHeight, word.getHeight());
	}

	UINT32 TextUtility::TextLine::removeLastWord()
	{
		if(mIsEmpty)
		{
			assert(false);
			return 0;
		}

		UINT32 lastWord = mWordsEnd--;
		if(mWordsStart == lastWord)
		{
			mIsEmpty = true;
			mWordsStart = mWordsEnd = 0;
		}

		calculateBounds();

		return lastWord;
	}

	UINT32 TextUtility::TextLine::fillBuffer(UINT32 page, Vector2* vertices, Vector2* uvs, UINT32* indexes, UINT32 offset, UINT32 size) const
	{
		UINT32 numQuads = 0;

		if(mIsEmpty)
			return numQuads;

		UINT32 penX = 0;
		for(UINT32 i = mWordsStart; i <= mWordsEnd; i++)
		{
			const TextWord& word = mTextData->getWord(i);

			if(word.isSpacer())
			{
				// We store invisible space quads in the first page. Even though they aren't needed
				// for rendering and we could just leave an empty space, they are needed for intersection tests
				// for things like determining caret placement and selection areas
				if(page == 0) 
				{
					INT32 curX = penX;
					INT32 curY = 0;

					UINT32 curVert = offset * 4;
					UINT32 curIndex = offset * 6;

					vertices[curVert + 0] = Vector2((float)curX, (float)curY);
					vertices[curVert + 1] = Vector2((float)(curX + mTextData->getSpaceWidth()), (float)curY);
					vertices[curVert + 2] = Vector2((float)curX, (float)curY + (float)mTextData->getLineHeight());
					vertices[curVert + 3] = Vector2((float)(curX + mTextData->getSpaceWidth()), (float)curY + (float)mTextData->getLineHeight());

					if(uvs != nullptr)
					{
						uvs[curVert + 0] = Vector2(0.0f, 0.0f);
						uvs[curVert + 1] = Vector2(0.0f, 0.0f);
						uvs[curVert + 2] = Vector2(0.0f, 0.0f);
						uvs[curVert + 3] = Vector2(0.0f, 0.0f);
					}

					// Triangles are back-facing which makes them invisible
					if(indexes != nullptr)
					{
						indexes[curIndex + 0] = curVert + 0;
						indexes[curIndex + 1] = curVert + 2;
						indexes[curIndex + 2] = curVert + 1;
						indexes[curIndex + 3] = curVert + 1;
						indexes[curIndex + 4] = curVert + 2;
						indexes[curIndex + 5] = curVert + 3;
					}

					offset++;
					numQuads++;

					if(offset > size)
						CM_EXCEPT(InternalErrorException, "Out of buffer bounds. Buffer size: " + toString(size));
				}

				penX += mTextData->getSpaceWidth();
			}
			else
			{
				UINT32 kerning = 0;
				for(UINT32 j = word.getCharsStart(); j <= word.getCharsEnd(); j++)
				{
					const CHAR_DESC& curChar = mTextData->getChar(j);

					INT32 curX = penX + curChar.xOffset;
					INT32 curY = ((INT32) mTextData->getBaselineOffset() - curChar.yOffset);

					penX += curChar.xAdvance + kerning;
					
					kerning = 0;
					if((j + 1) <= word.getCharsEnd())
					{
						const CHAR_DESC& nextChar = mTextData->getChar(j + 1);
						for(size_t j = 0; j < curChar.kerningPairs.size(); j++)
						{
							if(curChar.kerningPairs[j].otherCharId == nextChar.charId)
							{
								kerning = curChar.kerningPairs[j].amount;
								break;
							}
						}
					}

					if(curChar.page != page)
						continue;

					UINT32 curVert = offset * 4;
					UINT32 curIndex = offset * 6;

					vertices[curVert + 0] = Vector2((float)curX, (float)curY);
					vertices[curVert + 1] = Vector2((float)(curX + curChar.width), (float)curY);
					vertices[curVert + 2] = Vector2((float)curX, (float)curY + (float)curChar.height);
					vertices[curVert + 3] = Vector2((float)(curX + curChar.width), (float)curY + (float)curChar.height);

					if(uvs != nullptr)
					{
						uvs[curVert + 0] = Vector2(curChar.uvX, curChar.uvY);
						uvs[curVert + 1] = Vector2(curChar.uvX + curChar.uvWidth, curChar.uvY);
						uvs[curVert + 2] = Vector2(curChar.uvX, curChar.uvY + curChar.uvHeight);
						uvs[curVert + 3] = Vector2(curChar.uvX + curChar.uvWidth, curChar.uvY + curChar.uvHeight);
					}

					if(indexes != nullptr)
					{
						indexes[curIndex + 0] = curVert + 0;
						indexes[curIndex + 1] = curVert + 1;
						indexes[curIndex + 2] = curVert + 2;
						indexes[curIndex + 3] = curVert + 1;
						indexes[curIndex + 4] = curVert + 3;
						indexes[curIndex + 5] = curVert + 2;
					}

					offset++;
					numQuads++;

					if(offset > size)
						CM_EXCEPT(InternalErrorException, "Out of buffer bounds. Buffer size: " + toString(size));
				}
			}
		}

		return numQuads;
	}

	UINT32 TextUtility::TextLine::getNumChars() const
	{
		if(mIsEmpty)
			return 0;

		UINT32 numChars = 0;
		for(UINT32 i = mWordsStart; i <= mWordsEnd; i++)
		{
			TextWord& word = TextUtility::WordBuffer[i];

			if(word.isSpacer())
				numChars++;
			else
				numChars += (UINT32)word.getNumChars();
		}

		return numChars;
	}

	void TextUtility::TextLine::calculateBounds()
	{
		mWidth = 0;
		mHeight = 0;

		if(mIsEmpty)
			return;

		for(UINT32 i = mWordsStart; i <= mWordsEnd; i++)
		{
			TextWord& word = TextUtility::WordBuffer[i];

			mWidth += word.getWidth();
			mHeight = std::max(mHeight, word.getHeight());
		}
	}

	TextUtility::TextData::TextData(const HFont& font, INT32 baselineOffset, UINT32 lineHeight, UINT32 spaceWidth)
		:mFont(font), mBaselineOffset(baselineOffset), mLineHeight(lineHeight), mSpaceWidth(spaceWidth), mChars(nullptr),
		mNumChars(0), mWords(nullptr), mNumWords(0), mLines(nullptr), mNumLines(0), mPageInfos(nullptr), mNumPageInfos(0), mData(nullptr)
	{
	}

	TextUtility::TextData::~TextData()
	{
		if(mData != nullptr)
			cm_free(mData);
	}

	Vector<TextUtility::TextWord>::type TextUtility::WordBuffer = Vector<TextUtility::TextWord>::type(2000);
	UINT32 TextUtility::NextFreeWord = 0;

	Vector<TextUtility::TextLine>::type TextUtility::LineBuffer = Vector<TextUtility::TextLine>::type(500);
	UINT32 TextUtility::NextFreeLine = 0;

	Vector<TextUtility::PageInfo>::type TextUtility::PageBuffer = Vector<TextUtility::PageInfo>::type(20);
	UINT32 TextUtility::NextFreePageInfo = 0;

	UINT32 TextUtility::allocWord(bool spacer)
	{
		if(NextFreeWord >= WordBuffer.size())
			WordBuffer.resize(WordBuffer.size() * 2);

		WordBuffer[NextFreeWord].init(spacer);

		return NextFreeWord++;
	}

	UINT32 TextUtility::allocLine(TextData* textData)
	{
		if(NextFreeLine >= LineBuffer.size())
			LineBuffer.resize(LineBuffer.size() * 2);

		LineBuffer[NextFreeLine].init(textData);

		return NextFreeLine++;
	}

	void TextUtility::deallocAll()
	{
		NextFreeWord = 0;
		NextFreeLine = 0;
		NextFreePageInfo = 0;
	}

	std::shared_ptr<TextUtility::TextData> TextUtility::getTextData(const WString& text, const HFont& font, UINT32 fontSize, UINT32 width, UINT32 height, bool wordWrap)
	{
		// In order to reduce number of memory allocations algorithm first calculates data into temporary buffers and then copies the results
		
		const FontData* fontData = nullptr;
		if(font != nullptr)
		{
			UINT32 nearestSize = font->getClosestAvailableSize(fontSize);
			fontData = font->getFontDataForSize(nearestSize);
		}

		if(fontData == nullptr || fontData->texturePages.size() == 0)
			return nullptr;

		if(fontData->size != fontSize)
		{
			LOGWRN("Unable to find font with specified size (" + toString(fontSize) + "). Using nearest available size: " + toString(fontData->size));
		}

		bool widthIsLimited = width > 0;

		std::shared_ptr<TextUtility::TextData> textData = cm_shared_ptr<TextData, PoolAlloc>(font, fontData->fontDesc.baselineOffset, fontData->fontDesc.lineHeight, fontData->fontDesc.spaceWidth);

		UINT32 curLineIdx = allocLine(textData.get());
		UINT32 curHeight = fontData->fontDesc.lineHeight;
		UINT32 charIdx = 0;

		while(true)
		{
			if(charIdx >= text.size())
				break;

			UINT32 charId = text[charIdx];
			const CHAR_DESC& charDesc = fontData->getCharDesc(charId);

			TextLine* curLine = &LineBuffer[curLineIdx];

			if(text[charIdx] == '\n')
			{
				curLine->finalize(true);

				curLineIdx = allocLine(textData.get());
				curLine = &LineBuffer[curLineIdx];

				curHeight += fontData->fontDesc.lineHeight;

				charIdx++;
				continue;
			}

			if(charId != SPACE_CHAR)
			{
				curLine->add(charIdx, charDesc);
				addCharToPage(charDesc.page, *fontData);
			}
			else
			{
				curLine->addSpace();
				addCharToPage(0, *fontData);
			}

			if(widthIsLimited && curLine->getWidth() > width)
			{
				if(wordWrap)
				{
					assert(!curLine->isEmpty());

					UINT32 lastWordIdx = curLine->removeLastWord();
					TextWord& lastWord = WordBuffer[lastWordIdx];

					if(lastWord.getWidth() <= width) // If the word fits, attempt to add it to a new line
					{
						curLine->finalize(false);

						curLineIdx = allocLine(textData.get());
						curLine = &LineBuffer[curLineIdx];

						curHeight += fontData->fontDesc.lineHeight;
					}

					curLine->addWord(lastWordIdx, lastWord);
				}
			}

			charIdx++;
		}

		LineBuffer[curLineIdx].finalize(true);

		// Now that we have all the data we need, allocate the permanent buffers and copy the data
		textData->mNumChars = (UINT32)text.size();
		textData->mNumWords = NextFreeWord;
		textData->mNumLines = NextFreeLine;
		textData->mNumPageInfos = NextFreePageInfo;

		UINT32 charArraySize = textData->mNumChars * sizeof(const CHAR_DESC*);
		UINT32 wordArraySize = textData->mNumWords * sizeof(TextWord);
		UINT32 lineArraySize = textData->mNumLines * sizeof(TextLine);
		UINT32 pageInfoArraySize = textData->mNumPageInfos * sizeof(PageInfo);

		UINT32 totalBufferSize = charArraySize + wordArraySize + lineArraySize + pageInfoArraySize;
		textData->mData = cm_alloc(totalBufferSize);

		UINT8* dataPtr = (UINT8*)textData->mData;
		textData->mChars = (const CHAR_DESC**)dataPtr;

		for(UINT32 i = 0; i < textData->mNumChars; i++)
		{
			UINT32 charId = text[i];
			const CHAR_DESC& charDesc = fontData->getCharDesc(charId);

			textData->mChars[i] = &charDesc;
		}

		dataPtr += charArraySize;
		textData->mWords = (TextWord*)dataPtr;
		memcpy(textData->mWords, &WordBuffer[0], wordArraySize);

		dataPtr += wordArraySize;
		textData->mLines = (TextLine*)dataPtr;
		memcpy(textData->mLines, &LineBuffer[0], lineArraySize);

		dataPtr += lineArraySize;
		textData->mPageInfos = (PageInfo*)dataPtr;
		memcpy(textData->mPageInfos, &PageBuffer[0], pageInfoArraySize);

		TextUtility::deallocAll();

		return textData;
	}

	void TextUtility::addCharToPage(UINT32 page, const FontData& fontData)
	{
		while(page >= NextFreePageInfo)
		{
			PageBuffer[NextFreePageInfo].numQuads = 0;
			PageBuffer[NextFreePageInfo].texture = HTexture();

			NextFreePageInfo++;
		}

		PageBuffer[page].numQuads++;

		if(PageBuffer[page].texture == nullptr)
			PageBuffer[page].texture = fontData.texturePages[page];
	}

	UINT32 TextUtility::TextData::getWidth() const
	{
		UINT32 width = 0;

		for(UINT32 i = 0; i < mNumLines; i++)
			width = std::max(width, mLines[i].getWidth());

		return width;
	}

	UINT32 TextUtility::TextData::getHeight() const
	{
		UINT32 height = 0;

		for(UINT32 i = 0; i < mNumLines; i++)
			height += mLines[i].getHeight();

		return height;
	}
}
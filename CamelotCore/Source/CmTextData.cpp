#include "CmTextData.h"
#include "CmFont.h"
#include "CmVector2.h"
#include "CmDebug.h"

namespace BansheeEngine
{
	const int SPACE_CHAR = 32;

	void TextData::TextWord::init(bool spacer)
	{
		mWidth = mHeight = 0;
		mSpacer = spacer;
		mSpaceWidth = 0;
		mCharsStart = 0;
		mCharsEnd = 0;
		mLastChar = nullptr;
	}

	// Assumes charIdx is an index right after last char in the list (if any). All chars need to be sequential.
	UINT32 TextData::TextWord::addChar(UINT32 charIdx, const CHAR_DESC& desc)
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

	void TextData::TextWord::addSpace(UINT32 spaceWidth)
	{
		mSpaceWidth += spaceWidth;
		mWidth = mSpaceWidth;
		mHeight = 0;
	}

	void TextData::TextLine::init(TextData* textData)
	{
		mWidth = 0;
		mHeight = 0;
		mIsEmpty = true; 
		mTextData = textData;
		mWordsStart = mWordsEnd = 0;
	}

	void TextData::TextLine::finalize(bool hasNewlineChar)
	{
		mHasNewline = hasNewlineChar;
	}

	void TextData::TextLine::add(UINT32 charIdx, const CHAR_DESC& charDesc)
	{
		UINT32 charWidth = 0;
		if(mIsEmpty)
		{
			mWordsStart = mWordsEnd = allocWord(false);
			mIsEmpty = false;
		}
		else
		{
			if(TextData::WordBuffer[mWordsEnd].isSpacer())
				mWordsEnd = allocWord(false);
		}

		TextWord& lastWord = TextData::WordBuffer[mWordsEnd];
		charWidth = lastWord.addChar(charIdx, charDesc);

		mWidth += charWidth;
		mHeight = std::max(mHeight, lastWord.getHeight());
	}

	void TextData::TextLine::addSpace()
	{
		if(mIsEmpty)
		{
			mWordsStart = mWordsEnd = allocWord(true);
			mIsEmpty = false;
		}
		else
			mWordsEnd = allocWord(true); // Each space is counted as its own word, to make certain operations easier

		TextWord& lastWord = TextData::WordBuffer[mWordsEnd];
		lastWord.addSpace(mTextData->getSpaceWidth());

		mWidth += mTextData->getSpaceWidth();
	}

	// Assumes wordIdx is an index right after last word in the list (if any). All words need to be sequential.
	void TextData::TextLine::addWord(UINT32 wordIdx, const TextWord& word)
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

	UINT32 TextData::TextLine::removeLastWord()
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

	UINT32 TextData::TextLine::fillBuffer(UINT32 page, Vector2* vertices, Vector2* uvs, UINT32* indexes, UINT32 offset, UINT32 size) const
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

	UINT32 TextData::TextLine::getNumChars() const
	{
		if(mIsEmpty)
			return 0;

		UINT32 numChars = 0;
		for(UINT32 i = mWordsStart; i <= mWordsEnd; i++)
		{
			TextWord& word = TextData::WordBuffer[i];

			if(word.isSpacer())
				numChars++;
			else
				numChars += (UINT32)word.getNumChars();
		}

		return numChars;
	}

	void TextData::TextLine::calculateBounds()
	{
		mWidth = 0;
		mHeight = 0;

		if(mIsEmpty)
			return;

		for(UINT32 i = mWordsStart; i <= mWordsEnd; i++)
		{
			TextWord& word = TextData::WordBuffer[i];

			mWidth += word.getWidth();
			mHeight = std::max(mHeight, word.getHeight());
		}
	}

	TextData::TextData(const WString& text, const HFont& font, UINT32 fontSize, UINT32 width, UINT32 height, bool wordWrap)
		:mFont(font), mChars(nullptr), mFontData(nullptr),
		mNumChars(0), mWords(nullptr), mNumWords(0), mLines(nullptr), mNumLines(0), mPageInfos(nullptr), mNumPageInfos(0), mData(nullptr)
	{
		// In order to reduce number of memory allocations algorithm first calculates data into temporary buffers and then copies the results
		initAlloc();

		if(font != nullptr)
		{
			UINT32 nearestSize = font->getClosestAvailableSize(fontSize);
			mFontData = font->getFontDataForSize(nearestSize);
		}

		if(mFontData == nullptr || mFontData->texturePages.size() == 0)
			return;

		if(mFontData->size != fontSize)
		{
			LOGWRN("Unable to find font with specified size (" + toString(fontSize) + "). Using nearest available size: " + toString(mFontData->size));
		}

		bool widthIsLimited = width > 0;
		mFont = font;

		UINT32 curLineIdx = allocLine(this);
		UINT32 curHeight = mFontData->fontDesc.lineHeight;
		UINT32 charIdx = 0;

		while(true)
		{
			if(charIdx >= text.size())
				break;

			UINT32 charId = text[charIdx];
			const CHAR_DESC& charDesc = mFontData->getCharDesc(charId);

			TextLine* curLine = &LineBuffer[curLineIdx];

			if(text[charIdx] == '\n')
			{
				curLine->finalize(true);

				curLineIdx = allocLine(this);
				curLine = &LineBuffer[curLineIdx];

				curHeight += mFontData->fontDesc.lineHeight;

				charIdx++;
				continue;
			}

			if(charId != SPACE_CHAR)
			{
				curLine->add(charIdx, charDesc);
				addCharToPage(charDesc.page, *mFontData);
			}
			else
			{
				curLine->addSpace();
				addCharToPage(0, *mFontData);
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

						curLineIdx = allocLine(this);
						curLine = &LineBuffer[curLineIdx];

						curHeight += mFontData->fontDesc.lineHeight;
					}

					curLine->addWord(lastWordIdx, lastWord);
				}
			}

			charIdx++;
		}

		LineBuffer[curLineIdx].finalize(true);

		// Now that we have all the data we need, allocate the permanent buffers and copy the data
		mNumChars = (UINT32)text.size();
		mNumWords = NextFreeWord;
		mNumLines = NextFreeLine;
		mNumPageInfos = NextFreePageInfo;

		UINT32 charArraySize = mNumChars * sizeof(const CHAR_DESC*);
		UINT32 wordArraySize = mNumWords * sizeof(TextWord);
		UINT32 lineArraySize = mNumLines * sizeof(TextLine);
		UINT32 pageInfoArraySize = mNumPageInfos * sizeof(PageInfo);

		UINT32 totalBufferSize = charArraySize + wordArraySize + lineArraySize + pageInfoArraySize;
		mData = cm_alloc(totalBufferSize);

		UINT8* dataPtr = (UINT8*)mData;
		mChars = (const CHAR_DESC**)dataPtr;

		for(UINT32 i = 0; i < mNumChars; i++)
		{
			UINT32 charId = text[i];
			const CHAR_DESC& charDesc = mFontData->getCharDesc(charId);

			mChars[i] = &charDesc;
		}

		dataPtr += charArraySize;
		mWords = (TextWord*)dataPtr;
		memcpy(mWords, &WordBuffer[0], wordArraySize);

		dataPtr += wordArraySize;
		mLines = (TextLine*)dataPtr;
		memcpy(mLines, &LineBuffer[0], lineArraySize);

		dataPtr += lineArraySize;
		mPageInfos = (PageInfo*)dataPtr;
		memcpy(mPageInfos, &PageBuffer[0], pageInfoArraySize);

		TextData::deallocAll();
	}

	TextData::~TextData()
	{
		if(mData != nullptr)
			cm_free(mData);
	}

	const HTexture& TextData::getTextureForPage(UINT32 page) const 
	{ 
		return mFontData->texturePages[page]; 
	}

	INT32 TextData::getBaselineOffset() const 
	{ 
		return mFontData->fontDesc.baselineOffset; 
	}

	UINT32 TextData::getLineHeight() const 
	{ 
		return mFontData->fontDesc.lineHeight; 
	}

	UINT32 TextData::getSpaceWidth() const 
	{ 
		return mFontData->fontDesc.spaceWidth; 
	}

	bool TextData::BuffersInitialized = false;

	TextData::TextWord* TextData::WordBuffer = nullptr;
	UINT32 TextData::NextFreeWord = 0;
	UINT32 TextData::WordBufferSize = 0;

	TextData::TextLine* TextData::LineBuffer = nullptr;
	UINT32 TextData::NextFreeLine = 0;
	UINT32 TextData::LineBufferSize = 0;

	TextData::PageInfo* TextData::PageBuffer = nullptr;
	UINT32 TextData::NextFreePageInfo = 0;
	UINT32 TextData::PageBufferSize = 0;

	void TextData::initAlloc()
	{
		if(!BuffersInitialized)
		{
			WordBufferSize = 2000;
			LineBufferSize = 500;
			PageBufferSize = 20;

			WordBuffer = cm_newN<TextWord>(WordBufferSize);
			LineBuffer = cm_newN<TextLine>(LineBufferSize);
			PageBuffer = cm_newN<PageInfo>(PageBufferSize);

			BuffersInitialized = true;
		}
	}

	UINT32 TextData::allocWord(bool spacer)
	{
		if(NextFreeWord >= WordBufferSize)
		{
			UINT32 newBufferSize = WordBufferSize * 2;
			TextWord* newBuffer = cm_newN<TextWord>(newBufferSize);
			memcpy(WordBuffer, newBuffer, WordBufferSize);

			cm_deleteN(WordBuffer, WordBufferSize);
			WordBuffer = newBuffer;
			WordBufferSize = newBufferSize;
		}

		WordBuffer[NextFreeWord].init(spacer);

		return NextFreeWord++;
	}

	UINT32 TextData::allocLine(TextData* textData)
	{
		if(NextFreeLine >= LineBufferSize)
		{
			UINT32 newBufferSize = LineBufferSize * 2;
			TextLine* newBuffer = cm_newN<TextLine>(newBufferSize);
			memcpy(LineBuffer, newBuffer, LineBufferSize);

			cm_deleteN(LineBuffer, LineBufferSize);
			LineBuffer = newBuffer;
			LineBufferSize = newBufferSize;
		}

		LineBuffer[NextFreeLine].init(textData);

		return NextFreeLine++;
	}

	void TextData::deallocAll()
	{
		NextFreeWord = 0;
		NextFreeLine = 0;
		NextFreePageInfo = 0;
	}

	void TextData::addCharToPage(UINT32 page, const FontData& fontData)
	{
		if(NextFreePageInfo >= PageBufferSize)
		{
			UINT32 newBufferSize = PageBufferSize * 2;
			PageInfo* newBuffer = cm_newN<PageInfo>(newBufferSize);
			memcpy(PageBuffer, newBuffer, PageBufferSize);

			cm_deleteN(PageBuffer, PageBufferSize);
			PageBuffer = newBuffer;
			PageBufferSize = newBufferSize;
		}

		while(page >= NextFreePageInfo)
		{
			PageBuffer[NextFreePageInfo].numQuads = 0;

			NextFreePageInfo++;
		}

		PageBuffer[page].numQuads++;
	}

	UINT32 TextData::getWidth() const
	{
		UINT32 width = 0;

		for(UINT32 i = 0; i < mNumLines; i++)
			width = std::max(width, mLines[i].getWidth());

		return width;
	}

	UINT32 TextData::getHeight() const
	{
		UINT32 height = 0;

		for(UINT32 i = 0; i < mNumLines; i++)
			height += mLines[i].getHeight();

		return height;
	}
}
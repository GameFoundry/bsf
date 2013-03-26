#include "CmTextSprite.h"
#include "CmDebug.h"
#include "CmFontDesc.h"
#include "CmFont.h"
#include "CmVector2.h"

namespace CamelotEngine
{
	const int SPACE_CHAR = 32;

	class TextWord
	{
	public:
		TextWord(bool spacer)
			:mWidth(0), mSpacer(spacer)
		{ }

		void addChar(const CHAR_DESC& desc)
		{
			mChars.push_back(desc);

			calculateWidth();
		}

		void removeLastChar()
		{
			if(mChars.size() > 0)
			{
				mChars.erase(mChars.end() - 1);
				calculateWidth();
			}
		}

		UINT32 getWidth() const { return mWidth; }
		bool isSpacer() const { return mSpacer; }

		const vector<CHAR_DESC>::type& getChars() const { return mChars; }

	private:
		vector<CHAR_DESC>::type mChars;
		UINT32 mWidth;
		bool mSpacer;

		void calculateWidth()
		{
			if(mChars.size() == 0)
			{
				mWidth = 0;
				return;
			}

			mWidth = 0;
			UINT32 kerning = 0;
			for(size_t i = 0; i < mChars.size() - 1; i++)
			{
				mWidth += mChars[i].xAdvance + kerning;

				kerning = 0;
				for(size_t j = 0; j < mChars[i].kerningPairs.size(); j++)
				{
					if(mChars[i].kerningPairs[j].otherCharId == mChars[i + 1].charId)
					{
						kerning = mChars[i].kerningPairs[j].amount;
						break;
					}
				}
			}

			mWidth += mChars[mChars.size() - 1].xAdvance + kerning;
		}
	};

	class TextLine
	{
	public:
		TextLine()
			:mWidth(0), mLastWord(nullptr)
		{

		}

		~TextLine()
		{
			for(auto iter = mWords.begin(); iter != mWords.end(); ++iter)
				delete *iter;
		}

		void add(const CHAR_DESC& charDesc)
		{
			if(mLastWord == nullptr)
			{
				TextWord* newWord = new TextWord(charDesc.charId == SPACE_CHAR);
				mLastWord = newWord;

				mWords.push_back(mLastWord);

				mLastWord->addChar(charDesc);
			}
			else
			{
				if(charDesc.charId != SPACE_CHAR)
				{
					if(mLastWord->isSpacer())
					{
						TextWord* newWord = new TextWord(false);
						mLastWord = newWord;

						mWords.push_back(mLastWord);
					}

					mLastWord->addChar(charDesc);
				}
				else
				{
					TextWord* newWord = new TextWord(true); // Each space is counted as its own word, to make certain operations easier
					mLastWord = newWord;

					mWords.push_back(mLastWord);

					mLastWord->addChar(charDesc);
				}
			}

			calculateBounds();
		}

		void addWord(TextWord* word)
		{
			mWords.push_back(word);
			mLastWord = word;

			calculateBounds();
		}

		TextWord* removeLastWord()
		{
			if(mWords.size() == 0)
				return nullptr;

			TextWord* word = mWords[mWords.size() - 1];
			mWords.erase(mWords.end() - 1);

			if(mWords.size() > 0)
				mLastWord = mWords[mWords.size() - 1];
			else
				mLastWord = nullptr;

			calculateBounds();

			return word;
		}

		UINT32 getWidth() const { return mWidth; }
		Point getPosition() const { return mPosition; }
		void setPosition(const Point& pos) { mPosition = pos; }

		UINT32 fillBuffer(Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, const FontData& fontData)
		{
			UINT32 curVert = startingQuad * 4;
			UINT32 curIndex = startingQuad * 6;

			UINT32 maxVertIdx = maxNumQuads * 4;
			UINT32 maxIndexIdx = maxNumQuads * 6;

			UINT32 penX = mPosition.x;
			UINT32 baselineY = mPosition.y + fontData.fontDesc.baselineOffset;
			for(auto wordIter = mWords.begin(); wordIter != mWords.end(); ++wordIter)
			{
				if((*wordIter)->isSpacer())
				{
					penX += fontData.fontDesc.spaceWidth;
				}
				else
				{
					const vector<CHAR_DESC>::type& chars = (*wordIter)->getChars();
					UINT32 kerning = 0;
					for(auto charIter = chars.begin(); charIter != chars.end(); ++charIter)
					{
						assert(curVert < maxVertIdx);
						assert(curIndex < maxIndexIdx);

						INT32 curX = penX + charIter->xOffset;
						INT32 curY = -(baselineY - charIter->yOffset);

						vertices[curVert + 0] = Vector2((float)curX, (float)curY);
						vertices[curVert + 1] = Vector2((float)(curX + charIter->width), (float)curY);
						vertices[curVert + 2] = Vector2((float)curX, (float)curY - (float)charIter->height);
						vertices[curVert + 3] = Vector2((float)(curX + charIter->width), (float)curY - (float)charIter->height);

						uv[curVert + 0] = Vector2(charIter->uvX, charIter->uvY);
						uv[curVert + 1] = Vector2(charIter->uvX + charIter->uvWidth, charIter->uvY);
						uv[curVert + 2] = Vector2(charIter->uvX, charIter->uvY + charIter->uvHeight);
						uv[curVert + 3] = Vector2(charIter->uvX + charIter->uvWidth, charIter->uvY + charIter->uvHeight);

						indices[curIndex + 0] = curVert + 0;
						indices[curIndex + 1] = curVert + 1;
						indices[curIndex + 2] = curVert + 2;
						indices[curIndex + 3] = curVert + 1;
						indices[curIndex + 4] = curVert + 3;
						indices[curIndex + 5] = curVert + 2;

						penX += charIter->xAdvance + kerning;
						curVert += 4;
						curIndex += 6;
						
						kerning = 0;
						if((charIter + 1) != chars.end())
						{
							for(size_t j = 0; j < charIter->kerningPairs.size(); j++)
							{
								if(charIter->kerningPairs[j].otherCharId == (charIter + 1)->charId)
								{
									kerning = charIter->kerningPairs[j].amount;
									break;
								}
							}
						}
					}
				}
			}

			return (curVert / 4) - startingQuad;
		}

	private:
		UINT32 mWidth;
		vector<TextWord*>::type mWords;
		TextWord* mLastWord;
		Point mPosition;

		void calculateBounds()
		{
			mWidth = 0;
			for(auto iter = mWords.begin(); iter != mWords.end(); ++iter)
			{
				mWidth += (*iter)->getWidth();
			}
		}
	};

	TextSprite::TextSprite()
		:mFontSize(0), mWordWrap(false), mHorzAlign(THA_Left), mVertAlign(TVA_Top)
	{

	}

	TextSprite::TextSprite(const String& text, FontPtr font, UINT32 fontSize)
		:mText(text), mFont(font), mFontSize(fontSize), mWordWrap(false), mHorzAlign(THA_Left), mVertAlign(TVA_Top)
	{

	}

	void TextSprite::updateMesh()
	{
		const FontData* fontData = getFontData();

		clearMesh();

		if(fontData == nullptr)
			return;

		if(fontData->size != mFontSize)
		{
			LOGWRN("Unable to find font with specified size (" + toString(mFontSize) + "). Using nearest available size: " + toString(fontData->size));
		}

		bool heightIsLimited = mHeight > 0;
		bool widthIsLimited = mWidth > 0;

		TextLine* curLine = new TextLine();
		vector<TextLine*>::type textLines;
		textLines.push_back(curLine);

		UINT32 curHeight = fontData->fontDesc.lineHeight;
		UINT32 charIdx = 0;
		mNumMeshQuads = 0;
		while(true)
		{
			if(charIdx >= mText.size())
				break;

			if(mText[charIdx] == '\n')
			{
				if(heightIsLimited && (curHeight + fontData->fontDesc.lineHeight * 2) > mHeight)
					break; // Max height reached

				curLine = new TextLine();
				textLines.push_back(curLine);
				curHeight += fontData->fontDesc.lineHeight;

				charIdx++;
				continue;
			}

			UINT32 charId = mText[charIdx];
			const CHAR_DESC& charDesc = fontData->getCharDesc(charId);

			curLine->add(charDesc);

			if(charDesc.charId != SPACE_CHAR)
				mNumMeshQuads++;

			if(widthIsLimited && curLine->getWidth() > mWidth)
			{
				if(mWordWrap)
				{
					TextWord* lastWord = curLine->removeLastWord();
					if(lastWord->isSpacer())
						curLine->addWord(lastWord); // Spaces can stay on previous line even if they don't technically fit

					// No more lines fit vertically so we're done
					if(heightIsLimited && (curHeight + fontData->fontDesc.lineHeight * 2) > mHeight)
						break;

					curLine = new TextLine();
					textLines.push_back(curLine);
					curHeight += fontData->fontDesc.lineHeight;

					if(!lastWord->isSpacer())
						curLine->addWord(lastWord);
				}
				else
				{
					// Nothing else we can do, chars don't fit so we are done
					break;
				}
			}

			charIdx++;
		}

		// Calc vertical alignment offset
		UINT32 vertDiff = std::max(0U, mHeight - curHeight);
		UINT32 vertOffset = 0;
		switch(mVertAlign)
		{
		case TVA_Top:
			vertOffset = 0;
			break;
		case TVA_Bottom:
			vertOffset = std::max(0, (INT32)(mHeight - curHeight));
			break;
		case TVA_Center:
			vertOffset = std::max(0, (INT32)(mHeight - curHeight)) / 2;
			break;
		}

		// Calc horizontal alignment offset and set final line positions
		Point offset = mOffset + getAnchorOffset();
		UINT32 curY = 0;
		for(size_t i = 0; i < textLines.size(); i++)
		{
			UINT32 horzOffset = 0;
			switch(mHorzAlign)
			{
			case THA_Left:
				horzOffset = 0;
				break;
			case THA_Right:
				horzOffset = std::max(0, (INT32)(mWidth - textLines[i]->getWidth()));
				break;
			case THA_Center:
				horzOffset = std::max(0, (INT32)(mWidth - textLines[i]->getWidth())) / 2;
				break;
			}

			textLines[i]->setPosition(offset + Point(horzOffset, vertOffset + curY));

			curY += fontData->fontDesc.lineHeight;
		}

		// Actually generate a mesh
		mVertices = new Vector2[mNumMeshQuads * 4];
		mUVs = new Vector2[mNumMeshQuads * 4];
		mIndexes = new UINT32[mNumMeshQuads * 6];

		UINT32 curFace = 0;
		for(size_t i = 0; i < textLines.size(); i++)
		{
			UINT32 newFaces = textLines[i]->fillBuffer(mVertices, mUVs, mIndexes, curFace, mNumMeshQuads, *fontData);

			curFace += newFaces;
		}

		for(size_t i = 0; i < textLines.size(); i++)
			delete textLines[i];

		// TODO - Clip the mesh based on mWidth/mHeight
		
		// TODO - How do I implement a scrollable text area without a clip rect?
	}

	const FontData* TextSprite::getFontData() const
	{
		if(mFont == nullptr)
			return nullptr;

		UINT32 nearestSize = mFont->getClosestAvailableSize(mFontSize);

		return mFont->getFontDataForSize(nearestSize);
	}
}
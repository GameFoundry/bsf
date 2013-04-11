#include "CmTextSprite.h"
#include "CmDebug.h"
#include "CmFontDesc.h"
#include "CmFont.h"
#include "CmVector2.h"
#include "CmMath.h"
#include "CmGUIMaterialManager.h"

namespace CamelotEngine
{
	const int SPACE_CHAR = 32;

	class TextWord
	{
	public:
		TextWord(bool spacer)
			:mWidth(0), mSpacer(spacer), mSpaceWidth(0)
		{ }

		void addChar(const CHAR_DESC& desc)
		{
			mChars.push_back(desc);

			calculateWidth();
		}

		void addSpace(UINT32 spaceWidth)
		{
			mSpaceWidth += spaceWidth;
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
		UINT32 mSpaceWidth;

		void calculateWidth()
		{
			if(isSpacer())
			{
				mWidth = mSpaceWidth;
				return;
			}

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
				CM_DELETE(*iter, TextWord, ScratchAlloc);
		}

		void add(const CHAR_DESC& charDesc)
		{
			if(mLastWord == nullptr)
			{
				TextWord* newWord = CM_NEW(TextWord, ScratchAlloc) TextWord(false);
				mLastWord = newWord;

				mWords.push_back(mLastWord);

				mLastWord->addChar(charDesc);
			}
			else
			{
				if(mLastWord->isSpacer())
				{
					TextWord* newWord = CM_NEW(TextWord, ScratchAlloc) TextWord(false);
					mLastWord = newWord;

					mWords.push_back(mLastWord);
				}

				mLastWord->addChar(charDesc);
			}

			calculateBounds();
		}

		void addSpace(UINT32 spaceWidth)
		{
			if(mLastWord == nullptr)
			{
				TextWord* newWord = CM_NEW(TextWord, ScratchAlloc) TextWord(true);
				mLastWord = newWord;

				mWords.push_back(mLastWord);

				mLastWord->addSpace(spaceWidth);
			}
			else
			{
				TextWord* newWord = CM_NEW(TextWord, ScratchAlloc) TextWord(true); // Each space is counted as its own word, to make certain operations easier
				mLastWord = newWord;

				mWords.push_back(mLastWord);

				mLastWord->addSpace(spaceWidth);
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

		void fillBuffer(const vector<SpriteRenderElement>::type& renderElements, vector<UINT32>::type& faceOffsets, const FontData& fontData)
		{
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
						INT32 curX = penX + charIter->xOffset;
						INT32 curY = -((INT32)baselineY - charIter->yOffset);

						UINT32 curVert = faceOffsets[charIter->page] * 4;
						UINT32 curIndex = faceOffsets[charIter->page] * 6;

						const SpriteRenderElement& renderElem = renderElements[charIter->page];

						renderElem.vertices[curVert + 0] = Vector2((float)curX, (float)curY);
						renderElem.vertices[curVert + 1] = Vector2((float)(curX + charIter->width), (float)curY);
						renderElem.vertices[curVert + 2] = Vector2((float)curX, (float)curY - (float)charIter->height);
						renderElem.vertices[curVert + 3] = Vector2((float)(curX + charIter->width), (float)curY - (float)charIter->height);

						renderElem.uvs[curVert + 0] = Vector2(charIter->uvX, charIter->uvY);
						renderElem.uvs[curVert + 1] = Vector2(charIter->uvX + charIter->uvWidth, charIter->uvY);
						renderElem.uvs[curVert + 2] = Vector2(charIter->uvX, charIter->uvY + charIter->uvHeight);
						renderElem.uvs[curVert + 3] = Vector2(charIter->uvX + charIter->uvWidth, charIter->uvY + charIter->uvHeight);

						renderElem.indexes[curIndex + 0] = curVert + 0;
						renderElem.indexes[curIndex + 1] = curVert + 1;
						renderElem.indexes[curIndex + 2] = curVert + 2;
						renderElem.indexes[curIndex + 3] = curVert + 1;
						renderElem.indexes[curIndex + 4] = curVert + 3;
						renderElem.indexes[curIndex + 5] = curVert + 2;

						penX += charIter->xAdvance + kerning;
						faceOffsets[charIter->page]++;

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

	TextSprite::TextSprite(const String& text, HFont font, UINT32 fontSize)
		:mText(text), mFont(font), mFontSize(fontSize), mWordWrap(false), mHorzAlign(THA_Left), mVertAlign(TVA_Top)
	{

	}

	void TextSprite::updateMesh() const
	{
		const FontData* fontData = getFontData();

		if(fontData == nullptr)
		{
			clearMesh();
			return;
		}

		if(fontData->size != mFontSize)
		{
			LOGWRN("Unable to find font with specified size (" + toString(mFontSize) + "). Using nearest available size: " + toString(fontData->size));
		}

		bool heightIsLimited = mHeight > 0;
		bool widthIsLimited = mWidth > 0;

		TextLine* curLine = CM_NEW(TextLine, ScratchAlloc) TextLine();
		vector<TextLine*>::type textLines;
		textLines.push_back(curLine);

		UINT32 curHeight = fontData->fontDesc.lineHeight;
		UINT32 charIdx = 0;

		vector<UINT32>::type newRenderElemSizes;
		while(true)
		{
			if(charIdx >= mText.size())
				break;

			if(mText[charIdx] == '\n')
			{
				if(heightIsLimited && (curHeight + fontData->fontDesc.lineHeight * 2) > mHeight)
					break; // Max height reached

				curLine = CM_NEW(TextLine, ScratchAlloc) TextLine();
				textLines.push_back(curLine);
				curHeight += fontData->fontDesc.lineHeight;

				charIdx++;
				continue;
			}

			UINT32 charId = mText[charIdx];
			const CHAR_DESC& charDesc = fontData->getCharDesc(charId);

			if(charId != SPACE_CHAR)
			{
				curLine->add(charDesc);

				if(charDesc.page >= (UINT32)newRenderElemSizes.size())
					newRenderElemSizes.resize(charDesc.page + 1);

				newRenderElemSizes[charDesc.page]++;
			}
			else
				curLine->addSpace(fontData->fontDesc.spaceWidth);

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

					curLine = CM_NEW(TextLine, ScratchAlloc) TextLine();
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
		if(mCachedRenderElements.size() < newRenderElemSizes.size())
			mCachedRenderElements.resize(newRenderElemSizes.size());

		UINT32 texPage = 0;
		for(auto& cachedElem : mCachedRenderElements)
		{
			UINT32 newNumQuads = newRenderElemSizes[texPage];
			if(newNumQuads != cachedElem.numQuads)
			{
				UINT32 oldVertexCount = cachedElem.numQuads * 4;
				UINT32 oldIndexCount = cachedElem.numQuads * 6;

				if(cachedElem.vertices != nullptr) CM_DELETE_ARRAY(cachedElem.vertices, Vector2, oldVertexCount, ScratchAlloc);
				if(cachedElem.uvs != nullptr) CM_DELETE_ARRAY(cachedElem.uvs, Vector2, oldVertexCount, ScratchAlloc);
				if(cachedElem.indexes != nullptr) CM_DELETE_ARRAY(cachedElem.indexes, UINT32, oldIndexCount, ScratchAlloc);

				cachedElem.vertices = CM_NEW_ARRAY(Vector2, newNumQuads * 4, ScratchAlloc);
				cachedElem.uvs = CM_NEW_ARRAY(Vector2, newNumQuads * 4, ScratchAlloc);
				cachedElem.indexes = CM_NEW_ARRAY(UINT32, newNumQuads * 6, ScratchAlloc);
				cachedElem.numQuads = newNumQuads;
			}

			HMaterial newMaterial = GUIMaterialManager::instance().requestTextMaterial(fontData->texturePages[texPage]);
			if(cachedElem.material != nullptr)
				GUIMaterialManager::instance().releaseMaterial(newMaterial);

			cachedElem.material = newMaterial;

			texPage++;
		}

		vector<UINT32>::type faceOffsets(mCachedRenderElements.size(), 0);
		for(size_t i = 0; i < textLines.size(); i++)
			textLines[i]->fillBuffer(mCachedRenderElements, faceOffsets, *fontData);

		for(size_t i = 0; i < textLines.size(); i++)
			CM_DELETE(textLines[i], TextLine, ScratchAlloc);

		if(isClipRectangleValid())
		{
			for(auto& renderElem : mCachedRenderElements)
			{
				clipToRect(renderElem.vertices, renderElem.uvs, renderElem.numQuads, mClipRect);
			}
		}
	}

	const FontData* TextSprite::getFontData() const
	{
		if(mFont == nullptr)
			return nullptr;

		UINT32 nearestSize = mFont->getClosestAvailableSize(mFontSize);

		return mFont->getFontDataForSize(nearestSize);
	}

	// This will only properly clip an array of rectangular quads
	// Vertices in the quad must be in a specific order: top left, top right, bottom left, bottom right
	// (0, 0) represents top left of the screen
	void TextSprite::clipToRect(Vector2* vertices, Vector2* uv, UINT32 numQuads, Rect clipRect) const
	{
		float left = (float)clipRect.x;
		float right = (float)clipRect.x + clipRect.width;
		float top = (float)clipRect.y;
		float bottom = (float)clipRect.y - clipRect.height;

		for(UINT32 i = 0; i < numQuads; i++)
		{
			UINT32 vertIdx = i * 4;

			// Attempt to skip those that are definitely not clipped
			if(vertices[vertIdx + 0].x >= left && vertices[vertIdx + 1].x <= right &&
				vertices[vertIdx + 0].y <= top && vertices[vertIdx + 2].y >= bottom)
			{
				continue;
			}

			float du = (uv[vertIdx + 1].x - uv[vertIdx + 0].x) / (vertices[vertIdx + 1].x - vertices[vertIdx + 0].x);
			float dv = (uv[vertIdx + 0].y - uv[vertIdx + 2].y) / (vertices[vertIdx + 0].y - vertices[vertIdx + 2].y);

			// Clip left
			float newLeft = Math::Clamp(vertices[vertIdx + 0].x, left, right);
			float uvLeftOffset = (newLeft - vertices[vertIdx + 0].x) * du;

			vertices[vertIdx + 0].x = newLeft;
			vertices[vertIdx + 2].x = newLeft;
			uv[vertIdx + 0].x += uvLeftOffset;
			uv[vertIdx + 2].x += uvLeftOffset;

			// Clip right
			float newRight = Math::Clamp(vertices[vertIdx + 1].x, left, right);
			float uvRightOffset = (vertices[vertIdx + 1].x - newRight) * du;

			vertices[vertIdx + 1].x = newRight;
			vertices[vertIdx + 3].x = newRight;
			uv[vertIdx + 1].x -= uvRightOffset;
			uv[vertIdx + 3].x -= uvRightOffset;

			// Clip top
			float newTop = Math::Clamp(vertices[vertIdx + 0].y, bottom, top);
			float uvTopOffset = (vertices[vertIdx + 0].y - newTop) * dv;

			vertices[vertIdx + 0].y = newTop;
			vertices[vertIdx + 1].y = newTop;
			uv[vertIdx + 0].y -= uvTopOffset;
			uv[vertIdx + 1].y -= uvTopOffset;

			// Clip bottom
			float newBottom = Math::Clamp(vertices[vertIdx + 2].y, bottom, top);
			float uvBottomOffset = (newBottom - vertices[vertIdx + 2].y) * dv;

			vertices[vertIdx + 2].y = newBottom;
			vertices[vertIdx + 3].y = newBottom;
			uv[vertIdx + 2].y += uvBottomOffset;
			uv[vertIdx + 3].y += uvBottomOffset;
		}
	}
}
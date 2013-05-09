#include "BsTextSprite.h"
#include "BsGUIMaterialManager.h"
#include "CmDebug.h"
#include "CmFontDesc.h"
#include "CmFont.h"
#include "CmVector2.h"
#include "CmMath.h"

using namespace CamelotFramework;

namespace BansheeEngine
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
						INT32 curY = ((INT32)baselineY - charIter->yOffset);

						UINT32 curVert = faceOffsets[charIter->page] * 4;
						UINT32 curIndex = faceOffsets[charIter->page] * 6;

						const SpriteRenderElement& renderElem = renderElements[charIter->page];

						renderElem.vertices[curVert + 0] = Vector2((float)curX, (float)curY);
						renderElem.vertices[curVert + 1] = Vector2((float)(curX + charIter->width), (float)curY);
						renderElem.vertices[curVert + 2] = Vector2((float)curX, (float)curY + (float)charIter->height);
						renderElem.vertices[curVert + 3] = Vector2((float)(curX + charIter->width), (float)curY + (float)charIter->height);

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
	{

	}

	void TextSprite::update(const TEXT_SPRITE_DESC& desc)
	{
		const FontData* fontData = nullptr;
		if(desc.font != nullptr)
		{
			UINT32 nearestSize = desc.font->getClosestAvailableSize(desc.fontSize);
			fontData = desc.font->getFontDataForSize(nearestSize);
		}

		if(fontData == nullptr)
		{
			clearMesh();
			return;
		}

		if(fontData->size != desc.fontSize)
		{
			LOGWRN("Unable to find font with specified size (" + toString(desc.fontSize) + "). Using nearest available size: " + toString(fontData->size));
		}

		bool heightIsLimited = desc.height > 0;
		bool widthIsLimited = desc.width > 0;

		TextLine* curLine = CM_NEW(TextLine, ScratchAlloc) TextLine();
		vector<TextLine*>::type textLines;
		textLines.push_back(curLine);

		UINT32 curHeight = fontData->fontDesc.lineHeight;
		UINT32 charIdx = 0;

		vector<UINT32>::type newRenderElemSizes;
		while(true)
		{
			if(charIdx >= desc.text.size())
				break;

			if(desc.text[charIdx] == '\n')
			{
				if(heightIsLimited && (curHeight + fontData->fontDesc.lineHeight * 2) > desc.height)
					break; // Max height reached

				curLine = CM_NEW(TextLine, ScratchAlloc) TextLine();
				textLines.push_back(curLine);
				curHeight += fontData->fontDesc.lineHeight;

				charIdx++;
				continue;
			}

			UINT32 charId = desc.text[charIdx];
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

			if(widthIsLimited && curLine->getWidth() > desc.width)
			{
				if(desc.wordWrap)
				{
					TextWord* lastWord = curLine->removeLastWord();
					if(lastWord->isSpacer())
						curLine->addWord(lastWord); // Spaces can stay on previous line even if they don't technically fit

					// No more lines fit vertically so we're done
					if(heightIsLimited && curHeight > desc.height)
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
		UINT32 vertDiff = std::max(0U, desc.height - curHeight);
		UINT32 vertOffset = 0;
		switch(desc.vertAlign)
		{
		case TVA_Top:
			vertOffset = 0;
			break;
		case TVA_Bottom:
			vertOffset = std::max(0, (INT32)(desc.height - curHeight));
			break;
		case TVA_Center:
			vertOffset = std::max(0, (INT32)(desc.height - curHeight)) / 2;
			break;
		}

		// Calc horizontal alignment offset and set final line positions
		Point offset = getAnchorOffset(desc.anchor, desc.width, desc.height);
		UINT32 curY = 0;
		for(size_t i = 0; i < textLines.size(); i++)
		{
			UINT32 horzOffset = 0;
			switch(desc.horzAlign)
			{
			case THA_Left:
				horzOffset = 0;
				break;
			case THA_Right:
				horzOffset = std::max(0, (INT32)(desc.width - textLines[i]->getWidth()));
				break;
			case THA_Center:
				horzOffset = std::max(0, (INT32)(desc.width - textLines[i]->getWidth())) / 2;
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

		if(desc.clipRect.width > 0 && desc.clipRect.height > 0)
		{
			for(auto& renderElem : mCachedRenderElements)
			{
				clipToRect(renderElem.vertices, renderElem.uvs, renderElem.numQuads, desc.clipRect);
			}
		}

		// Apply offset
		for(auto& renderElem : mCachedRenderElements)
		{
			UINT32 numVertices = renderElem.numQuads * 4;
			for(size_t i = 0; i < numVertices; i++)
			{
				renderElem.vertices[i].x += (float)desc.offset.x;
				renderElem.vertices[i].y += (float)desc.offset.y;
			}
		}

		updateBounds();
	}
}
#pragma once

#include "CmPrerequisites.h"
#include "CmFontDesc.h"
#include "CmInt2.h"

namespace CamelotFramework
{
	class CM_EXPORT TextUtility
	{
	private:
		class TextWord
		{
		public:
			void init(bool spacer);

			UINT32 addChar(UINT32 charIdx, const CHAR_DESC& desc);
			void addSpace(UINT32 spaceWidth);

			UINT32 getWidth() const { return mWidth; }
			UINT32 getHeight() const { return mHeight; }
			bool isSpacer() const { return mSpacer; }

			UINT32 getNumChars() const { return mLastChar == nullptr ? 0 : (mCharsEnd - mCharsStart + 1); }
			UINT32 getCharsStart() const { return mCharsStart; }
			UINT32 getCharsEnd() const { return mCharsEnd; }

		private:
			UINT32 mCharsStart, mCharsEnd;
			UINT32 mWidth;
			UINT32 mHeight;

			const CHAR_DESC* mLastChar;

			bool mSpacer;
			UINT32 mSpaceWidth;
		};
		
		struct PageInfo
		{
			UINT32 numQuads;
			HTexture texture;
		};
	public:
		class TextData;

		class CM_EXPORT TextLine
		{
		public:
			UINT32 getWidth() const { return mWidth; }
			UINT32 getHeight() const { return mHeight; }

			/**
			 * @brief	Returns an offset used to separate two lines.
			 */
			UINT32 getYOffset() const { return mTextData->getLineHeight(); }

			/**
			 * @brief	Fills the vertex/uv/index buffers for the specified page, with all the character data
			 * 			needed for rendering.
			 *
			 * @param	page			The page.
			 * @param [out]	vertices	Pre-allocated array where character vertices will be written.
			 * @param [out]	uvs			Pre-allocated array where character uv coordinates will be written.
			 * @param [out]	indexes 	Pre-allocated array where character indices will be written.
			 * @param	offset			Offsets the location at which the method writes to the buffers.
			 * 							Counted as number of quads.
			 * @param	size			Total number of quads that can fit into the specified buffers.
			 *
			 * @return	Number of quads that were written.
			 */
			UINT32 fillBuffer(UINT32 page, Vector2* vertices, Vector2* uvs, UINT32* indexes, UINT32 offset, UINT32 size) const;

			/**
			 * @brief	Returns the total number of characters on this line.
			 */
			UINT32 getNumChars() const;

			/**
			 * @brief	Query if this line was created explicitly due to a newline character.
			 * 			As opposed to a line that was created because a word couldn't fit on the previous line.
			 */
			bool hasNewlineChar() const { return mHasNewline; }
		private:
			friend class TextUtility;

			TextData* mTextData;
			UINT32 mWordsStart, mWordsEnd;

			UINT32 mWidth;
			UINT32 mHeight;

			bool mIsEmpty;
			bool mHasNewline;

			void add(UINT32 charIdx, const CHAR_DESC& charDesc);
			void addSpace();
			void addWord(UINT32 wordIdx, const TextWord& word);

			void init(TextData* textData);
			void finalize(bool hasNewlineChar);

			bool isEmpty() const { return mIsEmpty; }
			UINT32 removeLastWord();

			void calculateBounds();
		};

		class CM_EXPORT TextData
		{
		public:
			TextData(const HFont& font, INT32 baselineOffset, UINT32 lineHeight, UINT32 spaceWidth);
			~TextData();

			UINT32 getNumLines() const { return mNumLines; }
			UINT32 getNumPages() const { return mNumPageInfos; }

			const TextLine& getLine(UINT32 idx) const { return mLines[idx]; }
			const HTexture& getTextureForPage(UINT32 page) const { return mPageInfos[page].texture; }
			UINT32 getNumQuadsForPage(UINT32 page) const { return mPageInfos[page].numQuads; }

			UINT32 getWidth() const;
			UINT32 getHeight() const;

		private:
			friend class TextUtility;
			friend class TextLine;

			INT32 getBaselineOffset() const { return mBaselineOffset; }
			UINT32 getLineHeight() const { return mLineHeight; }
			UINT32 getSpaceWidth() const { return mSpaceWidth; }

			const CHAR_DESC& getChar(UINT32 idx) const { return *mChars[idx]; }
			const TextWord& getWord(UINT32 idx) const { return mWords[idx]; }

		private:
			const CHAR_DESC** mChars;
			UINT32 mNumChars;

			TextWord* mWords;
			UINT32 mNumWords;

			TextLine* mLines;
			UINT32 mNumLines;

			PageInfo* mPageInfos;
			UINT32 mNumPageInfos;

			void* mData;

			HFont mFont;
			INT32 mBaselineOffset;
			UINT32 mLineHeight;
			UINT32 mSpaceWidth;
		};

		static std::shared_ptr<TextUtility::TextData> getTextData(const WString& text, const HFont& font, UINT32 fontSize, UINT32 width = 0, UINT32 height = 0, bool wordWrap = false);

	private:
		friend class TextLine;
		friend class TextWord;

		static Vector<TextWord>::type WordBuffer;
		static UINT32 NextFreeWord;

		static Vector<TextLine>::type LineBuffer;
		static UINT32 NextFreeLine;

		static Vector<PageInfo>::type PageBuffer;
		static UINT32 NextFreePageInfo;

		static UINT32 allocWord(bool spacer);
		static UINT32 allocLine(TextData* textData);
		static void deallocAll();

		static void addCharToPage(UINT32 page, const FontData& fontData);


	};
}
#pragma once

#include "CmPrerequisites.h"
#include "CmFontDesc.h"
#include "CmInt2.h"

namespace CamelotFramework
{
	class TextData
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
			friend class TextData;

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

	public:
		CM_EXPORT TextData(const WString& text, const HFont& font, UINT32 fontSize, UINT32 width = 0, UINT32 height = 0, bool wordWrap = false);
		CM_EXPORT ~TextData();

		CM_EXPORT UINT32 getNumLines() const { return mNumLines; }
		CM_EXPORT UINT32 getNumPages() const { return mNumPageInfos; }

		CM_EXPORT const TextLine& getLine(UINT32 idx) const { return mLines[idx]; }
		CM_EXPORT const HTexture& getTextureForPage(UINT32 page) const;
		CM_EXPORT UINT32 getNumQuadsForPage(UINT32 page) const { return mPageInfos[page].numQuads; }

		CM_EXPORT UINT32 getWidth() const;
		CM_EXPORT UINT32 getHeight() const;

	private:
		friend class TextLine;

		INT32 getBaselineOffset() const;
		UINT32 getLineHeight() const;
		UINT32 getSpaceWidth() const;

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
		const FontData* mFontData;

		// Static buffers
	private:
		static CM_THREADLOCAL bool BuffersInitialized;

		static CM_THREADLOCAL TextWord* WordBuffer;
		static CM_THREADLOCAL UINT32 WordBufferSize;
		static CM_THREADLOCAL UINT32 NextFreeWord;

		static CM_THREADLOCAL TextLine* LineBuffer;
		static CM_THREADLOCAL UINT32 LineBufferSize;
		static CM_THREADLOCAL UINT32 NextFreeLine;

		static CM_THREADLOCAL PageInfo* PageBuffer;
		static CM_THREADLOCAL UINT32 PageBufferSize;
		static CM_THREADLOCAL UINT32 NextFreePageInfo;

		static void initAlloc();
		static UINT32 allocWord(bool spacer);
		static UINT32 allocLine(TextData* textData);
		static void deallocAll();

		static void addCharToPage(UINT32 page, const FontData& fontData);
	};
}
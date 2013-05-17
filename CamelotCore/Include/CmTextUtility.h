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
			TextWord(bool spacer);

			void addChar(const CHAR_DESC& desc);
			void addSpace(UINT32 spaceWidth);

			void removeLastChar();

			UINT32 getWidth() const { return mWidth; }
			UINT32 getHeight() const { return mHeight; }
			bool isSpacer() const { return mSpacer; }

			const vector<CHAR_DESC>::type& getChars() const { return mChars; }

		private:
			vector<CHAR_DESC>::type mChars;
			UINT32 mWidth;
			UINT32 mHeight;
			bool mSpacer;
			UINT32 mSpaceWidth;

			void calculateWidth();
		};

	public:
		class TextLine
		{
		public:
			TextLine();
			~TextLine();

			UINT32 getWidth() const { return mWidth; }
			UINT32 getHeight() const { return mHeight; }

			/**
			 * @brief	Gets a number quads used by all characters for every page used by this text line.
			 *
			 * @note	One page generally corresponds to one bitmap from which the characters are read from.
			 * 			
			 *			One character is represented with a single quad. Some pages might be empty.
			 */
			std::vector<UINT32> getNumQuadsPerPage() const;

			/**
			 * @brief	Fills the vertex/uv/index buffers for the specified page, with all the character data needed for rendering.
			 *
			 * @param	page				The page. 
			 * @param [out]	vertices		Pre-allocated array where character vertices will be written.
			 * @param [out]	uvs				Pre-allocated array where character uv coordinates will be written.
			 * @param [out]	indexes 		Pre-allocated array where character indices will be written.
			 * @param	offset				Offsets the location at which the method writes to the buffers. Counted as number of quads.
			 * @param	size				Total number of quads that can fit into the specified buffers.
			 * @param	fontData			Information describing the font.
			 */
			void fillBuffer(UINT32 page, Vector2* vertices, Vector2* uvs, UINT32* indexes, UINT32 offset, UINT32 size, const FontData& fontData) const;

		private:
			friend class TextUtility;

			UINT32 mWidth;
			UINT32 mHeight;
			vector<TextWord*>::type mWords;
			TextWord* mLastWord;

			void add(const CHAR_DESC& charDesc);
			void addSpace(UINT32 spaceWidth);
			void addWord(TextWord* word);

			TextWord* removeLastWord();

			void calculateBounds();
		};

		class TextData
		{
		public:
			~TextData();

			const std::vector<TextLine*>& getLines() const { return mLines; }
			const std::vector<UINT32>& getNumQuadsPerPage() const  { return mQuadsPerPage; }
		private:
			friend class TextUtility;

			std::vector<UINT32> mQuadsPerPage;
			std::vector<TextLine*> mLines;
		};

		std::shared_ptr<TextUtility::TextData> getTextData(const String& text, const HFont& font, UINT32 fontSize, UINT32 width = 0, UINT32 height = 0, bool wordWrap = false);
	};
}
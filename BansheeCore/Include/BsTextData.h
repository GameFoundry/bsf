#pragma once

#include "BsCorePrerequisites.h"
#include "BsFontDesc.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	This object takes as input a string, a font and optionally some constraints (like word wrap)
	 *			and outputs a set of character data you may use for rendering or metrics.
	 */
	class TextData
	{
	private:
		/**
		 * @brief	Represents a single word as a set of characters, or optionally just a blank space
		 *			of a certain length.
		 *
		 * @note	Due to the way allocation is handled, this class is not allowed to have a destructor.
		 */
		class TextWord
		{
		public:
			/**
			 * @brief	Initializes the word and signals if it just a space (or multiple spaces), or 
			 *			an actual word with letters.
			 */
			void init(bool spacer);

			/**
			 * @brief	Appends a new character to the word.
			 *
			 * @param	charIdx		Sequential index of the character in the original string.
			 * @param	desc		Character description from the font.
			 *
			 * @returns		How many pixels did the added character expand the word by.
			 */
			UINT32 addChar(UINT32 charIdx, const CHAR_DESC& desc);

			/**
			 * @brief	Adds a space to the word. Word must have previously have been declared as
			 *			a "spacer".
			 */
			void addSpace(UINT32 spaceWidth);

			/**
			 * @brief	Returns the width of the word in pixels.
			 */
			UINT32 getWidth() const { return mWidth; }

			/**
			 * @brief	Returns height of the word in pixels.
			 */
			UINT32 getHeight() const { return mHeight; }

			/**
			 * @brief	Returns true if word is a spacer. Spacers contain just a space 
			 *			of a certain length with no actual characters.
			 */
			bool isSpacer() const { return mSpacer; }

			/**
			 * @brief	Returns the number of characters in the word.
			 */
			UINT32 getNumChars() const { return mLastChar == nullptr ? 0 : (mCharsEnd - mCharsStart + 1); }

			/**
			 * @brief	Returns the index of the starting character in the word.
			 */
			UINT32 getCharsStart() const { return mCharsStart; }

			/**
			 * @brief	Returns the index of the last character in the word.
			 */
			UINT32 getCharsEnd() const { return mCharsEnd; }

		private:
			UINT32 mCharsStart, mCharsEnd;
			UINT32 mWidth;
			UINT32 mHeight;

			const CHAR_DESC* mLastChar;

			bool mSpacer;
			UINT32 mSpaceWidth;
		};

		/**
		 * @brief	Contains information about a single texture page that contains rendered
		 *			character data.
		 *
		 * @note	Due to the way allocation is handled, this class is not allowed to have a destructor.
		 */
		struct PageInfo
		{
			UINT32 numQuads;
			HTexture texture;
		};

	public:
		/**
		 * @brief	Represents a single line as a set of words.
		 *
		 * @note	Due to the way allocation is handled, this class is not allowed to have a destructor.
		 */
		class BS_CORE_EXPORT TextLine
		{
		public:
			/**
			 * @brief	Returns width of the line in pixels.
			 */
			UINT32 getWidth() const { return mWidth; }

			/**
			 * @brief	Returns height of the line in pixels.
			 */
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

			/**
			 * @brief	Appends a new character to the line.
			 *
			 * @param	charIdx		Sequential index of the character in the original string.
			 * @param	desc		Character description from the font.
			 */
			void add(UINT32 charIdx, const CHAR_DESC& charDesc);

			/**
			 * @brief	Appends a space to the line.
			 */
			void addSpace();

			/**
			 * @brief	Adds a new word to the line.
			 *
			 * @param	wordIdx		Sequential index of the word in the original string. 
			 *						Spaces are counted as words as well.
			 * @param	word		Description of the word.
			 */
			void addWord(UINT32 wordIdx, const TextWord& word);

			/**
			 * @brief	Initializes the line. Must be called after construction.
			 */
			void init(TextData* textData);

			/**
			 * @brief	Finalizes the line. Do not add new characters/words after a line has
			 *			been finalized.
			 *
			 * @param	hasNewlineChar	Set to true if line was create due to an explicit newline char.
			 *							As opposed to a line that was created because a word couldn't fit 
			 *							on the previous line.
			 */
			void finalize(bool hasNewlineChar);

			/**
			 * @brief	Returns true if the line contains no words.
			 */
			bool isEmpty() const { return mIsEmpty; }

			/**
			 * @brief	Removes last word from the line and returns its sequential index.
			 */
			UINT32 removeLastWord();

			/**
			 * @brief	Calculates the line width and height in pixels.
			 */
			void calculateBounds();

		private:
			TextData* mTextData;
			UINT32 mWordsStart, mWordsEnd;

			UINT32 mWidth;
			UINT32 mHeight;

			bool mIsEmpty;
			bool mHasNewline;
		};

	public:
		/**
		 * @brief	Initializes a new text data using the specified string and font. Text will attempt to fit into
		 *			the provided area. It will wrap words to new line if it doesn't fit and is enabled. If word wrap
		 *			isn't possible the text will be clipped. If the specified area is zero size then the text
		 *			will not be clipped or word wrapped in any way.
		 *
		 *			After this object is constructed you may call various getter methods to get needed information.
		 */
		BS_CORE_EXPORT TextData(const WString& text, const HFont& font, UINT32 fontSize, UINT32 width = 0, UINT32 height = 0, bool wordWrap = false);
		BS_CORE_EXPORT ~TextData();

		/**
		 * @brief	Returns the number of lines that were generated.
		 */
		BS_CORE_EXPORT UINT32 getNumLines() const { return mNumLines; }

		/**
		 * @brief	Returns the number of font pages references by the used characters.
		 */
		BS_CORE_EXPORT UINT32 getNumPages() const { return mNumPageInfos; }

		/**
		 * @brief	Returns the height of a line in pixels.
		 */
		BS_CORE_EXPORT UINT32 getLineHeight() const;

		/**
		 * @brief	Gets information describing a single line at the specified index.
		 */
		BS_CORE_EXPORT const TextLine& getLine(UINT32 idx) const { return mLines[idx]; }

		/**
		 * @brief	Returns font texture for the provided page index. 
		 */
		BS_CORE_EXPORT const HTexture& getTextureForPage(UINT32 page) const;

		/**
		 * @brief	Returns the number of quads used by all the characters in the provided page.
		 */
		BS_CORE_EXPORT UINT32 getNumQuadsForPage(UINT32 page) const { return mPageInfos[page].numQuads; }

		/**
		 * @brief	Returns the width of the actual text in pixels.
		 */
		BS_CORE_EXPORT UINT32 getWidth() const;

		/**
		 * @brief	Returns the height of the actual text in pixels.
		 */
		BS_CORE_EXPORT UINT32 getHeight() const;

	private:
		friend class TextLine;

		/**
		 * @brief	Returns Y offset that determines the line on which the characters are placed.
		 *			In pixels.
		 */
		INT32 getBaselineOffset() const;

		/**
		 * @brief	Returns the width of a single space in pixels.
		 */
		UINT32 getSpaceWidth() const;

		/**
		 * @brief	Gets a description of a single character referenced by its sequential index
		 *			based on the original string.
		 */
		const CHAR_DESC& getChar(UINT32 idx) const { return *mChars[idx]; }

		/**
		 * @brief	Gets a description of a single word referenced by its sequential index
		 *			based on the original string.
		 */
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

		// Static buffers used to reduce runtime memory allocation
	private:
		static BS_THREADLOCAL bool BuffersInitialized;

		static BS_THREADLOCAL TextWord* WordBuffer;
		static BS_THREADLOCAL UINT32 WordBufferSize;
		static BS_THREADLOCAL UINT32 NextFreeWord;

		static BS_THREADLOCAL TextLine* LineBuffer;
		static BS_THREADLOCAL UINT32 LineBufferSize;
		static BS_THREADLOCAL UINT32 NextFreeLine;

		static BS_THREADLOCAL PageInfo* PageBuffer;
		static BS_THREADLOCAL UINT32 PageBufferSize;
		static BS_THREADLOCAL UINT32 NextFreePageInfo;

		/**
		 * @brief	Allocates an initial set of buffers that will be reused while parsing
		 *			text data.
		 */
		static void initAlloc();

		/**
		 * @brief	Allocates a new word and adds it to the buffer. Returns index of the word
		 *			in the word buffer.
		 *
		 * @param	spacer	Specify true if the word is only to contain spaces. (Spaces are considered
		 *					a special type of word).
		 */
		static UINT32 allocWord(bool spacer);

		/**
		 * @brief	Allocates a new line and adds it to the buffer. Returns index of the line
		 *			in the line buffer.
		 */
		static UINT32 allocLine(TextData* textData);

		/**
		 * @brief	Resets all allocation counters.
		 */
		static void deallocAll();

		/**
		 * @brief	Increments the count of characters for the referenced page, and optionally
		 *			creates page info if it doesn't already exist.
		 */
		static void addCharToPage(UINT32 page, const FontData& fontData);
	};
}
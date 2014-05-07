#pragma once

#include "BsPrerequisites.h"
#include "BsTextSprite.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIInputLineDesc
	{
	public:
		GUIInputLineDesc(UINT32 startChar, UINT32 endChar, UINT32 lineHeight, INT32 lineYStart, bool includesNewline);

		UINT32 getEndChar(bool includeNewline = true) const;
		UINT32 getStartChar() const { return mStartChar; }
		UINT32 getLineHeight() const { return mLineHeight; }
		INT32 getLineYStart() const { return mLineYStart; }
		bool isNewline(UINT32 charIdx) const;
		bool hasNewlineChar() const { return mIncludesNewline; }

	private:
		UINT32 mStartChar;
		UINT32 mEndChar;
		UINT32 mLineHeight;
		INT32 mLineYStart;
		bool mIncludesNewline;
	};

	class BS_EXPORT GUIInputTool
	{
	public:
		GUIInputTool();
		~GUIInputTool();

		void updateText(const GUIElement* element, const TEXT_SPRITE_DESC& textDesc);
	protected:
		const GUIElement* mElement;

		Vector2* mQuads;
		UINT32 mNumQuads;

		TEXT_SPRITE_DESC mTextDesc;

		Vector<GUIInputLineDesc> mLineDescs;

		Vector2I getTextOffset() const;

		UINT32 getNumLines() const { return (UINT32)mLineDescs.size(); }
		const GUIInputLineDesc& getLineDesc(UINT32 lineIdx) const { return mLineDescs.at(lineIdx); }
		UINT32 getLineForChar(UINT32 charIdx, bool newlineCountsOnNextLine = false) const;
		RectI getCharRect(UINT32 charIdx) const;
		RectI getLocalCharRect(UINT32 charIdx) const;
		INT32 getCharIdxAtPos(const Vector2I& pos) const;
		bool isDescValid() const;

		/**
		 * @brief	Gets a character index AFTER the input index. 
		 * 			"Input index" represents the empty areas between the characters. Newline counts as a character.
		 * 			(e.g. 0 is before the first character, 1 is after the first character but before the second, etc.)
		 * 			
		 * @note	This can return an out of range character index, in case the input index is specified after the last character.
		 */
		UINT32 getCharIdxAtInputIdx(UINT32 inputIdx) const;

		bool isNewlineChar(UINT32 charIdx) const;
		bool isNewline(UINT32 inputIdx) const;
	};
}
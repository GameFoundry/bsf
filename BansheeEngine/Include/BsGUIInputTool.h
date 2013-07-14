#pragma once

#include "BsPrerequisites.h"
#include "BsTextSprite.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIInputLineDesc
	{
	public:
		GUIInputLineDesc(CM::UINT32 startChar, CM::UINT32 endChar, CM::UINT32 lineHeight, CM::INT32 lineYStart, bool includesNewline);

		CM::UINT32 getEndChar(bool includeNewline = true) const;
		CM::UINT32 getStartChar() const { return mStartChar; }
		CM::UINT32 getLineHeight() const { return mLineHeight; }
		CM::INT32 getLineYStart() const { return mLineYStart; }
		bool isNewline(CM::UINT32 charIdx) const;
		bool hasNewlineChar() const { return mIncludesNewline; }

	private:
		CM::UINT32 mStartChar;
		CM::UINT32 mEndChar;
		CM::UINT32 mLineHeight;
		CM::INT32 mLineYStart;
		bool mIncludesNewline;
	};

	class BS_EXPORT GUIInputTool
	{
	public:
		GUIInputTool();
		~GUIInputTool();

		void updateText(const TEXT_SPRITE_DESC& textDesc, const CM::Int2& offset, const CM::Int2 clipOffset);
	protected:
		CM::Vector2* mQuads;
		CM::UINT32 mNumQuads;

		TEXT_SPRITE_DESC mTextDesc;
		CM::Int2 mTextOffset;
		CM::Int2 mClipOffset;

		CM::Vector<GUIInputLineDesc>::type mLineDescs;

		CM::UINT32 getNumLines() const { return (CM::UINT32)mLineDescs.size(); }
		const GUIInputLineDesc& getLineDesc(CM::UINT32 lineIdx) const { return mLineDescs.at(lineIdx); }
		CM::UINT32 getLineForChar(CM::UINT32 charIdx, bool newlineCountsOnNextLine = false) const;
		CM::Rect getCharRect(CM::UINT32 charIdx) const;
		CM::INT32 getCharIdxAtPos(const CM::Int2& pos) const;

		/**
		 * @brief	Gets a character index AFTER the input index. 
		 * 			"Input index" represents the empty areas between the characters. Newline counts as a character.
		 * 			(e.g. 0 is before the first character, 1 is after the first character but before the second, etc.)
		 * 			
		 * @note	This can return an out of range character index, in case the input index is specified after the last character.
		 */
		CM::UINT32 getCharIdxAtInputIdx(CM::UINT32 inputIdx) const;

		bool isNewlineChar(CM::UINT32 charIdx) const;
		bool isNewline(CM::UINT32 inputIdx) const;
	};
}
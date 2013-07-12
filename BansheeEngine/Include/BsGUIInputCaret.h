#pragma once

#include "BsPrerequisites.h"
#include "BsTextSprite.h"

namespace BansheeEngine
{
	enum CaretPos
	{
		CARET_BEFORE,
		CARET_AFTER
	};

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

	class BS_EXPORT GUIInputCaret
	{
	public:
		GUIInputCaret(const TEXT_SPRITE_DESC& textDesc, const CM::Int2& offset, const CM::Int2 clipOffset);
		~GUIInputCaret();

		ImageSprite* getSprite() const { return mCaretSprite; }
		CM::Int2 getSpriteOffset() const;
		CM::Rect getSpriteClipRect() const;

		void updateText(const TEXT_SPRITE_DESC& textDesc, const CM::Int2& offset, const CM::Int2 clipOffset);
		void updateSprite(const CM::Int2& offset);

		void moveCaretToStart();
		void moveCaretToEnd();
		void moveCaretLeft();
		void moveCaretRight();
		void moveCaretUp();
		void moveCaretDown();
		void moveCaretToPos(const CM::Int2& pos);
		void moveCaretToChar(CM::UINT32 charIdx, CaretPos caretPos);

		CM::UINT32 getCharIdxAtCaretPos() const;
		CM::UINT32 getCharIdxAtCaretPos(CM::UINT32 caretPos) const;
		CM::Int2 getCaretPosition(const CM::Int2& offset) const;
		CM::UINT32 getCaretHeight() const;

		bool isCaretAtNewline() const;
		CM::UINT32 getMaxCaretPos() const;
		CM::UINT32 getCaretPos() const { return mCaretPos; }
	private:
		CM::UINT32 mCaretPos;
		ImageSprite* mCaretSprite;

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
	};
}
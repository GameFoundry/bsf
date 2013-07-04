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

	class BS_EXPORT GUIInputCaret
	{
	public:
		GUIInputCaret(const TEXT_SPRITE_DESC& textDesc);
		~GUIInputCaret();

		ImageSprite* getSprite() const { return mCaretSprite; }
		void updateText(const TEXT_SPRITE_DESC& textDesc);
		void updateSprite(const CM::Int2& offset);

		void moveCaretToStart();
		void moveCaretLeft();
		void moveCaretRight();
		void moveCaretUp();
		void moveCaretDown();
		void moveCaretToPos(const CM::Int2& pos);
		void moveCaretToChar(CM::UINT32 charIdx, CaretPos caretPos);

		CM::UINT32 getCharIdxAtCaretPos() const;
		CM::Int2 getCaretPosition(const CM::Int2& offset) const;
		CM::UINT32 getCaretHeight() const;

	private:
		CM::UINT32 mCaretPos;
		TextSprite* mTextSprite; // TODO - Try to get rid of this and implement its methods internally?
		ImageSprite* mCaretSprite;

		TEXT_SPRITE_DESC mTextDesc;
	};
}
#pragma once

#include "BsPrerequisites.h"
#include "BsGUIInputTool.h"
#include "BsTextSprite.h"

namespace BansheeEngine
{
	enum CaretPos
	{
		CARET_BEFORE,
		CARET_AFTER
	};

	class BS_EXPORT GUIInputCaret : public GUIInputTool
	{
	public:
		GUIInputCaret(const TEXT_SPRITE_DESC& textDesc, const CM::Int2& offset, const CM::Int2 clipOffset);
		~GUIInputCaret();

		ImageSprite* getSprite() const { return mCaretSprite; }
		CM::Int2 getSpriteOffset() const;
		CM::Rect getSpriteClipRect() const;

		void updateSprite();

		void moveCaretToStart();
		void moveCaretToEnd();
		void moveCaretLeft();
		void moveCaretRight();
		void moveCaretUp();
		void moveCaretDown();
		void moveCaretToPos(const CM::Int2& pos);
		void moveCaretToChar(CM::UINT32 charIdx, CaretPos caretPos);

		CM::UINT32 getCharIdxAtCaretPos() const;
		CM::Int2 getCaretPosition(const CM::Int2& offset) const;
		CM::UINT32 getCaretHeight() const;

		bool isCaretAtNewline() const;
		CM::UINT32 getMaxCaretPos() const;
		CM::UINT32 getCaretPos() const { return mCaretPos; }
	private:
		CM::UINT32 mCaretPos;
		ImageSprite* mCaretSprite;
	};
}
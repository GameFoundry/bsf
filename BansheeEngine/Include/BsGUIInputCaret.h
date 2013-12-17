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
		GUIInputCaret();
		~GUIInputCaret();

		ImageSprite* getSprite() const { return mCaretSprite; }
		CM::Vector2I getSpriteOffset() const;
		CM::RectI getSpriteClipRect(const CM::RectI& parentClipRect) const;

		void updateSprite();

		void moveCaretToStart();
		void moveCaretToEnd();
		void moveCaretLeft();
		void moveCaretRight();
		void moveCaretUp();
		void moveCaretDown();
		void moveCaretToPos(const CM::Vector2I& pos);
		void moveCaretToChar(CM::UINT32 charIdx, CaretPos caretPos);

		CM::UINT32 getCharIdxAtCaretPos() const;
		CM::Vector2I getCaretPosition(const CM::Vector2I& offset) const;
		CM::UINT32 getCaretHeight() const;

		bool isCaretAtNewline() const;
		CM::UINT32 getMaxCaretPos() const;
		CM::UINT32 getCaretPos() const { return mCaretPos; }
	private:
		CM::UINT32 mCaretPos;
		ImageSprite* mCaretSprite;
	};
}
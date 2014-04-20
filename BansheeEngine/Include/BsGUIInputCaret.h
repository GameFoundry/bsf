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
		Vector2I getSpriteOffset() const;
		RectI getSpriteClipRect(const RectI& parentClipRect) const;

		void updateSprite();

		void moveCaretToStart();
		void moveCaretToEnd();
		void moveCaretLeft();
		void moveCaretRight();
		void moveCaretUp();
		void moveCaretDown();
		void moveCaretToPos(const Vector2I& pos);
		void moveCaretToChar(UINT32 charIdx, CaretPos caretPos);

		UINT32 getCharIdxAtCaretPos() const;
		Vector2I getCaretPosition(const Vector2I& offset) const;
		UINT32 getCaretHeight() const;

		bool isCaretAtNewline() const;
		UINT32 getMaxCaretPos() const;
		UINT32 getCaretPos() const { return mCaretPos; }
	private:
		UINT32 mCaretPos;
		ImageSprite* mCaretSprite;
	};
}
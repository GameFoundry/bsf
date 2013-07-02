#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{

	class BS_EXPORT GUIInputCaret
	{
	public:
		GUIInputCaret(TextSprite& sprite);
		~GUIInputCaret();

		ImageSprite& getSprite();
		void updateSprite();

		void moveCaretToStart();
		void moveCaretLeft();
		void moveCaretRight();
		void moveCaretUp();
		void moveCaretRight();
		void moveCaretToPos();

	private:
		CM::UINT32 mCaretPos;
		ImageSprite* mCaretSprite;
	};
}
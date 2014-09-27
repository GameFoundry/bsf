#pragma once

#include "BsPrerequisites.h"
#include "BsGUIInputTool.h"
#include "BsTextSprite.h"

namespace BansheeEngine
{
	/**
	 * @brief	When paired with a character index determines should the caret be placed
	 *			before or after it.
	 */
	enum CaretPos
	{
		CARET_BEFORE,
		CARET_AFTER
	};

	/**
	 * @brief	Helper class for dealing with caret for text input boxes and similar
	 *			controls.
	 */
	class BS_EXPORT GUIInputCaret : public GUIInputTool
	{
	public:
		GUIInputCaret();
		~GUIInputCaret();

		/**
		 * @brief	Returns sprite used for rendering the caret.
		 */
		ImageSprite* getSprite() const { return mCaretSprite; }

		/**
		 * @brief	Returns offset relative to parent widget that determines placement of the
		 *			caret sprite.
		 */
		Vector2I getSpriteOffset() const;

		/**
		 * @brief	Returns clip rectangle relative to parent GUI element that determines
		 *			how is caret sprite clipped.
		 *
		 * @param	parentClipRect	Clip rectangle of the parent GUI element. Caret clip rectangle will additionally be
		 *							clipped by this area. Relative to parent element.
		 */
		Rect2I getSpriteClipRect(const Rect2I& parentClipRect) const;

		/**
		 * @brief	Rebuilts internal caret sprite using current properties.
		 */
		void updateSprite();

		/**
		 * @brief	Moves caret to the start of text.
		 */
		void moveCaretToStart();

		/**
		 * @brief	Moves caret to the end of text.
		 */
		void moveCaretToEnd();

		/**
		 * @brief	Moves caret one character to the left, if not at start already.
		 */
		void moveCaretLeft();

		/**
		 * @brief	Moves caret one character to the right, if not at end already.
		 */
		void moveCaretRight();

		/**
		 * @brief	Moves caret one line up if possible.
		 */
		void moveCaretUp();

		/**
		 * @brief	Moves caret one line down if possible.
		 */
		void moveCaretDown();

		/**
		 * @brief	Moves caret to the character nearest to the specified position.
		 *			Position is relative to parent widget.
		 */
		void moveCaretToPos(const Vector2I& pos);

		/**
		 * @brief	Moves the caret to a specific character index.
		 *
		 * @param	charIdx		Index of the character to move the caret to.
		 * @param	caretPos	Whether to place the caret before or after the character.
		 */
		void moveCaretToChar(UINT32 charIdx, CaretPos caretPos);

		/**
		 * @brief	Returns character index after the current caret position.
		 */
		UINT32 getCharIdxAtCaretPos() const;

		/**
		 * @brief	Returns current caret position, relative to parent widget. Requires
		 *			you to provide offset to text the caret is used for (also relative to parent widget).
		 */
		Vector2I getCaretPosition(const Vector2I& offset) const;

		/**
		 * @brief	Returns height of the caret, in pixels.
		 */
		UINT32 getCaretHeight() const;

		/**
		 * @brief	Returns true if the character after the caret is  newline.
		 */
		bool isCaretAtNewline() const;

		/**
		 * @brief	Returns maximum valid caret index.
		 */
		UINT32 getMaxCaretPos() const;

		/**
		 * @brief	Returns current caret index (not equal to character index).
		 */
		UINT32 getCaretPos() const { return mCaretPos; }
	private:
		UINT32 mCaretPos;
		ImageSprite* mCaretSprite;
	};
}
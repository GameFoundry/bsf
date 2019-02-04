//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPrerequisites.h"
#include "GUI/BsGUIInputTool.h"
#include "2D/BsTextSprite.h"

namespace bs
{
	/** @addtogroup GUI-Internal
	 *  @{
	 */

	/**	Helper class for dealing with text selection for text input boxes and similar controls. */
	class BS_EXPORT GUIInputSelection : public GUIInputTool
	{
	public:
		GUIInputSelection() = default;
		~GUIInputSelection();

		/**	Returns sprites representing the currently selected areas. */
		const Vector<ImageSprite*>& getSprites() const { return mSprites; }

		/** Returns how much to offset the sprite with the specified index, relative to the parent widget. */
		Vector2I getSelectionSpriteOffset(UINT32 spriteIdx) const;

		/**
		 * Returns clip rectangle relative to parent GUI element for the sprite with the specified index. 
		 *
		 * @param[in]	spriteIdx		Index of the sprite to retrieve the clip rectangle for.
		 * @param[in]	parentClipRect	Clip rectangle of the parent GUI element. Selection clip rectangle will 
		 *								additionally be clipped by this area. Relative to parent element.
		 */
		Rect2I getSelectionSpriteClipRect(UINT32 spriteIdx, const Rect2I& parentClipRect) const;

		/**	Recreates the selection clip sprites. */
		void updateSprite();

		/**
		 * Shows the selection using the specified anchor. By default this will select 0 characters so you must manually
		 * move the selection using moveSelectionToCaret() before anything is considered selected.
		 *
		 * @param[in]	anchorCaretPos	Anchor position which to initially select. Anchor position determines selection 
		 *								area behavior when the input caret moves (determines whether left or right side of
		 *								the selection will move with the caret).
		 */
		void showSelection(UINT32 anchorCaretPos);

		/**	
		 * Clears the currently active selection. Note this does not clear the internal selection range, just the
		 * selection sprites.
		 */
		void clearSelectionVisuals();

		/**
		 * Moves the selection to caret. Selected area will be from the anchor provided in showSelection() to the caret
		 * position provided here.
		 */
		void moveSelectionToCaret(UINT32 caretPos);

		/**	Checks is anything selected. */
		bool isSelectionEmpty() const;

		/**	Selects all available text. */
		void selectAll();

		/**
		 * Starts selection drag at the specified caret position. Call selectionDragUpdate() and selectionDragEnd() as the
		 * drag operation progresses.
		 */
		void selectionDragStart(UINT32 caretPos);

		/**	Updates selection drag at the specified caret position. */
		void selectionDragUpdate(UINT32 caretPos);

		/**	Stops selection drag. */
		void selectionDragEnd();

		/**	Gets caret index of selection start. */
		UINT32 getSelectionStart() const { return mSelectionStart; }

		/**	Gets caret index of selection end. */
		UINT32 getSelectionEnd() const { return mSelectionEnd; }

	private:
		/** Returns rectangles describing the currently selected areas. Rectangles are relative to parent GUI element. */
		Vector<Rect2I> getSelectionRects() const;

	private:
		UINT32 mSelectionStart = 0;
		UINT32 mSelectionEnd = 0;
		UINT32 mSelectionAnchor = 0;
		UINT32 mSelectionDragAnchor = 0;

		Vector<Rect2I> mSelectionRects;
		Vector<ImageSprite*> mSprites;
	};

	/** @} */
}
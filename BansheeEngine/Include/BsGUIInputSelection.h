//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"
#include "BsGUIInputTool.h"
#include "BsTextSprite.h"

namespace BansheeEngine
{
	/**
	 * @brief	Helper class for dealing with text selection for text input boxes and similar
	 *			controls.
	 */
	class BS_EXPORT GUIInputSelection : public GUIInputTool
	{
	public:
		GUIInputSelection();
		~GUIInputSelection();

		/**
		 * @brief	Returns sprites representing the currently selected areas
		 */
		const Vector<ImageSprite*>& getSprites() const { return mSprites; }

		/**
		 * @brief	Returns how much to offset the sprite with the specified index, relative to
		 *			the parent widget.
		 */
		Vector2I GUIInputSelection::getSelectionSpriteOffset(UINT32 spriteIdx) const;

		/**
		 * @brief	Returns clip rectangle relative to parent GUI element for the sprite with the specified index. 
		 *
		 * @param	spriteIdx	Index of the sprite to retrieve the clip rectangle for.
		 * @param	parentClipRect	Clip rectangle of the parent GUI element. Selection clip rectangle will additionally be
		 *							clipped by this area. Relative to parent element.
		 */
		RectI GUIInputSelection::getSelectionSpriteClipRect(UINT32 spriteIdx, const RectI& parentClipRect) const;

		/**
		 * @brief	Recreates the selection clip sprites.
		 */
		void updateSprite();

		/**
		 * @brief	Shows the selection using the specified anchor. By default this will select 0 characters so you
		 *			must manually move the selection using "moveSelectionToCaret" before anything is considered selected.
		 *
		 * @param	anchorCaretPos	Anchor position which to initially select. Anchor position determines selection area behavior
		 *							when the input caret moves (determines whether left or right side of the selection will move with the caret).
		 */
		void showSelection(UINT32 anchorCaretPos);

		/**
		 * @brief	Clears the currently active selection.
		 */
		void clearSelection();

		/**
		 * @brief	Moves the selection to caret. Selected area will be from the anchor provided in
		 *			"showSelection" to the caret position provided here.
		 */
		void moveSelectionToCaret(UINT32 caretPos);

		/**
		 * @brief	Checks is anything selected.
		 */
		bool isSelectionEmpty() const;

		/**
		 * @brief	Selects all available text.
		 */
		void selectAll();

		/**
		 * @brief	Starts selection drag at the specified caret position. Call
		 *			"selectionDragUpdate" and "selectionDragEnd" as the drag operation
		 *			progresses.
		 */
		void selectionDragStart(UINT32 caretPos);

		/**
		 * @brief	Updates selection drag at the specified caret position. 
		 */
		void selectionDragUpdate(UINT32 caretPos);

		/**
		 * @brief	Stops selection drag.
		 */
		void selectionDragEnd();

		/**
		 * @brief	Gets caret index of selection start.
		 */
		UINT32 getSelectionStart() const { return mSelectionStart; }

		/**
		 * @brief	Gets caret index of selection end.
		 */
		UINT32 getSelectionEnd() const { return mSelectionEnd; }

	private:
		/**
		 * @brief	Returns rectangles describing the currently selected areas. Rectangles
		 *			are relative to parent GUI element.
		 */
		Vector<RectI> getSelectionRects() const;

	private:
		UINT32 mSelectionStart;
		UINT32 mSelectionEnd;
		UINT32 mSelectionAnchor;
		UINT32 mSelectionDragAnchor;

		Vector<RectI> mSelectionRects;
		Vector<ImageSprite*> mSprites;
	};
}
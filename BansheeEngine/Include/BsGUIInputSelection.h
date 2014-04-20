#pragma once

#include "BsPrerequisites.h"
#include "BsGUIInputTool.h"
#include "BsTextSprite.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIInputSelection : public GUIInputTool
	{
	public:
		GUIInputSelection();
		~GUIInputSelection();

		const Vector<ImageSprite*>::type& getSprites() const { return mSprites; }
		Vector2I GUIInputSelection::getSelectionSpriteOffset(UINT32 spriteIdx) const;
		RectI GUIInputSelection::getSelectionSpriteClipRect(UINT32 spriteIdx, const RectI& parentClipRect) const;
		void updateSprite();

		void showSelection(UINT32 anchorCaretPos);
		void clearSelection();

		void moveSelectionToCaret(UINT32 caretPos);
		bool isSelectionEmpty() const;
		void selectAll();

		void selectionDragStart(UINT32 caretPos);
		void selectionDragUpdate(UINT32 caretPos);
		void selectionDragEnd();

		UINT32 getSelectionStart() const { return mSelectionStart; }
		UINT32 getSelectionEnd() const { return mSelectionEnd; }

	private:
		UINT32 mSelectionStart;
		UINT32 mSelectionEnd;
		UINT32 mSelectionAnchor;
		UINT32 mSelectionDragAnchor;

		Vector<RectI>::type mSelectionRects;
		Vector<ImageSprite*>::type mSprites;

		Vector<RectI>::type getSelectionRects() const;
	};
}
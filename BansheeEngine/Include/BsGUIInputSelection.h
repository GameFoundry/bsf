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

		const CM::Vector<ImageSprite*>::type& getSprites() const { return mSprites; }
		CM::Vector2I GUIInputSelection::getSelectionSpriteOffset(CM::UINT32 spriteIdx) const;
		CM::RectI GUIInputSelection::getSelectionSpriteClipRect(CM::UINT32 spriteIdx, const CM::RectI& parentClipRect) const;
		void updateSprite();

		void showSelection(CM::UINT32 anchorCaretPos);
		void clearSelection();

		void moveSelectionToCaret(CM::UINT32 caretPos);
		bool isSelectionEmpty() const;
		void selectAll();

		void selectionDragStart(CM::UINT32 caretPos);
		void selectionDragUpdate(CM::UINT32 caretPos);
		void selectionDragEnd();

		CM::UINT32 getSelectionStart() const { return mSelectionStart; }
		CM::UINT32 getSelectionEnd() const { return mSelectionEnd; }

	private:
		CM::UINT32 mSelectionStart;
		CM::UINT32 mSelectionEnd;
		CM::UINT32 mSelectionAnchor;
		CM::UINT32 mSelectionDragAnchor;

		CM::Vector<CM::RectI>::type mSelectionRects;
		CM::Vector<ImageSprite*>::type mSprites;

		CM::Vector<CM::RectI>::type getSelectionRects() const;
	};
}
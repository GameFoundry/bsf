#pragma once

#include "BsPrerequisites.h"
#include "BsGUIInputTool.h"
#include "BsTextSprite.h"

namespace BansheeEngine
{
	enum class SelectionDir
	{
		Left,
		Right
	};

	class BS_EXPORT GUIInputSelection : public GUIInputTool
	{
	public:
		GUIInputSelection(const TEXT_SPRITE_DESC& textDesc, const CM::Int2& offset, const CM::Int2 clipOffset);
		~GUIInputSelection();

		const CM::Vector<ImageSprite*>::type& getSprites() const { return mSprites; }
		CM::Int2 GUIInputSelection::getSelectionSpriteOffset(CM::UINT32 spriteIdx) const;
		CM::Rect GUIInputSelection::getSelectionSpriteClipRect(CM::UINT32 spriteIdx) const;
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
		bool isAnchorAtStart() const { return mSelectionStart == mSelectionAnchor; }

	private:
		CM::UINT32 mSelectionStart;
		CM::UINT32 mSelectionEnd;
		CM::UINT32 mSelectionAnchor;
		CM::UINT32 mSelectionDragAnchor;

		CM::Vector<CM::Rect>::type mSelectionRects;
		CM::Vector<ImageSprite*>::type mSprites;

		CM::UINT32 caretPosToSelectionChar(CM::UINT32 caretPos, SelectionDir dir) const;
		CM::Vector<CM::Rect>::type getSelectionRects() const;
	};
}
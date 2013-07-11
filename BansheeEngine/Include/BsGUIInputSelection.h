#pragma once

#include "BsPrerequisites.h"
#include "BsTextSprite.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIInputSelection
	{
	public:
		GUIInputSelection(const TEXT_SPRITE_DESC& textDesc);
		~GUIInputSelection();

		CM::Vector<ImageSprite*>::type getSprites() const { return mSprites; }
		void updateText(const TEXT_SPRITE_DESC& textDesc);
		void updateSprite(const CM::Int2& offset);

	private:
		CM::UINT32 mSelectionStart;
		CM::UINT32 mSelectionEnd;
		CM::UINT32 mSelectionAnchor;
		CM::UINT32 mSelectionDragAnchor;
		TextSprite* mTextSprite; // TODO - Try to get rid of this and implement its methods internally?
		CM::Vector<ImageSprite*>::type mSprites;

		TEXT_SPRITE_DESC mTextDesc;

		CM::Vector<CM::Rect>::type getSelectionRects() const;
		bool isNewlineChar(CM::UINT32 charIdx) const;
	};
}
#pragma once

#include "BsPrerequisites.h"
#include "BsSprite.h"

namespace BansheeEngine
{
	enum TextHorzAlign
	{
		THA_Left, THA_Center, THA_Right
	};

	enum TextVertAlign
	{
		TVA_Top, TVA_Center, TVA_Bottom
	};

	struct TEXT_SPRITE_DESC
	{
		TEXT_SPRITE_DESC()
			:width(0), height(0), anchor(SA_TopLeft), fontSize(0),
			horzAlign(THA_Left), vertAlign(TVA_Top), wordWrap(false)
		{ }

		CM::Point offset;
		UINT32 width;
		UINT32 height;
		CM::Rect clipRect;
		SpriteAnchor anchor;

		CM::String text;
		CM::HFont font;
		UINT32 fontSize;
		TextHorzAlign horzAlign;
		TextVertAlign vertAlign;
		bool wordWrap;
	};

	class BS_EXPORT TextSprite : public Sprite
	{
	public:
		TextSprite();

		void update(const TEXT_SPRITE_DESC& desc);
	};
}
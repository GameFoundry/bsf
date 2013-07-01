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

		CM::Int2 offset;
		CM::UINT32 width;
		CM::UINT32 height;
		CM::Rect clipRect;
		SpriteAnchor anchor;

		CM::WString text;
		CM::HFont font;
		CM::UINT32 fontSize;
		TextHorzAlign horzAlign;
		TextVertAlign vertAlign;
		bool wordWrap;
	};

	struct SpriteLineDesc
	{
		CM::UINT32 startChar;
		CM::UINT32 endChar;
		CM::UINT32 lineHeight;
		CM::INT32 lineYStart;
	};

	class BS_EXPORT TextSprite : public Sprite
	{
	public:
		TextSprite();

		void update(const TEXT_SPRITE_DESC& desc);

		CM::UINT32 getNumLines() const { return (CM::UINT32)mLineDescs.size(); }
		const SpriteLineDesc& getLineDesc(CM::UINT32 lineIdx) const { return mLineDescs.at(lineIdx); }
		CM::UINT32 getLineForChar(CM::UINT32 charIdx) const;
		CM::Rect getCharRect(CM::UINT32 charIdx) const;
		CM::INT32 getCharIdxAtPos(const CM::Int2& pos) const;

	private:
		CM::Vector<SpriteLineDesc>::type mLineDescs;
	};
}
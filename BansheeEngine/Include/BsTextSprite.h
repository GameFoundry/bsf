#pragma once

#include "BsPrerequisites.h"
#include "BsSprite.h"
#include "CmTextUtility.h"

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

	class BS_EXPORT SpriteLineDesc
	{
	public:
		SpriteLineDesc(CM::UINT32 startChar, CM::UINT32 endChar, CM::UINT32 lineHeight, CM::INT32 lineYStart, bool includesNewline);

		CM::UINT32 getEndChar(bool includeNewline = true) const;
		CM::UINT32 getStartChar() const { return mStartChar; }
		CM::UINT32 getLineHeight() const { return mLineHeight; }
		CM::INT32 getLineYStart() const { return mLineYStart; }
		bool isNewline(CM::UINT32 charIdx) const;
		bool hasNewlineChar() const { return mIncludesNewline; }

	private:
		CM::UINT32 mStartChar;
		CM::UINT32 mEndChar;
		CM::UINT32 mLineHeight;
		CM::INT32 mLineYStart;
		bool mIncludesNewline;
	};

	class BS_EXPORT TextSprite : public Sprite
	{
	public:
		TextSprite();

		void update(const TEXT_SPRITE_DESC& desc);

		CM::UINT32 getNumLines() const { return (CM::UINT32)mLineDescs.size(); }
		const SpriteLineDesc& getLineDesc(CM::UINT32 lineIdx) const { return mLineDescs.at(lineIdx); }
		CM::UINT32 getLineForChar(CM::UINT32 charIdx, bool newlineCountsOnNextLine = false) const;
		CM::Rect getCharRect(CM::UINT32 charIdx) const;
		CM::INT32 getCharIdxAtPos(const CM::Int2& pos) const;

		static CM::Vector<CM::Int2>::type getAlignmentOffsets(const CM::Vector<CM::TextUtility::TextLine>::type& lines, 
			CM::UINT32 width, CM::UINT32 height, TextHorzAlign horzAlign, TextVertAlign vertAlign);
	private:
		CM::Vector<SpriteLineDesc>::type mLineDescs;
	};
}
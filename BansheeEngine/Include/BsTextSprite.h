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

	class BS_EXPORT TextSprite : public Sprite
	{
	public:
		TextSprite();

		void update(const TEXT_SPRITE_DESC& desc);

		/**
		 * @brief	Populates the provided buffer with vertices for the individual characters described
		 * 			by the descriptor structure. Characters will be in the same position as if they were being drawn on
		 * 			the screen using TextSprite directly.
		 *
		 * @param	desc				What text to get vertices for, what font, what size, etc.
		 * @param   [out]	vertices	Pre-allocated array with enough space to hold all vertices.						
		 *
		 * @return	Number of text quads. (1 quad per character)
		 * 			
		 * @note	This method should be called twice. Once with "vertices" as nullptr to receive number
		 * 			of quads you will need, then you should allocate the vertex array of enough size and
		 * 			send it to the second call. (4 * numQuads)
		 */
		static UINT32 getTextVertices(const TEXT_SPRITE_DESC& desc, CM::Vector2* vertices);
	};
}
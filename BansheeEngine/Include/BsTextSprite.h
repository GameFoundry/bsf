#pragma once

#include "BsPrerequisites.h"
#include "BsSprite.h"
#include "CmTextData.h"
#include "CmColor.h"

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

		UINT32 width;
		UINT32 height;
		SpriteAnchor anchor;

		WString text;
		HFont font;
		UINT32 fontSize;
		Color color;
		TextHorzAlign horzAlign;
		TextVertAlign vertAlign;
		bool wordWrap;
	};

	class BS_EXPORT TextSprite : public Sprite
	{
	public:
		TextSprite();

		void update(const TEXT_SPRITE_DESC& desc);

		static Vector<Vector2I> getAlignmentOffsets(const TextData& textData, 
			UINT32 width, UINT32 height, TextHorzAlign horzAlign, TextVertAlign vertAlign);

		/**
		 * @brief	Calculates text quads you may use for text rendering, based on the specified text
		 * 			data. Only generates quads for the specified page.
		 * 			
		 * 			You must provide pre-allocated vertex/uv/index buffers of adequate size to hold all
		 * 			quads for the specified page.
		 * 			
		 * @note	uv and/or index array may be null.
		 *
		 * @return	Number of generated quads.
		 */
		static UINT32 genTextQuads(UINT32 page, const TextData& textData, UINT32 width, UINT32 height, 
			TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, Vector2* vertices, Vector2* uv, UINT32* indices, 
			UINT32 bufferSizeQuads);

		/**
		 * @brief	Calculates text quads you may use for text rendering, based on the specified text data. Generates quads for all pages.
		 * 			
		 *			You must provide pre-allocated vertex/uv/index buffers of adequate size to hold all quads for all characters
		 *			on all pages.
		 *			
		 * @note	uv and/or index array may be null.
		 *
		 * @return	Number of generated quads.
		 */
		static UINT32 genTextQuads(const TextData& textData, UINT32 width, UINT32 height, 
			TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, Vector2* vertices, Vector2* uv, UINT32* indices, 
			UINT32 bufferSizeQuads);
	};
}
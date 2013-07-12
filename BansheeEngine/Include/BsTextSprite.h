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

		CM::UINT32 width;
		CM::UINT32 height;
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

		static CM::Vector<CM::Int2>::type getAlignmentOffsets(const CM::Vector<CM::TextUtility::TextLine>::type& lines, 
			CM::UINT32 width, CM::UINT32 height, TextHorzAlign horzAlign, TextVertAlign vertAlign);

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
		static CM::UINT32 genTextQuads(CM::UINT32 page, const CM::TextUtility::TextData& textData, CM::UINT32 width, CM::UINT32 height, 
			TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, CM::Vector2* vertices, CM::Vector2* uv, CM::UINT32* indices, 
			CM::UINT32 bufferSizeQuads);

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
		static CM::UINT32 genTextQuads(const CM::TextUtility::TextData& textData, CM::UINT32 width, CM::UINT32 height, 
			TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, CM::Vector2* vertices, CM::Vector2* uv, CM::UINT32* indices, 
			CM::UINT32 bufferSizeQuads);
	};
}
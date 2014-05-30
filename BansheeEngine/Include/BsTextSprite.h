#pragma once

#include "BsPrerequisites.h"
#include "BsSprite.h"
#include "BsTextData.h"
#include "BsColor.h"

namespace BansheeEngine
{
	/**
	 * @brief	Specifies how is text horizontally aligned within its bounds.
	 */
	enum TextHorzAlign
	{
		THA_Left, THA_Center, THA_Right
	};

	/**
	 * @brief	Specifies how is text vertically aligned within its bounds.
	 */
	enum TextVertAlign
	{
		TVA_Top, TVA_Center, TVA_Bottom
	};

	/**
	 * @brief	Text sprite description structure used for initializing or updating a text sprite.
	 */
	struct TEXT_SPRITE_DESC
	{
		TEXT_SPRITE_DESC()
			:width(0), height(0), anchor(SA_TopLeft), fontSize(0),
			horzAlign(THA_Left), vertAlign(TVA_Top), wordWrap(false)
		{ }

		UINT32 width; /**< Width of the bounds to render the text within, in pixels. */
		UINT32 height; /**< Height of the bounds to render the text within, in pixels. */
		SpriteAnchor anchor; /**< Determines how to anchor the text within the bounds. */

		WString text; /**< Text to generate geometry for. */
		HFont font; /**< Font containing the data about character glyphs. */
		UINT32 fontSize; /**< Size of the font to use when displaying the text. */
		Color color; /**< Color tint of the text. */
		TextHorzAlign horzAlign; /**< Specifies how is text horizontally aligned within its bounds. */
		TextVertAlign vertAlign; /**< Specifies how is text vertically aligned within its bounds. */
		bool wordWrap; /**< If true the text will word wrap when it doesn't fit, otherwise it will overflow. */
	};

	/**
	 * @brief	A sprite consisting of a quads representing a text string.
	 */
	class BS_EXPORT TextSprite : public Sprite
	{
	public:
		TextSprite();

		/**
		 * @brief	Recreates internal sprite data according the specified description structure.
		 */
		void update(const TEXT_SPRITE_DESC& desc);

		/**
		 * @brief	Calculates and returns offset for each individual text line. The offsets provide
		 *			information on how much to offset the lines within provided bounds.
		 *
		 * @param	textData	Text data to generate offsets for.
		 * @param	width		Width of the text bounds into which to constrain the text, in pixels.
		 * @param	height		Height of the text bounds into which to constrain the text, in pixels.
		 * @param	horzAlign	Specifies how is text horizontally aligned within its bounds.
		 * @param	vertAlign	Specifies how is text vertically aligned within its bounds.
		 */
		static Vector<Vector2I> getAlignmentOffsets(const TextData& textData, 
			UINT32 width, UINT32 height, TextHorzAlign horzAlign, TextVertAlign vertAlign);

		/**
		 * @brief	Calculates text quads you may use for text rendering, based on the specified text
		 * 			data. Only generates quads for the specified page.
		 *
		 * @param	page			Font page to generate the data for.
		 * @param	textData		Text data to generate offsets for.
		 * @param	width			Width of the text bounds into which to constrain the text, in pixels.
		 * @param	height			Height of the text bounds into which to constrain the text, in pixels.
		 * @param	horzAlign		Specifies how is text horizontally aligned within its bounds.
		 * @param	vertAlign		Specifies how is text vertically aligned within its bounds.
		 * @param	anchor			Determines how to anchor the text within the bounds.
		 * @param	vertices		Output buffer containing quad positions. Must be allocated and of adequate size.
		 * @param	uv				Output buffer containing quad UV coordinates. Must be allocated and of adequate size. Can be null.
		 * @param	indices			Output buffer containing quad indices. Must be allocated and of adequate size. Can be null.
		 * @param	bufferSizeQuads	Size of the output buffers, in number of quads.
		 *
		 * @returns	Number of generated quads.
		 */
		static UINT32 genTextQuads(UINT32 page, const TextData& textData, UINT32 width, UINT32 height, 
			TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, Vector2* vertices, Vector2* uv, UINT32* indices, 
			UINT32 bufferSizeQuads);

		/**
		 * @brief	Calculates text quads you may use for text rendering, based on the specified text data. Generates quads for all pages.
		 * 			
		 * @param	textData		Text data to generate offsets for.
		 * @param	width			Width of the text bounds into which to constrain the text, in pixels.
		 * @param	height			Height of the text bounds into which to constrain the text, in pixels.
		 * @param	horzAlign		Specifies how is text horizontally aligned within its bounds.
		 * @param	vertAlign		Specifies how is text vertically aligned within its bounds.
		 * @param	anchor			Determines how to anchor the text within the bounds.
		 * @param	vertices		Output buffer containing quad positions. Must be allocated and of adequate size.
		 * @param	uv				Output buffer containing quad UV coordinates. Must be allocated and of adequate size. Can be null.
		 * @param	indices			Output buffer containing quad indices. Must be allocated and of adequate size. Can be null.
		 * @param	bufferSizeQuads	Size of the output buffers, in number of quads.
		 *
		 * @returns	Number of generated quads.
		 */
		static UINT32 genTextQuads(const TextData& textData, UINT32 width, UINT32 height, 
			TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, Vector2* vertices, Vector2* uv, UINT32* indices, 
			UINT32 bufferSizeQuads);
	};
}
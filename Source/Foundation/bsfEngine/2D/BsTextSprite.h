//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPrerequisites.h"
#include "2D/BsSprite.h"
#include "Text/BsTextData.h"
#include "Image/BsColor.h"
#include "Math/BsVector2.h"
#include "Allocators/BsStaticAlloc.h"

namespace bs
{
	/** @addtogroup 2D
	 *  @{
	 */

	/**	Specifies how is text horizontally aligned within its bounds. */
	enum BS_SCRIPT_EXPORT(m:GUI) TextHorzAlign
	{
		THA_Left	BS_SCRIPT_EXPORT(n:Left),
		THA_Center	BS_SCRIPT_EXPORT(n:Center),
		THA_Right	BS_SCRIPT_EXPORT(n:Right)
	};

	/**	Specifies how is text vertically aligned within its bounds. */
	enum BS_SCRIPT_EXPORT(m:GUI) TextVertAlign
	{
		TVA_Top		BS_SCRIPT_EXPORT(n:Top),
		TVA_Center	BS_SCRIPT_EXPORT(n:Center),
		TVA_Bottom	BS_SCRIPT_EXPORT(n:Bottom)
	};

	/**	Text sprite description structure used for initializing or updating a text sprite. */
	struct TEXT_SPRITE_DESC
	{
		TEXT_SPRITE_DESC() = default;

		UINT32 width = 0; /**< Width of the bounds to render the text within, in pixels. */
		UINT32 height = 0; /**< Height of the bounds to render the text within, in pixels. */
		SpriteAnchor anchor = SA_TopLeft; /**< Determines how to anchor the text within the bounds. */

		String text; /**< UTF-8 encoded text to generate geometry for. */
		HFont font; /**< Font containing the data about character glyphs. */
		UINT32 fontSize = 0; /**< Size of the font to use when displaying the text. */
		Color color; /**< Color tint of the text. */
		TextHorzAlign horzAlign = THA_Left; /**< Specifies how is text horizontally aligned within its bounds. */
		TextVertAlign vertAlign = TVA_Top; /**< Specifies how is text vertically aligned within its bounds. */
		bool wordWrap = false; /**< If true the text will word wrap when it doesn't fit, otherwise it will overflow. */
		bool wordBreak = true; /**< If enabled together with word wrap it will allow words to be broken if they don't fit. */
	};

	/**	A sprite consisting of a quads representing a text string. */
	class BS_EXPORT TextSprite : public Sprite
	{
	public:
		TextSprite() = default;
		~TextSprite();

		/**
		 * Recreates internal sprite data according the specified description structure.
		 *
		 * @param[in]	desc	Describes the geometry and material of the sprite.
		 * @param[in]	groupId	Group identifier that forces different materials to be used for different groups (for
		 *						example you don't want the sprites to share the same group if they use different world
		 *						transform matrices).
		 */
		void update(const TEXT_SPRITE_DESC& desc, UINT64 groupId);

		/**
		 * Calculates and returns offset for each individual text line. The offsets provide information on how much to
		 * offset the lines within provided bounds.
		 *
		 * @param[in]	textData	Text data to generate offsets for.
		 * @param[in]	width		Width of the text bounds into which to constrain the text, in pixels.
		 * @param[in]	height		Height of the text bounds into which to constrain the text, in pixels.
		 * @param[in]	horzAlign	Specifies how is text horizontally aligned within its bounds.
		 * @param[in]	vertAlign	Specifies how is text vertically aligned within its bounds.
		 * @param[out]	output		Pre-allocated buffer to output the results in. Buffer must have an element
		 *							for every line in @p textData.
		 */
		static void getAlignmentOffsets(const TextDataBase& textData,
			UINT32 width, UINT32 height, TextHorzAlign horzAlign, TextVertAlign vertAlign, Vector2I* output);

		/**
		 * Calculates text quads you may use for text rendering, based on the specified text data. Only generates quads for
		 * the specified page.
		 *
		 * @param[in]	page			Font page to generate the data for.
		 * @param[in]	textData		Text data to generate offsets for.
		 * @param[in]	width			Width of the text bounds into which to constrain the text, in pixels.
		 * @param[in]	height			Height of the text bounds into which to constrain the text, in pixels.
		 * @param[in]	horzAlign		Specifies how is text horizontally aligned within its bounds.
		 * @param[in]	vertAlign		Specifies how is text vertically aligned within its bounds.
		 * @param[in]	anchor			Determines how to anchor the text within the bounds.
		 * @param[out]	vertices		Output buffer containing quad positions. Must be allocated and of adequate size.
		 * @param[out]	uv				Output buffer containing quad UV coordinates. Must be allocated and of adequate
		 *								size. Can be null.
		 * @param[out]	indices			Output buffer containing quad indices. Must be allocated and of adequate size. Can
		 *								be null.
		 * @param[in]	bufferSizeQuads	Size of the output buffers, in number of quads.
		 * @return						Number of generated quads.
		 */
		static UINT32 genTextQuads(UINT32 page, const TextDataBase& textData, UINT32 width, UINT32 height,
			TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, Vector2* vertices, Vector2* uv, UINT32* indices,
			UINT32 bufferSizeQuads);

		/**
		 * Calculates text quads you may use for text rendering, based on the specified text data. Generates quads for all
		 * pages.
		 * 			
		 * @param[in]	textData		Text data to generate offsets for.
		 * @param[in]	width			Width of the text bounds into which to constrain the text, in pixels.
		 * @param[in]	height			Height of the text bounds into which to constrain the text, in pixels.
		 * @param[in]	horzAlign		Specifies how is text horizontally aligned within its bounds.
		 * @param[in]	vertAlign		Specifies how is text vertically aligned within its bounds.
		 * @param[in]	anchor			Determines how to anchor the text within the bounds.
		 * @param[out]	vertices		Output buffer containing quad positions. Must be allocated and of adequate size.
		 * @param[out]	uv				Output buffer containing quad UV coordinates. Must be allocated and of adequate
		 *								size. Can be null.
		 * @param[out]	indices			Output buffer containing quad indices. Must be allocated and of adequate size. Can
		 *								be null.
		 * @param[in]	bufferSizeQuads	Size of the output buffers, in number of quads.
		 * @return						Number of generated quads.
		 */
		static UINT32 genTextQuads(const TextDataBase& textData, UINT32 width, UINT32 height,
			TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, Vector2* vertices, Vector2* uv, UINT32* indices,
			UINT32 bufferSizeQuads);

	private:
		static const int STATIC_CHARS_TO_BUFFER = 25;
		static const int STATIC_BUFFER_SIZE = STATIC_CHARS_TO_BUFFER * (4 * (2 * sizeof(Vector2)) + (6 * sizeof(UINT32)));

		/**	Clears internal geometry buffers. */
		void clearMesh();

		mutable StaticAlloc<STATIC_BUFFER_SIZE> mAlloc;
	};

	/** @} */
}

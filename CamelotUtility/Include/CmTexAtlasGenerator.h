#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmVector2.h"

namespace CamelotEngine
{
	/**
	 * @brief	Represents a single element used as in input to
	 * 			TexAtlasGenerator. Usually represents a single texture.
	 * 			
	 * @note	input is required to be filled in before passing it to
	 * 			TexAtlasGenerator
	 * 			output will be filled in by TexAtlasGenerator after a call to
	 * 			TexAtlasGenerator::createAtlasLayout
	 */
	struct TexAtlasElementDesc
	{
		struct
		{
			UINT32 width, height;
		} input;
		
		struct
		{
			UINT32 x, y;
			INT32 page;
		} output;
	};

	/**
	 * @brief	A single page of the texture atlas.
	 */
	struct TexAtlasPageDesc
	{
		UINT32 width, height;
	};

	class TexAtlasNode;

	class CM_UTILITY_EXPORT TexAtlasGenerator
	{
	public:
		/**
		 * @brief	Constructor.
		 *
		 * @param	square			(optional) Should the returned texture always be square. (width == height)
		 * 							This option is only used if "fixedSize" parameter is set to false.
		 * @param	maxTexWidth 	(optional) Maximum width of the texture. 
		 * @param	maxTexHeight	(optional) Maximum height of the texture. 
		 * @param	fixedSize   	(optional) If this field is false, algorithm will try to reduce the size of the texture
		 * 							if possible. If it is true, the algorithm will always produce textures of the specified
		 * 							"maxTexWidth", "maxTexHeight" size.
		 */
		TexAtlasGenerator(bool square = false, UINT32 maxTexWidth = 2048, UINT32 maxTexHeight = 2048, bool fixedSize = false);

		/**
		 * @brief	Creates an optimal texture layout by packing texture elements in order to end up with
		 * 			as little empty space as possible.
		 *
		 * @param	elements	Elements to process. They need to have their "input" structures filled in,
		 * 						and this method will fill "output" when it returns.
		 * 						
		 * @note	Algorithm will split elements over multiple textures if they don't fit
		 * 						in a single texture (Determined by maximum texture size)
		 *
		 * @return	One or more descriptors that determine the size of the final atlas textures. Texture elements
		 * 			will reference these pages with their "output.page" parameter.
		 */
		vector<TexAtlasPageDesc>::type createAtlasLayout(vector<TexAtlasElementDesc>::type& elements) const;

	private:
		bool mSquare;
		bool mFixedSize;
		UINT32 mMaxTexWidth;
		UINT32 mMaxTexHeight;

		int generatePagesForSize(vector<TexAtlasElementDesc>::type& elements, UINT32 width, UINT32 height, UINT32 startPage = 0) const;
		int addLargestTextureWithoutPageThatFits(vector<TexAtlasElementDesc>::type& elements, TexAtlasNode& node) const;
		int findLargestTextureWithoutPage(const vector<TexAtlasElementDesc>::type& elements) const;
		void sortBySize(vector<TexAtlasElementDesc>::type& elements) const;
	};
}
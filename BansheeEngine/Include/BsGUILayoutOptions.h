#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Options that control how an element is positioned and sized in a GUI layout.
	 */
	struct GUILayoutOptions
	{
		GUILayoutOptions()
			:width(0), height(0), minWidth(0), maxWidth(0),
			minHeight(0), maxHeight(0), fixedWidth(false), fixedHeight(false)
		{

		}

		/**
		 * @brief	Element is fixed to the specified width/height and will not resize.
		 * 			If elements doesn't fully fit in the layout it will be clipped.
		 */
		static GUILayoutOptions fixed(UINT32 _width, UINT32 _height)
		{
			GUILayoutOptions layoutOptions;
			layoutOptions.width = _width;
			layoutOptions.height = _height;
			layoutOptions.fixedWidth = true;
			layoutOptions.fixedHeight = true;

			return layoutOptions;
		}

		/**
		 * @brief	Element will resize to fit the layout in both directions.
		 * 			Optionally you may supply min and max constraints for both directions. 
		 * 			
		 *			If constraints are zero they will not be used.
		 */
		static GUILayoutOptions expandableXY(UINT32 _minWidth = 0, UINT32 _minHeight = 0, UINT32 _maxWidth = 0, UINT32 _maxHeight = 0)
		{
			GUILayoutOptions layoutOptions;
			layoutOptions.minWidth = _minWidth;
			layoutOptions.maxWidth = _maxWidth;
			layoutOptions.minHeight = _minHeight;
			layoutOptions.maxHeight = _maxHeight;
			layoutOptions.fixedWidth = false;
			layoutOptions.fixedHeight = false;

			return layoutOptions;
		}

		/**
		 * @brief	Element will resize to fit the layout in the X direction, while it will be fixed in the Y direction.
		 * 			Optionally you may supply min and max constraints for width. 
		 * 			
		 *			If constraints are zero they will not be used.
		 */
		static GUILayoutOptions expandableX(UINT32 _height, UINT32 _minWidth = 0, UINT32 _maxWidth = 0)
		{
			GUILayoutOptions layoutOptions;
			layoutOptions.height = _height;
			layoutOptions.minWidth = _minWidth;
			layoutOptions.maxWidth = _maxWidth;
			layoutOptions.fixedWidth = false;
			layoutOptions.fixedHeight = true;

			return layoutOptions;
		}

		/**
		 * @brief	Element will resize to fit the layout in the Y direction, while it will be fixed in the X direction.
		 * 			Optionally you may supply min and max constraints for height. 
		 * 			
		 *			If constraints are zero they will not be used.
		 */
		static GUILayoutOptions expandableY(UINT32 _width, UINT32 _minHeight = 0, UINT32 _maxHeight = 0)
		{
			GUILayoutOptions layoutOptions;
			layoutOptions.width = _width;
			layoutOptions.minHeight = _minHeight;
			layoutOptions.maxHeight = _maxHeight;
			layoutOptions.fixedWidth = true;
			layoutOptions.fixedHeight = false;

			return layoutOptions;
		}

		UINT32 width, height;
		UINT32 minWidth, maxWidth, minHeight, maxHeight;
		bool fixedWidth, fixedHeight;
	};
}
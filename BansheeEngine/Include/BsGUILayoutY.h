#pragma once

#include "BsPrerequisites.h"
#include "BsGUILayout.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a vertical layout that will layout out its child elements
	 *			top to bottom.
	 */
	class BS_EXPORT GUILayoutY : public GUILayout
	{
	public:
		GUILayoutY() {};
		~GUILayoutY() {};

		/**
		 * @brief	Calculate optimal sizes of all child layout elements.
		 */
		void _updateOptimalLayoutSizes();
	protected:
		/**
		 * @brief	Positions/size all child layout elements based on the provided settings and their (previously calculated) optimal sizes.
		 *
		 * @brief	x			Start X coordinate of the layout area. First element will be placed here. Relative to parent widget.
		 * @brief	y			Start Y coordinate of the layout area. First element will be placed here. Relative to parent widget.
		 * @brief	width		Maximum width of the layout in pixels. Elements will be optimized so they best fit within this width if possible.
		 * @brief	height		Maximum height of the layout in pixels. Elements will be optimized so they best fit within this height if possible.
		 * @brief	clipRect	Rectagle to clip all child elements to. Relative to parent widget. Usually equal to specified x, y, width, height parameters.
		 * @brief	widgetDepth	Depth of the parent widget. Determines depth at which child elements will be placed on. Takes priority over any other depth.
		 * @brief	areaDepth	Depth of the parent area. Determines depth at which child elements will be placed on. Takes priority over element-specific depth.
		 */
		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);
	};
}
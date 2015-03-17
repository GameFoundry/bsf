#pragma once

#include "BsPrerequisites.h"
#include "BsGUILayout.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a horizontal layout that will layout out its child elements
	 *			left to right.
	 */
	class BS_EXPORT GUILayoutX : public GUILayout
	{
	public:
		GUILayoutX(GUIArea* parentArea);
		GUILayoutX() {};
		~GUILayoutX() {};

		/**
		 * @brief	Calculate optimal sizes of all child layout elements.
		 */
		void _updateOptimalLayoutSizes();

		/**
		 * @copydoc	GUIElementBase::_calculateLayoutSizeRange
		 */
		virtual LayoutSizeRange _calculateLayoutSizeRange() const;

		/**
		 * @copydoc	GUILayout::_getElementAreas
		 */
		void _getElementAreas(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I* elementAreas, UINT32 numElements, 
			const Vector<LayoutSizeRange>& sizeRanges, const LayoutSizeRange& mySizeRange) const;

		/**
		 * @copydoc	GUILayout::_calcActualSize
		 */
		virtual Vector2I _calcActualSize(Rect2I* elementAreas, UINT32 numElements) const;
	protected:
		/**
		 * @brief	Positions/size all child layout elements based on the provided settings and their (previously calculated) optimal sizes.
		 *
		 * @brief	x			Start X coordinate of the layout area. First element will be placed here. Relative to parent widget.
		 * @brief	y			Start Y coordinate of the layout area. First element will be placed here. Relative to parent widget.
		 * @brief	width		Maximum width of the layout in pixels. Elements will be optimized so they best fit within this width if possible.
		 * @brief	height		Maximum height of the layout in pixels. Elements will be optimized so they best fit within this height if possible.
		 * @brief	clipRect	Rectangle to clip all child elements to. Relative to parent widget. Usually equal to specified x, y, width, height parameters.
		 * @brief	widgetDepth	Depth of the parent widget. Determines depth at which child elements will be placed on. Takes priority over any other depth.
		 * @brief	areaDepth	Depth of the parent area. Determines depth at which child elements will be placed on. Takes priority over element-specific depth.
		 */
		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth);
	};
}
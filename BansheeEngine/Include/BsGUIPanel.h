#pragma once

#include "BsPrerequisites.h"
#include "BsGUILayout.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a GUI panel that you can use for free placement 
	 *			of GUI elements within its bounds.
	 */
	class BS_EXPORT GUIPanel : public GUILayout
	{
	public:
		GUIPanel(INT16 depth, UINT16 depthRangeMin, UINT16 depthRangeMax, const GUIDimensions& dimensions);
		GUIPanel() {};
		~GUIPanel() {};

		/**
		 * @copydoc	GUIElementBase::_getType
		 */
		virtual Type _getType() const override { return GUIElementBase::Type::Panel; }

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
		 * @brief	Calculates the size of the provided child within this layout with the provided dimensions.
		 *
		 * @note	Internal method.
		 */
		Rect2I _getElementArea(INT32 x, INT32 y, UINT32 width, UINT32 height, const GUIElementBase* element, const LayoutSizeRange& sizeRange) const;

		/**
		 * @brief	Calculates an element size range for the provided child of the GUI panel. Will return cached bounds
		 *			so make sure to update optimal size ranges before calling.
		 *
		 * @note	Internal method.
		 */
		LayoutSizeRange _getElementSizeRange(const GUIElementBase* element) const;

		/**
		 * @brief	Assigns the specified layout information to a child element of a GUI panel.
		 *
		 * @note	Internal method.
		 */
		void _updateChildLayout(GUIElementBase* element, const Rect2I& area, const Rect2I& clipRect, UINT8 widgetDepth,
			INT16 panelDepth, UINT16 panelDepthRangeMin, UINT16 panelDepthRangeMax);

		/**
		 * @copydoc	GUILayout::_calcActualSize
		 */
		virtual Vector2I _calcActualSize(INT32 x, INT32 y, Rect2I* elementAreas, UINT32 numElements) const;

		/**
		 * @brief	Changes values that control at which depth is GUI panel and its children rendered.
		 *
		 * @param	depth			Determines rendering order of the GUI panel. Panels with lower depth will be
		 *							rendered in front of panels with higher depth. Provided depth is relative
		 *							to depth of the parent GUI panel (if any).
		 * @param	depthRangeMin	Minimum range of depths that children of this GUI panel can have. If any panel has depth
		 *							outside of the range [depth - depthRangeMin, depth + depthRangeMax] it will be clamped to
		 *							nearest extreme. Value of -1 means infinite range.
		 * @param	depthRangeMax	Maximum range of depths that children of this GUI panel can have. If any panel has depth
		 *							outside of the range [depth - depthRangeMin, depth + depthRangeMax] it will be clamped to
		 *							nearest extreme. Value of -1 means infinite range.
		 */
		void setDepthRange(INT16 depth = 0, UINT16 depthRangeMin = -1, UINT16 depthRangeMax = -1);

		/**
		 * @brief	Creates a new GUI panel.
		 *
		 * @param	depth			Determines rendering order of the GUI panel. Panels with lower depth will be
		 *							rendered in front of panels with higher depth. Provided depth is relative
		 *							to depth of the parent GUI panel (if any).
		 * @param	depthRangeMin	Minimum range of depths that children of this GUI panel can have. If any panel has depth
		 *							outside of the range [depth - depthRangeMin, depth + depthRangeMax] it will be clamped to
		 *							nearest extreme. Value of -1 means infinite range.
		 * @param	depthRangeMax	Maximum range of depths that children of this GUI panel can have. If any panel has depth
		 *							outside of the range [depth - depthRangeMin, depth + depthRangeMax] it will be clamped to
		 *							nearest extreme. Value of -1 means infinite range.
		 */
		static GUIPanel* create(INT16 depth = 0, UINT16 depthRangeMin = -1, UINT16 depthRangeMax = -1);

		/**
		 * @brief	Creates a new GUI panel.
		 *
		 * @param	depth		Determines rendering order of the GUI panel. Panels with lower depth will be
		 *						rendered in front of panels with higher depth. Provided depth is relative
		 *						to depth of the parent GUI panel (if any).
		 */
		static GUIPanel* create(const GUIOptions& options);

		/**
		 * @brief	Creates a new GUI panel.
		 *
		 * @param	depth			Determines rendering order of the GUI panel. Panels with lower depth will be
		 *							rendered in front of panels with higher depth. Provided depth is relative
		 *							to depth of the parent GUI panel (if any).
		 * @param	depthRangeMin	Minimum range of depths that children of this GUI panel can have. If any panel has depth
		 *							outside of the range [depth - depthRangeMin, depth + depthRangeMax] it will be clamped to
		 *							nearest extreme. Value of -1 means infinite range.
		 * @param	depthRangeMax	Maximum range of depths that children of this GUI panel can have. If any panel has depth
		 *							outside of the range [depth - depthRangeMin, depth + depthRangeMax] it will be clamped to
		 *							nearest extreme. Value of -1 means infinite range.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 */
		static GUIPanel* create(INT16 depth, UINT16 depthRangeMin, UINT16 depthRangeMax, const GUIOptions& options);

	protected:
		/**
		 * @brief	Positions/size all child layout elements based on the provided settings and their (previously calculated) optimal sizes.
		 *
		 * @param	x					Start X coordinate of the layout area. First element will be placed here. Relative to parent widget.
		 * @param	y					Start Y coordinate of the layout area. First element will be placed here. Relative to parent widget.
		 * @param	width				Maximum width of the layout in pixels. Elements will be optimized so they best fit within this width if possible.
		 * @param	height				Maximum height of the layout in pixels. Elements will be optimized so they best fit within this height if possible.
		 * @param	clipRect			Rectangle to clip all child elements to. Relative to parent widget. Usually equal to specified x, y, width, height parameters.
		 * @param	widgetDepth			Depth of the parent widget. Determines depth at which child elements will be placed on. Takes priority over any other depth.
		 * @param	panelDepth			Depth of the parent panel, will be set for all child elements.
		 * @param	panelDepthRangeMin  Minimum depth range that child GUI panels can have (relative to panelDepth).
		 *								Values outside of the depth range will be clamped.
		 * @param	panelDepthRangeMax	Maximum depth range that child GUI panels can have (relative to panelDepth).
		 *								Values outside of the depth range will be clamped.
		 */
		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I clipRect, UINT8 widgetDepth, 
			INT16 panelDepth, UINT16 panelDepthRangeMin, UINT16 panelDepthRangeMax);

		INT16 mDepthOffset;
		UINT16 mDepthRangeMin;
		UINT16 mDepthRangeMax;
	};
}
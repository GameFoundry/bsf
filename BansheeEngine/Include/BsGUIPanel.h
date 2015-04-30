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
		GUIPanel(UINT16 depth, UINT16 depthRange, const GUIDimensions& dimensions);
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
		 * @copydoc	GUILayout::_calcActualSize
		 */
		virtual Vector2I _calcActualSize(INT32 x, INT32 y, Rect2I* elementAreas, UINT32 numElements) const;

		/**
		 * @brief	Changes values that control at which depth is GUI panel and its children rendered.
		 *
		 * @param	depth		Determines rendering order of the GUI panel. Panels with lower depth will be
		 *						rendered in front of panels with higher depth. Provided depth is relative
		 *						to depth of the parent GUI panel (if any).
		 * @param	depthRange	Maximum range of depths that children of this GUI panel can have. If any panel has depth
		 *						outside of the range [depth, depth + depthRange] it will be clamped to nearest extreme.
		 *						Value of -1 means infinite range.
		 */
		void setDepthRange(UINT16 depth = 0, UINT16 depthRange = -1);

		/**
		 * @brief	Creates a new GUI panel.
		 *
		 * @param	depth		Determines rendering order of the GUI panel. Panels with lower depth will be
		 *						rendered in front of panels with higher depth. Provided depth is relative
		 *						to depth of the parent GUI panel (if any).
		 * @param	depthRange	Maximum range of depths that children of this GUI panel can have. If any panel has depth
		 *						outside of the range [depth, depth + depthRange] it will be clamped to nearest extreme.
		 *						Value of -1 means infinite range.
		 */
		static GUIPanel* create(UINT16 depth = 0, UINT16 depthRange = -1);

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
		 * @param	depth		Determines rendering order of the GUI panel. Panels with lower depth will be
		 *						rendered in front of panels with higher depth. Provided depth is relative
		 *						to depth of the parent GUI panel (if any).
		 * @param	depthRange	Maximum range of depths that children of this GUI panel can have. If any panel has depth
		 *						outside of the range [depth, depth + depthRange] it will be clamped to nearest extreme.
		 *						Value of -1 means infinite range.
		 * @param	options		Options that allow you to control how is the element positioned and sized.
		 */
		static GUIPanel* create(UINT16 depth, UINT16 depthRange, const GUIOptions& options);

	protected:
		/**
		 * @brief	Positions/size all child layout elements based on the provided settings and their (previously calculated) optimal sizes.
		 *
		 * @param	x			Start X coordinate of the layout area. First element will be placed here. Relative to parent widget.
		 * @param	y			Start Y coordinate of the layout area. First element will be placed here. Relative to parent widget.
		 * @param	width		Maximum width of the layout in pixels. Elements will be optimized so they best fit within this width if possible.
		 * @param	height		Maximum height of the layout in pixels. Elements will be optimized so they best fit within this height if possible.
		 * @param	clipRect	Rectangle to clip all child elements to. Relative to parent widget. Usually equal to specified x, y, width, height parameters.
		 * @param	widgetDepth	Depth of the parent widget. Determines depth at which child elements will be placed on. Takes priority over any other depth.
		 * @param	panelDepth		Depth of the parent panel, will be set for all child elements.
		 * @param	panelDepthRange	Maximum depth range that child GUI panels can have (relative to panelDepth). 
		 *							Values outside of the depth range will be clamped. Value of -1 means infinite range.
		 */
		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I clipRect, UINT8 widgetDepth, UINT16 panelDepth, UINT16 panelDepthRange);

		UINT16 mDepthOffset;
		UINT16 mDepthRange;
	};
}
#pragma once

#include "BsPrerequisites.h"
#include "BsCGUIWidget.h"

namespace BansheeEngine
{
	/**
	 * @brief	GUI widget that renders a tooltip overlaid over other GUI elements.
	 */
	class BS_EXPORT GUITooltip : public CGUIWidget
	{
	public:
		/**
		 * @brief	Name of the style used for tooltip's background frame.
		 */
		static String getFrameStyleName();

		/**
		 * @brief	Creates a new tooltip widget.
		 *
		 * @param	parent			Parent scene object to attach the tooltip to.
		 * @param	overlaidWidget	Widget over which to overlay the tooltip.
		 * @param	position		Position of the tooltip, relative to the overlaid widget position.
		 * @param	text			Text to display in the tooltip.
		 */
		GUITooltip(const HSceneObject& parent, const CGUIWidget& overlaidWidget, const Vector2I& position, const WString& text);
		~GUITooltip();

	private:
		static const UINT32 TOOLTIP_WIDTH;
		static const UINT32 CURSOR_SIZE;
	};
}
#pragma once

#include "BsPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manages displaying tooltips over GUI elements.
	 */
	class BS_EXPORT GUITooltipManager : public Module<GUITooltipManager>
	{
	public:
		~GUITooltipManager();

		/**
		 * @brief	Shows a tooltip at the specified location. This will replace any previously shown tooltip.
		 *
		 * @param	widget		GUI widget over which to display the tooltip.
		 * @param	position	Position of the tooltip, relative to the parent GUI widget.
		 * @param	text		Text to display on the tooltip.
		 */
		void show(const GUIWidget& widget, const Vector2I& position, const WString& text);

		/**
		 * @brief	Hides the currently shown tooltip.
		 */
		void hide();

	private:
		HSceneObject mTooltipSO;
	};
}
#pragma once

#include "BsPrerequisites.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains valid size range for a GUI element in a GUI layout
	 */
	struct BS_EXPORT LayoutSizeRange
	{
		Vector2I optimal;
		Vector2I min;
		Vector2I max;
	};

	/**
	 * @brief	Options that control how an element is positioned and sized in a GUI layout.
	 */
	struct BS_EXPORT GUILayoutOptions
	{
		/**
		 * @brief	Creates new default layout options.
		 */
		static GUILayoutOptions create();

		/**
		 * @brief	Creates layout options with user defined options.
		 */
		static GUILayoutOptions create(const GUIOptions& options);

		GUILayoutOptions();

		/**
		 * @brief	Updates layout options from the provided style. If user has not manually
		 *			set a specific layout property, that property will be inherited from style.
		 */
		void updateWithStyle(const GUIElementStyle* style);

		/**
		 * @brief	Calculates size range for a GUI element using this layout.
		 *
		 * @param	optimal	Preferred size of the GUI element.
		 */
		LayoutSizeRange calculateSizeRange(const Vector2I& optimal) const;

		UINT32 width, height;
		UINT32 minWidth, maxWidth, minHeight, maxHeight;
		bool fixedWidth, fixedHeight;
		bool overridenWidth, overridenHeight;
	};
}
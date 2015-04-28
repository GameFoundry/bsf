#pragma once

#include "BsPrerequisites.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains valid size range for a GUI element in a GUI layout.
	 */
	struct BS_EXPORT LayoutSizeRange
	{
		Vector2I optimal;
		Vector2I min;
		Vector2I max;
	};

	/**
	 * @brief	Flags that identify the type of data stored in a GUIDimensions structure.
	 */
	enum GUIDimensionFlags
	{
		GUIDF_FixedWidth = 0x01, 
		GUIDF_FixedHeight = 0x02, 
		GUIDF_OverWidth = 0x04, 
		GUIDF_OverHeight = 0x08
	};

	/**
	 * @brief	Options that control how an element is positioned and sized.
	 */
	struct BS_EXPORT GUIDimensions
	{
		/**
		 * @brief	Creates new default layout options.
		 */
		static GUIDimensions create();

		/**
		 * @brief	Creates layout options with user defined options.
		 */
		static GUIDimensions create(const GUIOptions& options);

		GUIDimensions();

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

		/**
		 * @brief	Checks do the dimensions override the style height.
		 */
		bool overridenHeight() const { return (flags & GUIDF_OverHeight) != 0; }

		/**
		 * @brief	Checks do the dimensions override the style width.
		 */
		bool overridenWidth() const { return (flags & GUIDF_OverWidth) != 0; }

		/**
		 * @brief	Checks do the dimensions contain fixed width.
		 */
		bool fixedWidth() const { return (flags & GUIDF_FixedWidth) != 0; }

		/**
		 * @brief	Checks do the dimensions contain fixed height.
		 */
		bool fixedHeight() const { return (flags & GUIDF_FixedHeight) != 0; }

		INT32 x, y;
		UINT32 minWidth, maxWidth, minHeight, maxHeight;
		UINT32 flags;
	};
}
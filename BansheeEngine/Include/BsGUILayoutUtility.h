#pragma once

#include "BsPrerequisites.h"
#include "BsRect2I.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/**
	 * Helper class that performs various operations related to
	 * GUILayout and GUI element sizing/placement.
	 */
	class BS_EXPORT GUILayoutUtility
	{
	public:
		/**
		 * Calculates optimal size of a GUI element.
		 */
		static Vector2I calcOptimalSize(const GUIElementBase* elem);

		/**
		 * Calculates position and size of a GUI element in its current layout.
		 * Returned position is relative to layout origin.
		 */
		// TODO - This method might fail if element is part of a more complex hierarchy
		// other than just GUILayouts and base elements (e.g. a tree view) because for a lot
		// of such custom container elements like tree view don't have method for calculating 
		// element bounds implemented
		static Rect2I calcBounds(const GUIElementBase* elem);
	};
}

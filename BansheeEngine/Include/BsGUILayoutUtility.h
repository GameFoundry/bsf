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
		 * @brief	Calculates the actual size of the layout taken up by all of its elements.
		 *			
		 * @note	Actual size means the bounds might be smaller or larger than the layout area itself.
		 *			If larger that means certain portions of the child elements will be clipped, and if
		 *			smaller certain portions of the layout area will be empty.
		 */
		static Vector2I calcActualSize(UINT32 width, UINT32 height, const GUILayout* layout);
	};
}

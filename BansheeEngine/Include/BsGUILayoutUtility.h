#pragma once

#include "BsPrerequisites.h"
#include "BsRectI.h"
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
		static RectI calcArea(const GUIElementBase* elem);
	};
}

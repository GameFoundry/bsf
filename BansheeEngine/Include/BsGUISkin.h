//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementStyle.h"

namespace BansheeEngine
{
	/**
	 * @brief	Holds information about how are GUI element types positioned and
	 *			displayed in the GUI. Each element type can be assigned a specific style.
	 */
	class BS_EXPORT GUISkin
	{
	public:
		GUISkin() {}

		/**
		 * @brief	Returns a style for the specified GUI element type.
		 *
		 * @see		GUIElement::getGUITypeName
		 */
		const GUIElementStyle* getStyle(const String& guiElemType) const;

		/**
		 * @brief	Sets a style for the specified GUI element type.
		 *
		 * @see		GUIElement::getGUITypeName
		 */
		void setStyle(const String& guiElemType, const GUIElementStyle& style);

		/**
		 * @brief	Default style that may be used when no other is available.
		 */
		static GUIElementStyle DefaultStyle;
	private:
		GUISkin(const GUISkin& skin) {} // Disable copying

		UnorderedMap<String, GUIElementStyle> mStyles;
	};
}
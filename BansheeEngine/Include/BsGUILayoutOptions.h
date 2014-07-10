//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
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

		UINT32 width, height;
		UINT32 minWidth, maxWidth, minHeight, maxHeight;
		bool fixedWidth, fixedHeight;
		bool overridenWidth, overridenHeight;
	};
}
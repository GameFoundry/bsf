#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Options that control how an element is positioned and sized in a GUI layout.
	 */
	struct BS_EXPORT GUILayoutOptions
	{
		static GUILayoutOptions create();
		static GUILayoutOptions create(const GUIOptions& options);

		GUILayoutOptions();

		void updateWithStyle(const GUIElementStyle* style);

		UINT32 width, height;
		UINT32 minWidth, maxWidth, minHeight, maxHeight;
		bool fixedWidth, fixedHeight;
		bool overridenWidth, overridenHeight;
	};
}
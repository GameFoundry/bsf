#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Options that control how an element is positioned and sized in a GUI layout.
	 */
	struct BS_EXPORT GUILayoutOptions
	{
		static GUILayoutOptions create(const GUIOptions& options, const GUIElementStyle* style);
		static GUILayoutOptions create(const GUIElementStyle* style);

		GUILayoutOptions();

		CM::UINT32 width, height;
		CM::UINT32 minWidth, maxWidth, minHeight, maxHeight;
		bool fixedWidth, fixedHeight;
	};
}
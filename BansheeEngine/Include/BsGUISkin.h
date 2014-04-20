#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementStyle.h"

namespace BansheeEngine
{
	class BS_EXPORT GUISkin
	{
	public:
		static GUIElementStyle DefaultStyle;
		
		GUISkin() {}

		const GUIElementStyle* getStyle(const String& guiElemType) const;
		void setStyle(const String& guiElemType, const GUIElementStyle& style);

	private:
		UnorderedMap<String, GUIElementStyle>::type mStyles;

		GUISkin(const GUISkin& skin) {} // Disable copying
	};
}
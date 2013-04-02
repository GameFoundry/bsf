#pragma once

#include "CmPrerequisites.h"
#include "CmGUIElementStyle.h"

namespace CamelotEngine
{
	class CM_EXPORT GUISkin
	{
	public:
		const GUIElementStyle* getStyle(const String& guiElemType) const;
		void setStyle(const String& guiElemType, const GUIElementStyle& style);

	private:
		static GUIElementStyle DefaultStyle;

		unordered_map<std::string, GUIElementStyle>::type mStyles;
	};
}
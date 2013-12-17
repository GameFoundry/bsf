#pragma once

#include "BsPrerequisites.h"
#include "BsTextSprite.h"
#include "BsGUIContent.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIHelper
	{
	public:
		static CM::Vector2I calcOptimalContentsSize(const GUIContent& content, const GUIElementStyle& style, const GUILayoutOptions& layoutOptions);
		static CM::Vector2I calcOptimalContentsSize(const CM::WString& text, const GUIElementStyle& style, const GUILayoutOptions& layoutOptions);
	};
}
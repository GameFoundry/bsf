#pragma once

#include "BsPrerequisites.h"
#include "BsTextSprite.h"
#include "BsGUIContent.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIHelper
	{
	public:
		static Vector2I calcOptimalContentsSize(const Vector2I& contentSize, const GUIElementStyle& style, const GUILayoutOptions& layoutOptions);
		static Vector2I calcOptimalContentsSize(const GUIContent& content, const GUIElementStyle& style, const GUILayoutOptions& layoutOptions);
		static Vector2I calcOptimalContentsSize(const WString& text, const GUIElementStyle& style, const GUILayoutOptions& layoutOptions);
	};
}
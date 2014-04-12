#include "BsGUIButton.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIHelper.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIButton::getGUITypeName()
	{
		static String name = "Button";
		return name;
	}

	GUIButton::GUIButton(const GUIElementStyle* style, const GUIContent& content, const GUILayoutOptions& layoutOptions)
		:GUIButtonBase(style, content, layoutOptions)
	{ }

	GUIButton* GUIButton::create(const HString& text, const GUIElementStyle* style)
	{
		return create(GUIContent(text), style);
	}

	GUIButton* GUIButton::create(const HString& text, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		return create(GUIContent(text), layoutOptions, style);
	}

	GUIButton* GUIButton::create(const GUIContent& content, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUIButton, PoolAlloc>()) GUIButton(style, content, GUILayoutOptions::create(style));
	}

	GUIButton* GUIButton::create(const GUIContent& content, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUIButton, PoolAlloc>()) GUIButton(style, content, GUILayoutOptions::create(layoutOptions, style));
	}
}
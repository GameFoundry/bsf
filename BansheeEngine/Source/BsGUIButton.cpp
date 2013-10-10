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

	GUIButton::GUIButton(GUIWidget& parent, const GUIElementStyle* style, const GUIContent& content, const GUILayoutOptions& layoutOptions)
		:GUIButtonBase(parent, style, content, layoutOptions)
	{ }

	GUIButton* GUIButton::create(GUIWidget& parent, const HString& text, const GUIElementStyle* style)
	{
		return create(parent, GUIContent(text), style);
	}

	GUIButton* GUIButton::create(GUIWidget& parent, const HString& text, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		return create(parent, GUIContent(text), layoutOptions, style);
	}

	GUIButton* GUIButton::create(GUIWidget& parent, const GUIContent& content, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIButton, PoolAlloc>()) GUIButton(parent, style, content, getDefaultLayoutOptions(style));
	}

	GUIButton* GUIButton::create(GUIWidget& parent, const GUIContent& content, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIButton, PoolAlloc>()) GUIButton(parent, style, content, layoutOptions);
	}
}
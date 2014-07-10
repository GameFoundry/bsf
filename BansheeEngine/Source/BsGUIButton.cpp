//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGUIButton.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIHelper.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	const String& GUIButton::getGUITypeName()
	{
		static String name = "Button";
		return name;
	}

	GUIButton::GUIButton(const String& styleName, const GUIContent& content, const GUILayoutOptions& layoutOptions)
		:GUIButtonBase(styleName, content, layoutOptions)
	{ }

	GUIButton* GUIButton::create(const HString& text, const String& styleName)
	{
		return create(GUIContent(text), styleName);
	}

	GUIButton* GUIButton::create(const HString& text, const GUIOptions& layoutOptions, const String& styleName)
	{
		return create(GUIContent(text), layoutOptions, styleName);
	}

	GUIButton* GUIButton::create(const GUIContent& content, const String& styleName)
	{
		return new (bs_alloc<GUIButton, PoolAlloc>()) GUIButton(getStyleName<GUIButton>(styleName), content, GUILayoutOptions::create());
	}

	GUIButton* GUIButton::create(const GUIContent& content, const GUIOptions& layoutOptions, const String& styleName)
	{
		return new (bs_alloc<GUIButton, PoolAlloc>()) GUIButton(getStyleName<GUIButton>(styleName), content, GUILayoutOptions::create(layoutOptions));
	}
}
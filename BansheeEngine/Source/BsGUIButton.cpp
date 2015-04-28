#include "BsGUIButton.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUIDimensions.h"
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

	GUIButton::GUIButton(const String& styleName, const GUIContent& content, const GUIDimensions& layoutOptions)
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
		return new (bs_alloc<GUIButton, PoolAlloc>()) GUIButton(getStyleName<GUIButton>(styleName), content, GUIDimensions::create());
	}

	GUIButton* GUIButton::create(const GUIContent& content, const GUIOptions& layoutOptions, const String& styleName)
	{
		return new (bs_alloc<GUIButton, PoolAlloc>()) GUIButton(getStyleName<GUIButton>(styleName), content, GUIDimensions::create(layoutOptions));
	}
}
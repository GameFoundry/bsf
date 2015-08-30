#include "BsGUIButton.h"
#include "BsImageSprite.h"
#include "BsCGUIWidget.h"
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

	GUIButton::GUIButton(const String& styleName, const GUIContent& content, const GUIDimensions& dimensions)
		:GUIButtonBase(styleName, content, dimensions)
	{ }

	GUIButton* GUIButton::create(const HString& text, const String& styleName)
	{
		return create(GUIContent(text), styleName);
	}

	GUIButton* GUIButton::create(const HString& text, const GUIOptions& options, const String& styleName)
	{
		return create(GUIContent(text), options, styleName);
	}

	GUIButton* GUIButton::create(const GUIContent& content, const String& styleName)
	{
		return new (bs_alloc<GUIButton>()) GUIButton(getStyleName<GUIButton>(styleName), content, GUIDimensions::create());
	}

	GUIButton* GUIButton::create(const GUIContent& content, const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUIButton>()) GUIButton(getStyleName<GUIButton>(styleName), content, GUIDimensions::create(options));
	}
}
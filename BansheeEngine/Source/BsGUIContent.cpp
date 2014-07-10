//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGUIContent.h"

namespace BansheeEngine
{
	GUIContent::GUIContent()
		:mText(L"")
	{ }

	GUIContent::GUIContent(const HString& text)
		:mText(text)
	{ }

	GUIContent::GUIContent(const HString& text, const HString& tooltip)
		:mText(text), mTooltipText(tooltip)
	{ }

	GUIContent::GUIContent(const HSpriteTexture& image)
		:mImage(image)
	{ }

	GUIContent::GUIContent(const HSpriteTexture& image, const HString& tooltip)
		:mImage(image), mTooltipText(tooltip)
	{ }

	GUIContent::GUIContent(const HString& text, const HSpriteTexture& image)
		:mText(text), mImage(image)
	{ }

	GUIContent::GUIContent(const HString& text, const HSpriteTexture& image, const HString& tooltip)
		:mText(text), mImage(image), mTooltipText(tooltip)
	{ }
}
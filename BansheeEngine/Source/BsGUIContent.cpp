#include "BsGUIContent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIContent::GUIContent()
		:mText(L"")
	{ }

	GUIContent::GUIContent(const CM::HString& text)
		:mText(text)
	{ }

	GUIContent::GUIContent(const CM::HString& text, const CM::HString& tooltip)
		:mText(text), mTooltipText(tooltip)
	{ }

	GUIContent::GUIContent(const HSpriteTexture& image)
		:mImage(image)
	{ }

	GUIContent::GUIContent(const HSpriteTexture& image, const CM::HString& tooltip)
		:mImage(image), mTooltipText(tooltip)
	{ }

	GUIContent::GUIContent(const CM::HString& text, const HSpriteTexture& image)
		:mText(text), mImage(image)
	{ }

	GUIContent::GUIContent(const CM::HString& text, const HSpriteTexture& image, const CM::HString& tooltip)
		:mText(text), mImage(image), mTooltipText(tooltip)
	{ }
}
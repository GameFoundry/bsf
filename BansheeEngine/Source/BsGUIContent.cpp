#include "BsGUIContent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIContent::GUIContent(const CM::HString& text)
		:mText(text)
	{ }

	GUIContent::GUIContent(const CM::HString& text, const CM::HString& tooltip)
		:mText(text), mTooltipText(tooltip)
	{ }

	GUIContent::GUIContent(const SpriteTexturePtr& image)
		:mImage(image)
	{ }

	GUIContent::GUIContent(const SpriteTexturePtr& image, const CM::HString& tooltip)
		:mImage(image), mTooltipText(tooltip)
	{ }

	GUIContent::GUIContent(const CM::HString& text, const SpriteTexturePtr& image)
		:mText(text), mImage(image)
	{ }

	GUIContent::GUIContent(const CM::HString& text, const SpriteTexturePtr& image, const CM::HString& tooltip)
		:mText(text), mImage(image), mTooltipText(tooltip)
	{ }
}
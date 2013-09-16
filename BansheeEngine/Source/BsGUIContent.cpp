#include "BsGUIContent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIContent::GUIContent(const CM::WString& text)
		:mText(text)
	{ }

	GUIContent::GUIContent(const CM::WString& text, const CM::WString& tooltip)
		:mText(text), mTooltipText(tooltip)
	{ }

	GUIContent::GUIContent(const SpriteTexturePtr& image)
		:mImage(image)
	{ }

	GUIContent::GUIContent(const SpriteTexturePtr& image, const CM::WString& tooltip)
		:mImage(image), mTooltipText(tooltip)
	{ }

	GUIContent::GUIContent(const CM::WString& text, const SpriteTexturePtr& image)
		:mText(text), mImage(image)
	{ }

	GUIContent::GUIContent(const CM::WString& text, const SpriteTexturePtr& image, const CM::WString& tooltip)
		:mText(text), mImage(image), mTooltipText(tooltip)
	{ }
}
#pragma once

#include "BsPrerequisites.h"
#include "BsGUIButtonBase.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "BsGUIContent.h"
#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT GUIButton : public GUIButtonBase
	{
	public:
		static const String& getGUITypeName();

		static GUIButton* create(const HString& text, const String& styleName = StringUtil::BLANK);
		static GUIButton* create(const HString& text, const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		static GUIButton* create(const GUIContent& content, const String& styleName = StringUtil::BLANK);
		static GUIButton* create(const GUIContent& content, const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		virtual ElementType getElementType() const { return ElementType::Button; }
	private:
		GUIButton(const String& styleName, const GUIContent& content, const GUILayoutOptions& layoutOptions);
	};
}
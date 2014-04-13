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
		static const CM::String& getGUITypeName();

		static GUIButton* create(const CM::HString& text, const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIButton* create(const CM::HString& text, const GUIOptions& layoutOptions, const CM::String& styleName = CM::StringUtil::BLANK);

		static GUIButton* create(const GUIContent& content, const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIButton* create(const GUIContent& content, const GUIOptions& layoutOptions, const CM::String& styleName = CM::StringUtil::BLANK);

		virtual ElementType getElementType() const { return ElementType::Button; }
	private:
		GUIButton(const CM::String& styleName, const GUIContent& content, const GUILayoutOptions& layoutOptions);
	};
}
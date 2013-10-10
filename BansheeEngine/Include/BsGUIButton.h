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

		static GUIButton* create(GUIWidget& parent, const CM::HString& text, const GUIElementStyle* style = nullptr);
		static GUIButton* create(GUIWidget& parent, const CM::HString& text, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style = nullptr);

		static GUIButton* create(GUIWidget& parent, const GUIContent& content, const GUIElementStyle* style = nullptr);
		static GUIButton* create(GUIWidget& parent, const GUIContent& content, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style = nullptr);

	private:
		GUIButton(GUIWidget& parent, const GUIElementStyle* style, const GUIContent& content, const GUILayoutOptions& layoutOptions);
	};
}
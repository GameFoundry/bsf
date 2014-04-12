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

		static GUIButton* create(const CM::HString& text, const GUIElementStyle* style = nullptr);
		static GUIButton* create(const CM::HString& text, const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);

		static GUIButton* create(const GUIContent& content, const GUIElementStyle* style = nullptr);
		static GUIButton* create(const GUIContent& content, const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);

		virtual ElementType getElementType() const { return ElementType::Button; }
	private:
		GUIButton(const GUIElementStyle* style, const GUIContent& content, const GUILayoutOptions& layoutOptions);
	};
}
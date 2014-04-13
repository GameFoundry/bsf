#pragma once

#include "BsPrerequisites.h"
#include "BsGUIButtonBase.h"
#include "BsGUIToggleGroup.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "BsGUIContent.h"
#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT GUIToggle : public GUIButtonBase
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIToggle* create(const CM::HString& text, const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIToggle* create(const CM::HString& text, const GUIOptions& layoutOptions, 
			const CM::String& styleName = CM::StringUtil::BLANK);
	
		static GUIToggle* create(const CM::HString& text, std::shared_ptr<GUIToggleGroup> toggleGroup, 
			const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIToggle* create(const CM::HString& text, std::shared_ptr<GUIToggleGroup> toggleGroup, 
			const GUIOptions& layoutOptions, const CM::String& styleName = CM::StringUtil::BLANK);

		static GUIToggle* create(const GUIContent& content, const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIToggle* create(const GUIContent& content, const GUIOptions& layoutOptions, 
			const CM::String& styleName = CM::StringUtil::BLANK);

		static GUIToggle* create(const GUIContent& content, std::shared_ptr<GUIToggleGroup> toggleGroup, 
			const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIToggle* create(const GUIContent& content, std::shared_ptr<GUIToggleGroup> toggleGroup, 
			const GUIOptions& layoutOptions, const CM::String& styleName = CM::StringUtil::BLANK);

		static std::shared_ptr<GUIToggleGroup> createToggleGroup();

		void toggleOn();
		void toggleOff();

		bool isToggled() const { return mIsToggled; }

		virtual ElementType getElementType() const { return ElementType::Toggle; }

		void _setToggleGroup(std::shared_ptr<GUIToggleGroup> toggleGroup);

		boost::signal<void(bool)> onToggled;
	protected:
		virtual ~GUIToggle();

	protected:
		GUIToggle(const CM::String& styleName, const GUIContent& content, 
			std::shared_ptr<GUIToggleGroup> toggleGroup, const GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const GUIMouseEvent& ev);

	protected:
		std::shared_ptr<GUIToggleGroup> mToggleGroup;
		bool mIsToggled;
	};
}
#include "BsGUIToggle.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIToggleGroup.h"
#include "BsGUIHelper.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIToggle::getGUITypeName()
	{
		static String name = "Toggle";
		return name;
	}

	GUIToggle::GUIToggle(const CM::String& styleName, const GUIContent& content, std::shared_ptr<GUIToggleGroup> toggleGroup, const GUILayoutOptions& layoutOptions)
		:GUIButtonBase(styleName, content, layoutOptions), mIsToggled(false), mToggleGroup(nullptr)
	{
		if(toggleGroup != nullptr)
			toggleGroup->add(this);
	}

	GUIToggle::~GUIToggle()
	{
		if(mToggleGroup != nullptr)
		{
			mToggleGroup->remove(this);
		}
	}

	GUIToggle* GUIToggle::create(const HString& text, const CM::String& styleName)
	{
		return create(GUIContent(text), styleName);
	}

	GUIToggle* GUIToggle::create(const HString& text, const GUIOptions& layoutOptions, const CM::String& styleName)
	{
		return create(GUIContent(text), layoutOptions, styleName);
	}

	GUIToggle* GUIToggle::create(const HString& text, std::shared_ptr<GUIToggleGroup> toggleGroup, const CM::String& styleName)
	{
		return create(GUIContent(text), toggleGroup, styleName);
	}

	GUIToggle* GUIToggle::create(const HString& text, std::shared_ptr<GUIToggleGroup> toggleGroup, 
		const GUIOptions& layoutOptions, const CM::String& styleName)
	{
		return create(GUIContent(text), toggleGroup, layoutOptions, styleName);
	}

	GUIToggle* GUIToggle::create(const GUIContent& content, const CM::String& styleName)
	{
		return new (cm_alloc<GUIToggle, PoolAlloc>()) GUIToggle(getStyleName<GUIToggle>(styleName), content, nullptr, GUILayoutOptions::create());
	}

	GUIToggle* GUIToggle::create(const GUIContent& content, const GUIOptions& layoutOptions, const CM::String& styleName)
	{
		return new (cm_alloc<GUIToggle, PoolAlloc>()) GUIToggle(getStyleName<GUIToggle>(styleName), content, nullptr, GUILayoutOptions::create(layoutOptions));
	}

	GUIToggle* GUIToggle::create(const GUIContent& content, std::shared_ptr<GUIToggleGroup> toggleGroup, const CM::String& styleName)
	{
		return new (cm_alloc<GUIToggle, PoolAlloc>()) GUIToggle(getStyleName<GUIToggle>(styleName), content, toggleGroup, GUILayoutOptions::create());
	}

	GUIToggle* GUIToggle::create(const GUIContent& content, std::shared_ptr<GUIToggleGroup> toggleGroup, 
		const GUIOptions& layoutOptions, const CM::String& styleName)
	{
		return new (cm_alloc<GUIToggle, PoolAlloc>()) GUIToggle(getStyleName<GUIToggle>(styleName), content, toggleGroup, GUILayoutOptions::create(layoutOptions));
	}

	std::shared_ptr<GUIToggleGroup> GUIToggle::createToggleGroup()
	{
		std::shared_ptr<GUIToggleGroup> toggleGroup = std::shared_ptr<GUIToggleGroup>(new GUIToggleGroup());
		toggleGroup->initialize(toggleGroup);

		return toggleGroup;
	}

	void GUIToggle::_setToggleGroup(std::shared_ptr<GUIToggleGroup> toggleGroup)
	{
		mToggleGroup = toggleGroup;

		bool isToggled = false;
		if(mToggleGroup != nullptr) // If in group ensure at least one element is toggled on
		{
			for(auto& toggleElem : mToggleGroup->mButtons)
			{
				if(isToggled)
				{
					if(toggleElem->mIsToggled)
						toggleElem->toggleOff();
				}
				else
				{
					if(toggleElem->mIsToggled)
						isToggled = true;
				}

			}

			if(!isToggled)
				toggleOn();
		}
	}

	void GUIToggle::toggleOn()
	{
		if(mIsToggled)
			return;

		mIsToggled = true;

		if(!onToggled.empty())
			onToggled(mIsToggled);

		if(mToggleGroup != nullptr)
		{
			for(auto& toggleElem : mToggleGroup->mButtons)
			{
				if(toggleElem != this)
					toggleElem->toggleOff();
			}
		}

		_setOn(true);
	}

	void GUIToggle::toggleOff()
	{
		if(!mIsToggled)
			return;

		bool canBeToggledOff = false;
		if(mToggleGroup != nullptr) // If in group ensure at least one element is toggled on
		{
			for(auto& toggleElem : mToggleGroup->mButtons)
			{
				if(toggleElem != this)
				{
					if(toggleElem->mIsToggled)
					{
						canBeToggledOff = true;
						break;
					}
				}

			}
		}
		else
			canBeToggledOff = true;

		if(canBeToggledOff)
		{
			mIsToggled = false;

			if(!onToggled.empty())
				onToggled(mIsToggled);

			_setOn(false);
		}
	}

	bool GUIToggle::mouseEvent(const GUIMouseEvent& ev)
	{
		bool processed = GUIButtonBase::mouseEvent(ev);

		if(ev.getType() == GUIMouseEventType::MouseUp)
		{
			if(mIsToggled)
				toggleOff();
			else
				toggleOn();

			processed = true;
		}

		return processed;
	}
}
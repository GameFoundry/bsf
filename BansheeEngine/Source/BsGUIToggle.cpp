#include "BsGUIToggle.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUIDimensions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIToggleGroup.h"
#include "BsGUIHelper.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	const String& GUIToggle::getGUITypeName()
	{
		static String name = "Toggle";
		return name;
	}

	GUIToggle::GUIToggle(const String& styleName, const GUIContent& content, std::shared_ptr<GUIToggleGroup> toggleGroup, const GUIDimensions& dimensions)
		:GUIButtonBase(styleName, content, dimensions), mIsToggled(false), mToggleGroup(nullptr)
	{
		if(toggleGroup != nullptr)
			toggleGroup->_add(this);
	}

	GUIToggle::~GUIToggle()
	{
		if(mToggleGroup != nullptr)
		{
			mToggleGroup->_remove(this);
		}
	}

	GUIToggle* GUIToggle::create(const HString& text, const String& styleName)
	{
		return create(GUIContent(text), styleName);
	}

	GUIToggle* GUIToggle::create(const HString& text, const GUIOptions& options, const String& styleName)
	{
		return create(GUIContent(text), options, styleName);
	}

	GUIToggle* GUIToggle::create(const HString& text, std::shared_ptr<GUIToggleGroup> toggleGroup, const String& styleName)
	{
		return create(GUIContent(text), toggleGroup, styleName);
	}

	GUIToggle* GUIToggle::create(const HString& text, std::shared_ptr<GUIToggleGroup> toggleGroup, 
		const GUIOptions& options, const String& styleName)
	{
		return create(GUIContent(text), toggleGroup, options, styleName);
	}

	GUIToggle* GUIToggle::create(const GUIContent& content, const String& styleName)
	{
		return new (bs_alloc<GUIToggle>()) GUIToggle(getStyleName<GUIToggle>(styleName), content, nullptr, GUIDimensions::create());
	}

	GUIToggle* GUIToggle::create(const GUIContent& content, const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUIToggle>()) GUIToggle(getStyleName<GUIToggle>(styleName), content, nullptr, GUIDimensions::create(options));
	}

	GUIToggle* GUIToggle::create(const GUIContent& content, std::shared_ptr<GUIToggleGroup> toggleGroup, const String& styleName)
	{
		return new (bs_alloc<GUIToggle>()) GUIToggle(getStyleName<GUIToggle>(styleName), content, toggleGroup, GUIDimensions::create());
	}

	GUIToggle* GUIToggle::create(const GUIContent& content, std::shared_ptr<GUIToggleGroup> toggleGroup, 
		const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUIToggle>()) GUIToggle(getStyleName<GUIToggle>(styleName), content, toggleGroup, GUIDimensions::create(options));
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

	bool GUIToggle::_mouseEvent(const GUIMouseEvent& ev)
	{
		bool processed = GUIButtonBase::_mouseEvent(ev);

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
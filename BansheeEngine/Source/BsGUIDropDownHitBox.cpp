#include "BsGUIDropDownHitBox.h"
#include "BsGUICommandEvent.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const CM::String& GUIDropDownHitBox::getGUITypeName()
	{
		static String name = "DropDownHitBox";
		return name;
	}

	GUIDropDownHitBox* GUIDropDownHitBox::create(GUIWidget& parent, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIDropDownHitBox, PoolAlloc>()) GUIDropDownHitBox(parent, style, GUILayoutOptions::create(style));
	}

	GUIDropDownHitBox* GUIDropDownHitBox::create(GUIWidget& parent, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIDropDownHitBox, PoolAlloc>()) GUIDropDownHitBox(parent, style, GUILayoutOptions::create(layoutOptions, style));
	}

	GUIDropDownHitBox::GUIDropDownHitBox(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions)
	{

	}

	bool GUIDropDownHitBox::commandEvent(const GUICommandEvent& ev)
	{
		bool processed = GUIElementContainer::commandEvent(ev);

		if(ev.getType() == GUICommandEventType::FocusGained)
		{
			if(!onFocusGained.empty())
				onFocusGained();

			return true;
		}
		else if(ev.getType() == GUICommandEventType::FocusLost)
		{
			if(!onFocusLost.empty())
				onFocusLost();

			return true;
		}

		return processed;
	}

	bool GUIDropDownHitBox::_isInBounds(const CM::Vector2I position) const
	{
		for(auto& bound : mBounds)
		{
			if(bound.contains(position))
				return true;
		}

		return false;
	}
};
#include "BsGUIDropDownHitBox.h"
#include "BsGUICommandEvent.h"
#include "BsGUIMouseEvent.h"
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

	GUIDropDownHitBox* GUIDropDownHitBox::create(GUIWidget& parent, bool captureMouse, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIDropDownHitBox, PoolAlloc>()) GUIDropDownHitBox(parent, captureMouse, style, GUILayoutOptions::create(style));
	}

	GUIDropDownHitBox* GUIDropDownHitBox::create(GUIWidget& parent, bool captureMouse, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIDropDownHitBox, PoolAlloc>()) GUIDropDownHitBox(parent, captureMouse, style, GUILayoutOptions::create(layoutOptions, style));
	}

	GUIDropDownHitBox::GUIDropDownHitBox(GUIWidget& parent, bool captureMouse, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mCaptureMouse(captureMouse)
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

	bool GUIDropDownHitBox::mouseEvent(const GUIMouseEvent& ev)
	{
		bool processed = GUIElementContainer::mouseEvent(ev);

		if(mCaptureMouse)
		{
			if(ev.getType() == GUIMouseEventType::MouseUp)
			{
				return true;
			}
			else if(ev.getType() == GUIMouseEventType::MouseDown)
			{
				return true;
			}
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
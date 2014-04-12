#include "BsGUITabButton.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUITabbedTitleBar.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const CM::UINT32 GUITabButton::DRAG_MIN_DISTANCE = 3;

	const String& GUITabButton::getGUITypeName()
	{
		static String name = "TabButton";
		return name;
	}

	GUITabButton::GUITabButton(const GUIElementStyle* style, const GUIToggleGroupPtr& toggleGroup, CM::UINT32 index, const GUIContent& content, const GUILayoutOptions& layoutOptions)
		:GUIToggle(style, content, toggleGroup, layoutOptions), mIndex(index), mDraggedState(false)
	{

	}

	GUITabButton* GUITabButton::create(const GUIToggleGroupPtr& toggleGroup, CM::UINT32 index, const HString& text, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUITabButton, PoolAlloc>()) GUITabButton(style, toggleGroup, index, GUIContent(text), GUILayoutOptions::create(style));
	}

	GUITabButton* GUITabButton::create(const GUIToggleGroupPtr& toggleGroup, CM::UINT32 index, const HString& text, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUITabButton, PoolAlloc>()) GUITabButton(style, toggleGroup, index, GUIContent(text), GUILayoutOptions::create(layoutOptions, style));
	}

	GUITabButton* GUITabButton::create(const GUIToggleGroupPtr& toggleGroup, CM::UINT32 index, const GUIContent& content, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUITabButton, PoolAlloc>()) GUITabButton(style, toggleGroup, index, content, GUILayoutOptions::create(style));
	}

	GUITabButton* GUITabButton::create(const GUIToggleGroupPtr& toggleGroup, CM::UINT32 index, const GUIContent& content, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUITabButton, PoolAlloc>()) GUITabButton(style, toggleGroup, index, content, GUILayoutOptions::create(layoutOptions, style));
	}

	void GUITabButton::_setDraggedState(bool active) 
	{
		if(mDraggedState == active)
			return;

		mDraggedState = active; 

		if(mDraggedState)
		{
			mInactiveState = getState();

			if(mInactiveState != GUIButtonState::Normal)
				setState(GUIButtonState::Normal);
		}
		else
		{
			if(getState() != mInactiveState)
				setState(mInactiveState);
		}
	}

	bool GUITabButton::mouseEvent(const GUIMouseEvent& ev)
	{	
		if(ev.getType() == GUIMouseEventType::MouseOver)
		{
			GUIButtonState state = _isOn() ? GUIButtonState::HoverOn : GUIButtonState::Hover;

			if(!mDraggedState)
			{
				setState(state);

				if(!onHover.empty())
					onHover();
			}
			else
				mInactiveState = state;

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseOut)
		{
			GUIButtonState state = _isOn() ? GUIButtonState::NormalOn : GUIButtonState::Normal;

			if(!mDraggedState)
			{
				setState(state);

				if(!onOut.empty())
					onOut();
			}
			else
				mInactiveState = state;

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDown)
		{
			if(!mDraggedState)
				setState(_isOn() ? GUIButtonState::ActiveOn : GUIButtonState::Active);

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseUp)
		{
			if(!mDraggedState)
			{
				setState(_isOn() ? GUIButtonState::HoverOn : GUIButtonState::Hover);

				if(!onClick.empty())
					onClick();

				if(!mIsToggled)
					toggleOn();
			}

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDragStart)
		{
			mDragStartPosition = ev.getPosition();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDrag)
		{
			UINT32 dist = mDragStartPosition.manhattanDist(ev.getPosition());

			if(dist > DRAG_MIN_DISTANCE)
			{
				if(!onDragged.empty())
					onDragged(mIndex, ev.getPosition());
			}

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDragEnd)
		{
			UINT32 dist = mDragStartPosition.manhattanDist(ev.getPosition());

			if(dist > DRAG_MIN_DISTANCE)
			{
				if(!onDragEnd.empty())
					onDragEnd(mIndex, ev.getPosition());
			}

			return true;
		}

		return false;
	}
}
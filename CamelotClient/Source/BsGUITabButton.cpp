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

	GUITabButton::GUITabButton(GUIWidget& parent, const GUIElementStyle* style, const GUIToggleGroupPtr& toggleGroup, CM::UINT32 index, const GUIContent& content, const GUILayoutOptions& layoutOptions)
		:GUIToggle(parent, style, content, toggleGroup, layoutOptions), mIndex(index)
	{

	}

	GUITabButton* GUITabButton::create(GUIWidget& parent, const GUIToggleGroupPtr& toggleGroup, CM::UINT32 index, const HString& text, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUITabButton, PoolAlloc>()) GUITabButton(parent, style, toggleGroup, index, GUIContent(text), GUILayoutOptions::create(style));
	}

	GUITabButton* GUITabButton::create(GUIWidget& parent, const GUIToggleGroupPtr& toggleGroup, CM::UINT32 index, const HString& text, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUITabButton, PoolAlloc>()) GUITabButton(parent, style, toggleGroup, index, GUIContent(text), GUILayoutOptions::create(layoutOptions, style));
	}

	GUITabButton* GUITabButton::create(GUIWidget& parent, const GUIToggleGroupPtr& toggleGroup, CM::UINT32 index, const GUIContent& content, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUITabButton, PoolAlloc>()) GUITabButton(parent, style, toggleGroup, index, content, GUILayoutOptions::create(style));
	}

	GUITabButton* GUITabButton::create(GUIWidget& parent, const GUIToggleGroupPtr& toggleGroup, CM::UINT32 index, const GUIContent& content, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUITabButton, PoolAlloc>()) GUITabButton(parent, style, toggleGroup, index, content, GUILayoutOptions::create(layoutOptions, style));
	}

	bool GUITabButton::mouseEvent(const GUIMouseEvent& ev)
	{
		bool eventProcessed = GUIButtonBase::mouseEvent(ev);

		if(ev.getType() == GUIMouseEventType::MouseUp)
		{
			if(!mIsToggled)
				toggleOn();

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

		return eventProcessed;
	}
}
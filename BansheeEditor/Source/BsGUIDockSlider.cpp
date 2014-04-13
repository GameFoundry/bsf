#include "BsGUIDockSlider.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUITabbedTitleBar.h"
#include "BsCursor.h"
#include "CmPlatform.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const String& GUIDockSlider::getGUITypeName()
	{
		static String name = "DockSlider";
		return name;
	}

	GUIDockSlider::GUIDockSlider(bool horizontal, const CM::String& styleName, const BS::GUILayoutOptions& layoutOptions)
		:GUIButtonBase(styleName, GUIContent(HString(L"")), layoutOptions),
		mDragInProgress(false), mHorizontal(horizontal), mIsCursorSet(false)
	{

	}

	GUIDockSlider* GUIDockSlider::create(bool horizontal, const CM::String& styleName)
	{
		return new (cm_alloc<GUIDockSlider, PoolAlloc>()) GUIDockSlider(horizontal, 
			getStyleName<GUIDockSlider>(styleName), GUILayoutOptions::create());
	}

	GUIDockSlider* GUIDockSlider::create(bool horizontal, const GUIOptions& layoutOptions, const CM::String& styleName)
	{
		return new (cm_alloc<GUIDockSlider, PoolAlloc>()) GUIDockSlider(horizontal, 
			getStyleName<GUIDockSlider>(styleName), GUILayoutOptions::create(layoutOptions));
	}

	bool GUIDockSlider::_hasCustomCursor(const CM::Vector2I position, CursorType& type) const
	{
		if(_isInBounds(position))
		{
			type = mHorizontal ? CursorType::SizeNS : CursorType::SizeWE;
			return true;
		}

		return false;
	}

	bool GUIDockSlider::mouseEvent(const GUIMouseEvent& ev)
	{	
		bool processed = GUIButtonBase::mouseEvent(ev);

		if(ev.getType() == GUIMouseEventType::MouseDragStart)
		{
			mLastDragPosition = ev.getPosition();
			mDragInProgress = true;

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDrag)
		{
			Vector2I delta = ev.getPosition() - mLastDragPosition;
			mLastDragPosition = ev.getPosition();

			if(!onDragged.empty())
				onDragged(delta);

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDragEnd)
		{
			mDragInProgress = false;

			return true;
		}

		return processed;
	}
}
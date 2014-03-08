#include "BsGUIDockSlider.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUITabbedTitleBar.h"
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

	GUIDockSlider::GUIDockSlider(BS::GUIWidget& parent, bool horizontal, const BS::GUIElementStyle* style, const BS::GUILayoutOptions& layoutOptions)
		:GUIButtonBase(parent, style, GUIContent(HString(L"")), layoutOptions),
		mIsMouseOver(false), mDragInProgress(false), mHorizontal(horizontal), mIsCursorSet(false)
	{

	}

	GUIDockSlider* GUIDockSlider::create(GUIWidget& parent, bool horizontal, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIDockSlider, PoolAlloc>()) GUIDockSlider(parent, horizontal, style, GUILayoutOptions::create(style));
	}

	GUIDockSlider* GUIDockSlider::create(GUIWidget& parent, bool horizontal, const GUIOptions& layoutOptions, const BS::GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIDockSlider, PoolAlloc>()) GUIDockSlider(parent, horizontal, style, GUILayoutOptions::create(layoutOptions, style));
	}

	bool GUIDockSlider::mouseEvent(const GUIMouseEvent& ev)
	{	
		bool processed = GUIButtonBase::mouseEvent(ev);

		if(ev.getType() == GUIMouseEventType::MouseOver)
		{
			mIsMouseOver = true;

			if(!mIsCursorSet)
			{
				Platform::setCursor(mHorizontal ? CursorType::SizeNS : CursorType::SizeWE);
				mIsCursorSet = true;
			}

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseOut)
		{
			mIsMouseOver = false;

			if(!mDragInProgress && mIsCursorSet)
			{
				Platform::setCursor(CursorType::Arrow);
				mIsCursorSet = false;
			}

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDragStart)
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

			if(mIsCursorSet && !mIsMouseOver)
			{
				Platform::setCursor(CursorType::Arrow);
				mIsCursorSet = false;
			}

			return true;
		}

		return processed;
	}
}
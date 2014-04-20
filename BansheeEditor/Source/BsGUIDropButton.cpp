#include "BsGUIDropButton.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIHelper.h"
#include "CmTexture.h"
#include "BsDragAndDropManager.h"

namespace BansheeEngine
{
	const String& GUIDropButton::getGUITypeName()
	{
		static String name = "DropButton";
		return name;
	}

	GUIDropButton::GUIDropButton(UINT32 dragType, const String& styleName, const GUILayoutOptions& layoutOptions)
		:GUIButtonBase(styleName, GUIContent(HString(L"None")), layoutOptions)
	{

	}

	GUIDropButton::~GUIDropButton()
	{ }

	GUIDropButton* GUIDropButton::create(UINT32 dragType, const String& styleName)
	{
		return new (cm_alloc<GUIDropButton, PoolAlloc>()) GUIDropButton(dragType, 
			getStyleName<GUIDropButton>(styleName), GUILayoutOptions::create());
	}

	GUIDropButton* GUIDropButton::create(UINT32 dragType, const GUIOptions& layoutOptions, const String& styleName)
	{
		return new (cm_alloc<GUIDropButton, PoolAlloc>()) GUIDropButton(dragType, 
			getStyleName<GUIDropButton>(styleName), GUILayoutOptions::create(layoutOptions));
	}

	bool GUIDropButton::mouseEvent(const GUIMouseEvent& ev)
	{
		bool processed = GUIButtonBase::mouseEvent(ev);

		if(ev.getType() == GUIMouseEventType::MouseDragAndDropDragged)
		{
			if(DragAndDropManager::instance().isDragInProgress())
			{
				if(DragAndDropManager::instance().getDragTypeId() == mDragType)
				{
					if(!_isOn())
						_setOn(true);
				}
				else
				{
					if(_isOn())
						_setOn(false);
				}
			}
			else
			{
				if(_isOn())
					_setOn(false);
			}

			processed = true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDragAndDropDropped)
		{
			if(_isOn())
				_setOn(false);

			if(DragAndDropManager::instance().isDragInProgress() && DragAndDropManager::instance().getDragTypeId() == mDragType)
			{
				if(!onDataDropped.empty())
					onDataDropped(DragAndDropManager::instance().getDragData());
			}
		}

		return processed;
	}
}
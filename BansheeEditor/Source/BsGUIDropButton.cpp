#include "BsGUIDropButton.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUIDimensions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIHelper.h"
#include "BsTexture.h"
#include "BsDragAndDropManager.h"

namespace BansheeEngine
{
	const String& GUIDropButton::getGUITypeName()
	{
		static String name = "DropButton";
		return name;
	}

	GUIDropButton::GUIDropButton(UINT32 dragType, const String& styleName, const GUIDimensions& dimensions)
		:GUIButtonBase(styleName, GUIContent(HString(L"None")), dimensions), mDragType(dragType)
	{

	}

	GUIDropButton::~GUIDropButton()
	{ }

	GUIDropButton* GUIDropButton::create(UINT32 dragType, const String& styleName)
	{
		return new (bs_alloc<GUIDropButton, PoolAlloc>()) GUIDropButton(dragType, 
			getStyleName<GUIDropButton>(styleName), GUIDimensions::create());
	}

	GUIDropButton* GUIDropButton::create(UINT32 dragType, const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUIDropButton, PoolAlloc>()) GUIDropButton(dragType, 
			getStyleName<GUIDropButton>(styleName), GUIDimensions::create(options));
	}

	bool GUIDropButton::_mouseEvent(const GUIMouseEvent& ev)
	{
		bool processed = GUIButtonBase::_mouseEvent(ev);

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

	bool GUIDropButton::_acceptDragAndDrop(const Vector2I position, UINT32 typeId) const
	{
		return typeId == mDragType;
	}
}
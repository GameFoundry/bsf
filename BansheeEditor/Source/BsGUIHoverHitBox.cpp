#include "BsGUIHoverHitBox.h"
#include "BsGUICommandEvent.h"
#include "BsGUIMouseEvent.h"
#include "BsCGUIWidget.h"
#include "BsGUISkin.h"

namespace BansheeEngine
{
	const String& GUIHoverHitBox::getGUITypeName()
	{
		static String name = "HoverHitBox";
		return name;
	}

	GUIHoverHitBox* GUIHoverHitBox::create()
	{
		return new (bs_alloc<GUIHoverHitBox>()) GUIHoverHitBox(GUIDimensions::create());
	}

	GUIHoverHitBox* GUIHoverHitBox::create(const GUIOptions& options)
	{
		return new (bs_alloc<GUIHoverHitBox>()) GUIHoverHitBox(GUIDimensions::create(options));
	}

	GUIHoverHitBox::GUIHoverHitBox(const GUIDimensions& dimensions)
		:GUIElementContainer(dimensions)
	{

	}

	void GUIHoverHitBox::updateClippedBounds()
	{
		mClippedBounds = mLayoutData.area;
	}

	bool GUIHoverHitBox::_mouseEvent(const GUIMouseEvent& ev)
	{
		bool processed = GUIElementContainer::_mouseEvent(ev);

		if (ev.getType() == GUIMouseEventType::MouseOver)
		{
			onHover();
			return false;
		}
		else if (ev.getType() == GUIMouseEventType::MouseOut)
		{
			onOut();
			return false;
		}

		return processed;
	}
};
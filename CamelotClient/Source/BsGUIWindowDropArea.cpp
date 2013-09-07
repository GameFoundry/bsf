#include "BsGUIWindowDropArea.h"
#include "CmApplication.h"
#include "CmTexture.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const String& GUIWindowDropArea::getGUITypeName()
	{
		static String name = "WindowDropArea";
		return name;
	}

	GUIWindowDropArea::GUIWindowDropArea(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUITexture(parent, style, nullptr, GUIImageScaleMode::ScaleToFit, layoutOptions)
	{ }

	GUIWindowDropArea::~GUIWindowDropArea()
	{ }

	GUIWindowDropArea* GUIWindowDropArea::create(GUIWidget& parent, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIWindowDropArea, PoolAlloc>()) GUIWindowDropArea(parent, style, getDefaultLayoutOptions(style));
	}

	GUIWindowDropArea* GUIWindowDropArea::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIWindowDropArea, PoolAlloc>()) GUIWindowDropArea(parent, style, layoutOptions);
	}

	void GUIWindowDropArea::setFocused(bool focused)
	{
		if(focused)
			mDesc.texture = mStyle->focused.texture;
		else
			mDesc.texture = mStyle->normal.texture;

		markContentAsDirty();
	}

	bool GUIWindowDropArea::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseDragAndDropDropped)
		{
			if(!onDraggedItemDropped.empty())
				onDraggedItemDropped();

			return true;
		}

		return false;
	}
}
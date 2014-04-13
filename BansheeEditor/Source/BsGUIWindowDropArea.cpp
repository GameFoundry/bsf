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

	GUIWindowDropArea::GUIWindowDropArea(const CM::String& styleName, const GUILayoutOptions& layoutOptions)
		:GUITexture(styleName, HSpriteTexture(), GUIImageScaleMode::ScaleToFit, layoutOptions)
	{ }

	GUIWindowDropArea::~GUIWindowDropArea()
	{ }

	GUIWindowDropArea* GUIWindowDropArea::create(const CM::String& styleName)
	{
		return new (cm_alloc<GUIWindowDropArea, PoolAlloc>()) 
			GUIWindowDropArea(getStyleName<GUIWindowDropArea>(styleName), GUILayoutOptions::create());
	}

	GUIWindowDropArea* GUIWindowDropArea::create(const GUIOptions& layoutOptions, const CM::String& styleName)
	{
		return new (cm_alloc<GUIWindowDropArea, PoolAlloc>()) 
			GUIWindowDropArea(getStyleName<GUIWindowDropArea>(styleName), GUILayoutOptions::create(layoutOptions));
	}

	void GUIWindowDropArea::setFocused(bool focused)
	{
		if(focused)
			mActiveTexture = _getStyle()->focused.texture;
		else
			mActiveTexture = _getStyle()->normal.texture;

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
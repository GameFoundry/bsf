#include "BsGUIWindowFrame.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "CmApplication.h"
#include "CmPlatform.h"
#include "CmTexture.h"
#include "CmRenderWindow.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const String& GUIWindowFrame::getGUITypeName()
	{
		static String name = "WindowFrame";
		return name;
	}

	GUIWindowFrame::GUIWindowFrame(const CM::String& styleName, const GUILayoutOptions& layoutOptions)
		:GUITexture(styleName, HSpriteTexture(), GUIImageScaleMode::StretchToFit, layoutOptions)
	{

	}

	GUIWindowFrame::~GUIWindowFrame()
	{ }

	GUIWindowFrame* GUIWindowFrame::create(const CM::String& styleName)
	{
		return new (cm_alloc<GUIWindowFrame, PoolAlloc>()) GUIWindowFrame(getStyleName<GUIWindowFrame>(styleName), GUILayoutOptions::create());
	}

	GUIWindowFrame* GUIWindowFrame::create(const GUIOptions& layoutOptions, const CM::String& styleName)
	{
		return new (cm_alloc<GUIWindowFrame, PoolAlloc>()) GUIWindowFrame(getStyleName<GUIWindowFrame>(styleName), GUILayoutOptions::create(layoutOptions));
	}

	void GUIWindowFrame::setFocused(bool focused)
	{
		if(focused)
			mActiveTexture = _getStyle()->focused.texture;
		else
			mActiveTexture = _getStyle()->normal.texture;

		markContentAsDirty();
	}
}
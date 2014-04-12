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

	GUIWindowFrame::GUIWindowFrame(const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUITexture(style, HSpriteTexture(), GUIImageScaleMode::StretchToFit, layoutOptions)
	{

	}

	GUIWindowFrame::~GUIWindowFrame()
	{ }

	GUIWindowFrame* GUIWindowFrame::create(const GUIElementStyle* style)
	{
		return new (cm_alloc<GUIWindowFrame, PoolAlloc>()) GUIWindowFrame(style, GUILayoutOptions::create(style));
	}

	GUIWindowFrame* GUIWindowFrame::create(const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUIWindowFrame, PoolAlloc>()) GUIWindowFrame(style, GUILayoutOptions::create(layoutOptions, style));
	}

	void GUIWindowFrame::setFocused(bool focused)
	{
		if(focused)
			mActiveTexture = mStyle->focused.texture;
		else
			mActiveTexture = mStyle->normal.texture;

		markContentAsDirty();
	}
}
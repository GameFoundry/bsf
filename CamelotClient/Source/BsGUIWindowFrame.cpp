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

	GUIWindowFrame::GUIWindowFrame(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUITexture(parent, style, nullptr, GUIImageScaleMode::StretchToFit, layoutOptions)
	{

	}

	GUIWindowFrame::~GUIWindowFrame()
	{ }

	GUIWindowFrame* GUIWindowFrame::create(GUIWidget& parent, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIWindowFrame, PoolAlloc>()) GUIWindowFrame(parent, style, getDefaultLayoutOptions(style));
	}

	GUIWindowFrame* GUIWindowFrame::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIWindowFrame, PoolAlloc>()) GUIWindowFrame(parent, style, layoutOptions);
	}

	void GUIWindowFrame::setFocused(bool focused)
	{
		if(focused)
			mDesc.texture = mStyle->focused.texture;
		else
			mDesc.texture = mStyle->normal.texture;

		markContentAsDirty();
	}
}
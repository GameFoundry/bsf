#include "BsGUIWindowFrame.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "BsCoreApplication.h"
#include "BsPlatform.h"
#include "BsTexture.h"
#include "BsRenderWindow.h"

namespace BansheeEngine
{
	const String& GUIWindowFrame::getGUITypeName()
	{
		static String name = "WindowFrame";
		return name;
	}

	GUIWindowFrame::GUIWindowFrame(const String& styleName, const GUILayoutOptions& layoutOptions)
		:GUITexture(styleName, HSpriteTexture(), GUIImageScaleMode::StretchToFit, layoutOptions)
	{

	}

	GUIWindowFrame::~GUIWindowFrame()
	{ }

	GUIWindowFrame* GUIWindowFrame::create(const String& styleName)
	{
		return new (bs_alloc<GUIWindowFrame, PoolAlloc>()) GUIWindowFrame(getStyleName<GUIWindowFrame>(styleName), GUILayoutOptions::create());
	}

	GUIWindowFrame* GUIWindowFrame::create(const GUIOptions& layoutOptions, const String& styleName)
	{
		return new (bs_alloc<GUIWindowFrame, PoolAlloc>()) GUIWindowFrame(getStyleName<GUIWindowFrame>(styleName), GUILayoutOptions::create(layoutOptions));
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
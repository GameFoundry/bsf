#include "BsGUIWindowFrame.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUIDimensions.h"
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

	GUIWindowFrame::GUIWindowFrame(const String& styleName, const GUIDimensions& dimensions)
		:GUITexture(styleName, HSpriteTexture(), GUIImageScaleMode::StretchToFit, true, dimensions)
	{

	}

	GUIWindowFrame::~GUIWindowFrame()
	{ }

	GUIWindowFrame* GUIWindowFrame::create(const String& styleName)
	{
		return new (bs_alloc<GUIWindowFrame>()) GUIWindowFrame(getStyleName<GUIWindowFrame>(styleName), GUIDimensions::create());
	}

	GUIWindowFrame* GUIWindowFrame::create(const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUIWindowFrame>()) GUIWindowFrame(getStyleName<GUIWindowFrame>(styleName), GUIDimensions::create(options));
	}

	void GUIWindowFrame::setFocused(bool focused)
	{
		Vector2I origSize = mDimensions.calculateSizeRange(_getOptimalSize()).optimal;

		if(focused)
			mActiveTexture = _getStyle()->focused.texture;
		else
			mActiveTexture = _getStyle()->normal.texture;

		Vector2I newSize = mDimensions.calculateSizeRange(_getOptimalSize()).optimal;

		if (origSize != newSize)
			_markLayoutAsDirty();
		else
			_markContentAsDirty();
	}
}
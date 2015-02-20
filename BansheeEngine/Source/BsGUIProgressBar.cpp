#include "BsGUIProgressBar.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUITexture.h"
#include "BsSpriteTexture.h"
#include "BsGUILayoutOptions.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	GUIProgressBar::GUIProgressBar(const String& styleName, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions, styleName), mPercent(0)
	{
		mBar = GUITexture::create(getSubStyleName(getBarStyleType()));
		mBackground = GUITexture::create(getSubStyleName(getBackgroundStyleType()));

		mBackground->_setElementDepth(mBar->_getRenderElementDepthRange());

		_registerChildElement(mBar);
		_registerChildElement(mBackground);
	}

	const String& GUIProgressBar::getBarStyleType()
	{
		static String HANDLE_STYLE_TYPE = "ProgressBarFill";
		return HANDLE_STYLE_TYPE;
	}

	const String& GUIProgressBar::getBackgroundStyleType()
	{
		static String BACKGROUND_STYLE_TYPE = "ProgressBarBackground";
		return BACKGROUND_STYLE_TYPE;
	}

	Vector2I GUIProgressBar::_getOptimalSize() const
	{
		Vector2I optimalSize = mBar->_getOptimalSize();

		Vector2I backgroundSize = mBackground->_getOptimalSize();
		optimalSize.x = std::max(optimalSize.x, backgroundSize.x);
		optimalSize.y = std::max(optimalSize.y, backgroundSize.y);

		return optimalSize;
	}

	void GUIProgressBar::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		Vector2I bgOffset(x, y);
		Rect2I bgClipRect(clipRect.x - bgOffset.x, clipRect.y - bgOffset.y, clipRect.width, clipRect.height);

		mBackground->setOffset(bgOffset);
		mBackground->setWidth(width);
		mBackground->setHeight(height);
		mBackground->_setAreaDepth(areaDepth);
		mBackground->_setWidgetDepth(widgetDepth);
		mBackground->_setClipRect(bgClipRect);

		const GUIElementStyle* style = _getStyle();
		
		Vector2I barOffset(x + style->margins.left, y + style->margins.top);
		Rect2I barClipRect(clipRect.x - barOffset.x, clipRect.y - barOffset.y, clipRect.width, clipRect.height);

		UINT32 maxProgressBarWidth = std::max((UINT32)0, (UINT32)(width - style->margins.left - style->margins.right));
		UINT32 progressBarHeight = std::max((UINT32)0, (UINT32)(height - style->margins.top - style->margins.bottom)); 

		mBar->setOffset(barOffset);
		mBar->setWidth((UINT32)Math::floorToInt(maxProgressBarWidth * mPercent));
		mBar->setHeight(progressBarHeight);
		mBar->_setAreaDepth(areaDepth);
		mBar->_setWidgetDepth(widgetDepth);
		mBar->_setClipRect(barClipRect);
	}

	void GUIProgressBar::styleUpdated()
	{
		mBar->setStyle(getSubStyleName(getBarStyleType()));
		mBackground->setStyle(getSubStyleName(getBackgroundStyleType()));
	}

	void GUIProgressBar::setPercent(float pct)
	{
		mPercent = pct;
		markContentAsDirty();
	}

	void GUIProgressBar::setTint(const Color& color)
	{
		mBar->setTint(color);
		mBackground->setTint(color);
	}

	GUIProgressBar* GUIProgressBar::create(const String& styleName)
	{
		return new (bs_alloc<GUIProgressBar, PoolAlloc>()) GUIProgressBar(getStyleName<GUIProgressBar>(styleName), GUILayoutOptions::create());
	}

	GUIProgressBar* GUIProgressBar::create(const GUIOptions& layoutOptions, const String& styleName)
	{
		return new (bs_alloc<GUIProgressBar, PoolAlloc>()) GUIProgressBar(getStyleName<GUIProgressBar>(styleName), GUILayoutOptions::create(layoutOptions));
	}

	const String& GUIProgressBar::getGUITypeName()
	{
		static String typeName = "ProgressBar";
		return typeName;
	}
}
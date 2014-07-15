#include "BsGUIFoldout.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUIToggle.h"
#include "BsGUITexture.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"

namespace BansheeEngine
{
	const String GUIFoldout::FOLDOUT_BUTTON_STYLE = "FoldoutButton";
	const String GUIFoldout::FOLDOUT_BG_STYLE = "FoldoutBackground";

	GUIFoldout::GUIFoldout(const PrivatelyConstruct& dummy, const String& toggleStyle, 
		const String& backgroundStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions, backgroundStyle), mToggle(nullptr), mBackground(nullptr), mIsExpanded(false)
	{
		mToggle = GUIToggle::create(HString(L""), toggleStyle);
		mBackground = GUITexture::create(backgroundStyle);

		_registerChildElement(mToggle);
		_registerChildElement(mBackground);
	}

	GUIFoldout::~GUIFoldout()
	{

	}

	GUIFoldout* GUIFoldout::create(const GUIOptions& layoutOptions, 
		const String& toggleStyle, const String& backgroundStyle)
	{
		const String* curToggleStyle = &toggleStyle;
		if(*curToggleStyle == StringUtil::BLANK)
			curToggleStyle = &FOLDOUT_BUTTON_STYLE;

		const String* curBackgroundStyle = &backgroundStyle;
		if(*curBackgroundStyle == StringUtil::BLANK)
			curBackgroundStyle = &FOLDOUT_BG_STYLE;

		return bs_new<GUIFoldout>(PrivatelyConstruct(), *curToggleStyle, *curBackgroundStyle, 
			GUILayoutOptions::create(layoutOptions));
	}

	GUIFoldout* GUIFoldout::create(const String& toggleStyle, 
		const String& backgroundStyle)
	{
		const String* curToggleStyle = &toggleStyle;
		if(*curToggleStyle == StringUtil::BLANK)
			curToggleStyle = &FOLDOUT_BUTTON_STYLE;

		const String* curBackgroundStyle = &backgroundStyle;
		if(*curBackgroundStyle == StringUtil::BLANK)
			curBackgroundStyle = &FOLDOUT_BG_STYLE;

		return bs_new<GUIFoldout>(PrivatelyConstruct(), *curToggleStyle, *curBackgroundStyle, 
			GUILayoutOptions::create());
	}

	void GUIFoldout::setExpanded(bool expanded)
	{
		if(mIsExpanded != expanded)
		{
			mIsExpanded = expanded;

			if(mIsExpanded)
				mToggle->toggleOn();
			else
				mToggle->toggleOff();

			markContentAsDirty();

			if(!onStateChanged.empty())
				onStateChanged(mIsExpanded);
		}
	}

	void GUIFoldout::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		{
			Vector2I optimalSize = mToggle->_getOptimalSize();
			INT32 yOffset = Math::roundToInt((height - optimalSize.y) * 0.5f);

			Vector2I offset(x, y + yOffset);
			mToggle->_setOffset(offset);
			mToggle->_setWidth(optimalSize.x);
			mToggle->_setHeight(optimalSize.y);
			mToggle->_setAreaDepth(areaDepth);
			mToggle->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mToggle->_setClipRect(elemClipRect);
		}

		{
			Vector2I optimalSize = mToggle->_getOptimalSize();
			INT32 yOffset = Math::roundToInt((height - optimalSize.y) * 0.5f);

			Vector2I offset(x, y + yOffset);
			mBackground->_setOffset(offset);
			mBackground->_setWidth(mWidth);
			mBackground->_setHeight(optimalSize.y);
			mBackground->_setAreaDepth(areaDepth + 1);
			mBackground->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mBackground->_setClipRect(elemClipRect);
		}
	}

	Vector2I GUIFoldout::_getOptimalSize() const
	{
		Vector2I optimalsize = mToggle->_getOptimalSize();

		if(mBackground != nullptr)
		{
			optimalsize.x = std::max(optimalsize.x, mBackground->_getOptimalSize().x);
			optimalsize.y = std::max(optimalsize.y, mBackground->_getOptimalSize().y);
		}

		return optimalsize;
	}

	const String& GUIFoldout::getGUITypeName()
	{
		static String typeName = "Foldout";
		return typeName;
	}
}
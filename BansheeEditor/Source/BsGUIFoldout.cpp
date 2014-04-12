#include "BsGUIFoldout.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUIToggle.h"
#include "BsGUITexture.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	GUIFoldout::GUIFoldout(const PrivatelyConstruct& dummy, BS::GUIElementStyle* buttonStyle, 
		BS::GUIElementStyle* backgroundStyle, const BS::GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions), mToggle(nullptr), mBackground(nullptr), mIsExpanded(false)
	{
		const GUIElementStyle* curButtonStyle = buttonStyle;
		const GUIElementStyle* curBackgroundStyle = backgroundStyle;

		if(curButtonStyle == nullptr)
			curButtonStyle = parent.getSkin().getStyle("FoldoutButton");

		if(curBackgroundStyle == nullptr)
			curBackgroundStyle = parent.getSkin().getStyle("FoldoutBackground");

		mToggle = GUIToggle::create(parent, HString(L""), curButtonStyle);
		mBackground = GUITexture::create(parent, curBackgroundStyle);

		_registerChildElement(mToggle);
		_registerChildElement(mBackground);
	}

	GUIFoldout::~GUIFoldout()
	{

	}

	GUIFoldout* GUIFoldout::create(const GUIOptions& layoutOptions, 
		GUIElementStyle* buttonStyle, GUIElementStyle* backgroundStyle)
	{
		return cm_new<GUIFoldout>(PrivatelyConstruct(), buttonStyle, backgroundStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIFoldout* GUIFoldout::create(GUIElementStyle* buttonStyle, 
		GUIElementStyle* backgroundStyle)
	{
		return cm_new<GUIFoldout>(PrivatelyConstruct(), buttonStyle, backgroundStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
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
			Vector2I offset(x, y);
			mBackground->_setOffset(offset);
			mBackground->_setWidth(mWidth);
			mBackground->_setHeight(mHeight);
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
		static String typeName = "GUIFoldout";
		return typeName;
	}
}
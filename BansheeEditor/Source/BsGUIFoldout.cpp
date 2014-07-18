#include "BsGUIFoldout.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIToggle.h"
#include "BsGUITexture.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const String GUIFoldout::FOLDOUT_BUTTON_STYLE = "FoldoutButton";
	const String GUIFoldout::FOLDOUT_BG_STYLE = "FoldoutBackground";
	const String GUIFoldout::FOLDOUT_LABEL_STYLE = "Label";

	GUIFoldout::GUIFoldout(const PrivatelyConstruct& dummy, const HString& label, const String& labelStyle,
		const String& toggleStyle, const String& backgroundStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions, backgroundStyle), mToggle(nullptr), mBackground(nullptr), mIsExpanded(false)
	{
		mLabel = GUILabel::create(label, labelStyle);
		mToggle = GUIToggle::create(HString(L""), toggleStyle);
		mBackground = GUITexture::create(backgroundStyle);

		_registerChildElement(mLabel);
		_registerChildElement(mToggle);
		_registerChildElement(mBackground);

		mToggle->onToggled.connect(std::bind(&GUIFoldout::toggleTriggered, this, _1));
	}

	GUIFoldout::~GUIFoldout()
	{

	}

	GUIFoldout* GUIFoldout::create(const HString& label, const GUIOptions& layoutOptions,
		const String& labelStyle, const String& toggleStyle, const String& backgroundStyle)
	{
		const String* curLabelStyle = &labelStyle;
		if (*curLabelStyle == StringUtil::BLANK)
			curLabelStyle = &FOLDOUT_LABEL_STYLE;

		const String* curToggleStyle = &toggleStyle;
		if(*curToggleStyle == StringUtil::BLANK)
			curToggleStyle = &FOLDOUT_BUTTON_STYLE;

		const String* curBackgroundStyle = &backgroundStyle;
		if(*curBackgroundStyle == StringUtil::BLANK)
			curBackgroundStyle = &FOLDOUT_BG_STYLE;

		return bs_new<GUIFoldout>(PrivatelyConstruct(), label, *curLabelStyle, *curToggleStyle, *curBackgroundStyle,
			GUILayoutOptions::create(layoutOptions));
	}

	GUIFoldout* GUIFoldout::create(const HString& label, const String& labelStyle,
		const String& toggleStyle, const String& backgroundStyle)
	{
		const String* curLabelStyle = &labelStyle;
		if (*curLabelStyle == StringUtil::BLANK)
			curLabelStyle = &FOLDOUT_LABEL_STYLE;

		const String* curToggleStyle = &toggleStyle;
		if(*curToggleStyle == StringUtil::BLANK)
			curToggleStyle = &FOLDOUT_BUTTON_STYLE;

		const String* curBackgroundStyle = &backgroundStyle;
		if(*curBackgroundStyle == StringUtil::BLANK)
			curBackgroundStyle = &FOLDOUT_BG_STYLE;

		return bs_new<GUIFoldout>(PrivatelyConstruct(), label, *curLabelStyle, *curToggleStyle, *curBackgroundStyle,
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

	void GUIFoldout::toggleTriggered(bool value)
	{
		mIsExpanded = value;

		markContentAsDirty();

		onStateChanged(value);
	}

	void GUIFoldout::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 toggleOffset = 0;

		{
			Vector2I optimalSize = mToggle->_getOptimalSize();
			INT32 yOffset = Math::roundToInt(((INT32)height - optimalSize.y) * 0.5f);

			Vector2I offset(x, y + yOffset);
			mToggle->_setOffset(offset);
			mToggle->_setWidth(optimalSize.x);
			mToggle->_setHeight(optimalSize.y);
			mToggle->_setAreaDepth(areaDepth);
			mToggle->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mToggle->_setClipRect(elemClipRect);

			toggleOffset = optimalSize.x;
		}

		{
			Vector2I optimalSize = mLabel->_getOptimalSize();
			INT32 yOffset = Math::roundToInt(((INT32)height - optimalSize.y) * 0.5f);

			Vector2I offset(x + toggleOffset, y + yOffset);
			mLabel->_setOffset(offset);
			mLabel->_setWidth(optimalSize.x);
			mLabel->_setHeight(optimalSize.y);
			mLabel->_setAreaDepth(areaDepth);
			mLabel->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mLabel->_setClipRect(elemClipRect);
		}

		{
			Vector2I labelOptimalSize = mLabel->_getOptimalSize();
			Vector2I toggleOptimalSize = mToggle->_getOptimalSize();
			INT32 maxHeight = std::max(labelOptimalSize.y, toggleOptimalSize.y);

			INT32 yOffset = Math::roundToInt(((INT32)height - maxHeight) * 0.5f);

			Vector2I offset(x, y + yOffset);
			mBackground->_setOffset(offset);
			mBackground->_setWidth(mWidth);
			mBackground->_setHeight(maxHeight);
			mBackground->_setAreaDepth(areaDepth + 1);
			mBackground->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mBackground->_setClipRect(elemClipRect);
		}
	}

	Vector2I GUIFoldout::_getOptimalSize() const
	{
		Vector2I optimalsize = mToggle->_getOptimalSize();
		Vector2I labelOptimalSize = mLabel->_getOptimalSize();

		optimalsize.x += labelOptimalSize.x;
		optimalsize.y = std::max(optimalsize.y, labelOptimalSize.y);

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
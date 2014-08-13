#include "BsGUIComponentFoldout.h"
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
	GUIComponentFoldout::GUIComponentFoldout(const PrivatelyConstruct& dummy, const HString& label, const String& style,
		const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions, style), mToggle(nullptr), mBackground(nullptr), mIsExpanded(false)
	{
		mLabel = GUILabel::create(label, getSubStyleName(getLabelStyleType()));
		mToggle = GUIToggle::create(HString(L""), getSubStyleName(getFoldoutButtonStyleType()));
		mBackground = GUITexture::create(getSubStyleName(getBackgroundStyleType()));

		_registerChildElement(mLabel);
		_registerChildElement(mToggle);
		_registerChildElement(mBackground);

		mToggle->onToggled.connect(std::bind(&GUIComponentFoldout::toggleTriggered, this, _1));
	}

	GUIComponentFoldout::~GUIComponentFoldout()
	{

	}

	GUIComponentFoldout* GUIComponentFoldout::create(const HString& label, const GUIOptions& layoutOptions,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIComponentFoldout>(PrivatelyConstruct(), label, *curStyle, GUILayoutOptions::create(layoutOptions));
	}

	GUIComponentFoldout* GUIComponentFoldout::create(const HString& label, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIComponentFoldout>(PrivatelyConstruct(), label, *curStyle, GUILayoutOptions::create());
	}

	void GUIComponentFoldout::setExpanded(bool expanded)
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

	void GUIComponentFoldout::setContent(const GUIContent& content)
	{
		mLabel->setContent(content);
	}

	void GUIComponentFoldout::toggleTriggered(bool value)
	{
		mIsExpanded = value;

		markContentAsDirty();

		onStateChanged(value);
	}

	void GUIComponentFoldout::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
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

	Vector2I GUIComponentFoldout::_getOptimalSize() const
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

	void GUIComponentFoldout::styleUpdated()
	{
		mLabel->setStyle(getSubStyleName(getLabelStyleType()));
		mToggle->setStyle(getSubStyleName(getFoldoutButtonStyleType()));
		mBackground->setStyle(getSubStyleName(getBackgroundStyleType()));
	}

	const String& GUIComponentFoldout::getGUITypeName()
	{
		static String typeName = "ComponentFoldout";
		return typeName;
	}

	const String& GUIComponentFoldout::getFoldoutButtonStyleType()
	{
		static String FOLDOUT_BUTTON_STYLE = "ComponentFoldoutButton";
		return FOLDOUT_BUTTON_STYLE;		
	}

	const String& GUIComponentFoldout::getBackgroundStyleType()
	{
		static String FOLDOUT_BG_STYLE = "ComponentFoldoutBackground";
		return FOLDOUT_BG_STYLE;
	}

	const String& GUIComponentFoldout::getLabelStyleType()
	{
		static String FOLDOUT_LABEL_STYLE = "EditorFieldLabel";
		return FOLDOUT_LABEL_STYLE;
	}
}
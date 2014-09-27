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
		:GUIElementContainer(layoutOptions, style), mToggle(nullptr), mIsExpanded(false)
	{
		mToggle = GUIToggle::create(label, getSubStyleName(getFoldoutButtonStyleType()));

		_registerChildElement(mToggle);

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
		mToggle->setContent(content);
	}

	void GUIComponentFoldout::toggleTriggered(bool value)
	{
		mIsExpanded = value;

		markContentAsDirty();

		onStateChanged(value);
	}

	void GUIComponentFoldout::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 toggleOffset = 0;

		{
			Vector2I optimalSize = mToggle->_getOptimalSize();
			INT32 yOffset = Math::roundToInt(((INT32)height - optimalSize.y) * 0.5f);

			Vector2I offset(x, y + yOffset);
			mToggle->_setOffset(offset);
			mToggle->_setWidth(width);
			mToggle->_setHeight(optimalSize.y);
			mToggle->_setAreaDepth(areaDepth);
			mToggle->_setWidgetDepth(widgetDepth);

			Rect2I elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mToggle->_setClipRect(elemClipRect);

			toggleOffset = optimalSize.x;
		}
	}

	Vector2I GUIComponentFoldout::_getOptimalSize() const
	{
		Vector2I optimalsize = mToggle->_getOptimalSize();

		return optimalsize;
	}

	void GUIComponentFoldout::styleUpdated()
	{
		mToggle->setStyle(getSubStyleName(getFoldoutButtonStyleType()));
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
}
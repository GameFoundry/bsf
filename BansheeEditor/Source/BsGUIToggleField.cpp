#include "BsGUIToggleField.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIToggle.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	GUIToggleField::GUIToggleField(const PrivatelyConstruct& dummy, GUIWidget& parent, const GUIContent& labelContent, 
		GUIElementStyle* labelStyle, GUIElementStyle* toggleStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mToggle(nullptr), mLabelWidth(100)
	{
		const GUIElementStyle* curLabelStyle = labelStyle;
		const GUIElementStyle* curToggleStyle = toggleStyle;

		if(curLabelStyle == nullptr)
			curLabelStyle = parent.getSkin().getStyle("Label");

		if(curToggleStyle == nullptr)
			curToggleStyle = parent.getSkin().getStyle("Toggle");

		mLabel = GUILabel::create(parent, labelContent, curLabelStyle);
		mToggle = GUIToggle::create(parent, HString(L""), curToggleStyle);

		_registerChildElement(mLabel);
		_registerChildElement(mToggle);
	}

	GUIToggleField::GUIToggleField(const PrivatelyConstruct& dummy, GUIWidget& parent, 
		GUIElementStyle* labelStyle, GUIElementStyle* toggleStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mToggle(nullptr), mLabelWidth(100)
	{
		const GUIElementStyle* curToggleStyle = toggleStyle;

		if(curToggleStyle == nullptr)
			curToggleStyle = parent.getSkin().getStyle("Toggle");

		mToggle = GUIToggle::create(parent, HString(L""), curToggleStyle);

		_registerChildElement(mToggle);
	}

	GUIToggleField::~GUIToggleField()
	{

	}

	GUIToggleField* GUIToggleField::create(GUIWidget& parent, const GUIContent& labelContent, const GUIOptions& layoutOptions, 
		GUIElementStyle* labelStyle, GUIElementStyle* toggleStyle)
	{
		return cm_new<GUIToggleField>(PrivatelyConstruct(), parent, labelContent, labelStyle, toggleStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIToggleField* GUIToggleField::create(GUIWidget& parent, const GUIContent& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* toggleStyle)
	{
		return cm_new<GUIToggleField>(PrivatelyConstruct(), parent, labelContent, labelStyle, toggleStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIToggleField* GUIToggleField::create(GUIWidget& parent, const HString& labelContent, const GUIOptions& layoutOptions, 
		GUIElementStyle* labelStyle, GUIElementStyle* toggleStyle)
	{
		return cm_new<GUIToggleField>(PrivatelyConstruct(), parent, GUIContent(labelContent), labelStyle, 
			toggleStyle, GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIToggleField* GUIToggleField::create(GUIWidget& parent, const HString& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* toggleStyle)
	{
		return cm_new<GUIToggleField>(PrivatelyConstruct(), parent, GUIContent(labelContent), labelStyle, toggleStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIToggleField* GUIToggleField::create(GUIWidget& parent, const GUIOptions& layoutOptions, GUIElementStyle* labelStyle, 
		GUIElementStyle* toggleStyle)
	{
		return cm_new<GUIToggleField>(PrivatelyConstruct(), parent, labelStyle, toggleStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIToggleField* GUIToggleField::create(GUIWidget& parent, GUIElementStyle* labelStyle, GUIElementStyle* toggleStyle)
	{
		return cm_new<GUIToggleField>(PrivatelyConstruct(), parent, labelStyle, toggleStyle, GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	bool GUIToggleField::getValue() const
	{
		return mToggle->isToggled();
	}

	void GUIToggleField::setValue(bool value)
	{
		if(value) 
			mToggle->toggleOn();
		else
			mToggle->toggleOff();
	}

	void GUIToggleField::setLabelWidth(UINT32 width)
	{
		mLabelWidth = width;

		markContentAsDirty();
	}

	void GUIToggleField::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 toggleOffset = 0;
		UINT32 toggleWidth = width;

		if(mLabel != nullptr)
		{
			UINT32 labelWidth = mLabelWidth;

			Vector2I optimalSize = mLabel->_getOptimalSize();
			INT32 yOffset = Math::roundToInt((height - optimalSize.y) * 0.5f);

			Vector2I offset(x, y + yOffset);
			mLabel->_setOffset(offset);
			mLabel->_setWidth(labelWidth);
			mLabel->_setHeight(optimalSize.y);
			mLabel->_setAreaDepth(areaDepth);
			mLabel->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mLabel->_setClipRect(elemClipRect);

			toggleOffset = labelWidth;
			toggleWidth = width - labelWidth;
		}

		Vector2I toggleSize = mToggle->_getOptimalSize();

		{
			Vector2I optimalSize = mToggle->_getOptimalSize();
			INT32 yOffset = Math::roundToInt((height - optimalSize.y) * 0.5f);

			Vector2I offset(x + toggleOffset, y + yOffset);
			mToggle->_setOffset(offset);
			mToggle->_setWidth(toggleWidth);
			mToggle->_setHeight(optimalSize.y);
			mToggle->_setAreaDepth(areaDepth);
			mToggle->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mToggle->_setClipRect(elemClipRect);
		}
	}

	Vector2I GUIToggleField::_getOptimalSize() const
	{
		Vector2I optimalsize = mToggle->_getOptimalSize();

		if(mLabel != nullptr)
		{
			optimalsize.x += mLabel->_getOptimalSize().x;
			optimalsize.y = std::max(optimalsize.y, mLabel->_getOptimalSize().y);
		}

		return optimalsize;
	}

	const String& GUIToggleField::getGUITypeName()
	{
		static String typeName = "GUIToggleField";
		return typeName;
	}
}
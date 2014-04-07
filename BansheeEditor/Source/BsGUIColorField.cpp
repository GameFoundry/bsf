#include "BsGUIColorField.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIColor.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	GUIColorField::GUIColorField(const PrivatelyConstruct& dummy, GUIWidget& parent, const GUIContent& labelContent, 
		GUIElementStyle* labelStyle, GUIElementStyle* colorStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mColor(nullptr), mLabelWidth(100)
	{
		const GUIElementStyle* curLabelStyle = labelStyle;
		const GUIElementStyle* curColorStyle = colorStyle;

		if(curLabelStyle == nullptr)
			curLabelStyle = parent.getSkin().getStyle("Label");

		if(curColorStyle == nullptr)
			curColorStyle = parent.getSkin().getStyle("Color");

		mLabel = GUILabel::create(parent, labelContent, curLabelStyle);
		mColor = GUIColor::create(parent, curColorStyle);

		_registerChildElement(mLabel);
		_registerChildElement(mColor);
	}

	GUIColorField::GUIColorField(const PrivatelyConstruct& dummy, GUIWidget& parent, 
		GUIElementStyle* labelStyle, GUIElementStyle* colorStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mColor(nullptr), mLabelWidth(100)
	{
		const GUIElementStyle* curColorStyle = colorStyle;

		if(curColorStyle == nullptr)
			curColorStyle = parent.getSkin().getStyle("Color");

		mColor = GUIColor::create(parent, curColorStyle);

		_registerChildElement(mColor);
	}

	GUIColorField::~GUIColorField()
	{

	}

	GUIColorField* GUIColorField::create(GUIWidget& parent, const GUIContent& labelContent, const GUIOptions& layoutOptions, 
		GUIElementStyle* labelStyle, GUIElementStyle* toggleStyle)
	{
		return cm_new<GUIColorField>(PrivatelyConstruct(), parent, labelContent, labelStyle, toggleStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIColorField* GUIColorField::create(GUIWidget& parent, const GUIContent& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* toggleStyle)
	{
		return cm_new<GUIColorField>(PrivatelyConstruct(), parent, labelContent, labelStyle, toggleStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIColorField* GUIColorField::create(GUIWidget& parent, const HString& labelContent, const GUIOptions& layoutOptions, 
		GUIElementStyle* labelStyle, GUIElementStyle* toggleStyle)
	{
		return cm_new<GUIColorField>(PrivatelyConstruct(), parent, GUIContent(labelContent), labelStyle, 
			toggleStyle, GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIColorField* GUIColorField::create(GUIWidget& parent, const HString& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* toggleStyle)
	{
		return cm_new<GUIColorField>(PrivatelyConstruct(), parent, GUIContent(labelContent), labelStyle, toggleStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIColorField* GUIColorField::create(GUIWidget& parent, const GUIOptions& layoutOptions, GUIElementStyle* labelStyle, 
		GUIElementStyle* toggleStyle)
	{
		return cm_new<GUIColorField>(PrivatelyConstruct(), parent, labelStyle, toggleStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIColorField* GUIColorField::create(GUIWidget& parent, GUIElementStyle* labelStyle, GUIElementStyle* toggleStyle)
	{
		return cm_new<GUIColorField>(PrivatelyConstruct(), parent, labelStyle, toggleStyle, GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	Color GUIColorField::getValue() const
	{
		return mColor->getColor();
	}

	void GUIColorField::setValue(const CM::Color& color)
	{
		mColor->setColor(color);
	}

	void GUIColorField::setLabelWidth(UINT32 width)
	{
		mLabelWidth = width;

		markContentAsDirty();
	}

	void GUIColorField::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 colorOffset = 0;
		UINT32 colorWidth = width;

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

			colorOffset = labelWidth;
			colorWidth = width - labelWidth;
		}

		{
			Vector2I optimalSize = mColor->_getOptimalSize();
			INT32 yOffset = Math::roundToInt((height - optimalSize.y) * 0.5f);

			Vector2I offset(x + colorOffset, y + yOffset);
			mColor->_setOffset(offset);
			mColor->_setWidth(colorWidth);
			mColor->_setHeight(optimalSize.y);
			mColor->_setAreaDepth(areaDepth);
			mColor->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mColor->_setClipRect(elemClipRect);
		}
	}

	Vector2I GUIColorField::_getOptimalSize() const
	{
		Vector2I optimalsize = mColor->_getOptimalSize();

		if(mLabel != nullptr)
		{
			optimalsize.x += mLabel->_getOptimalSize().x;
			optimalsize.y = std::max(optimalsize.y, mLabel->_getOptimalSize().y);
		}

		return optimalsize;
	}

	const String& GUIColorField::getGUITypeName()
	{
		static String typeName = "GUIColorField";
		return typeName;
	}
}
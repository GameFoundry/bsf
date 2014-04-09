#include "BsGUIVector2Field.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIFloatField.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const UINT32 GUIVector2Field::ELEMENT_LABEL_WIDTH = 10;

	GUIVector2Field::GUIVector2Field(const PrivatelyConstruct& dummy, GUIWidget& parent, const GUIContent& labelContent, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mFieldX(nullptr), mFieldY(nullptr)
	{
		construct(parent, labelContent, labelStyle, inputBoxStyle, layoutOptions, true);
	}

	GUIVector2Field::GUIVector2Field(const PrivatelyConstruct& dummy, GUIWidget& parent, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mFieldX(nullptr), mFieldY(nullptr)
	{
		construct(parent, GUIContent(HString()), labelStyle, inputBoxStyle, layoutOptions, false);
	}

	GUIVector2Field::~GUIVector2Field()
	{

	}

	GUIVector2Field* GUIVector2Field::create(GUIWidget& parent, const GUIContent& labelContent, const GUIOptions& layoutOptions, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIVector2Field>(PrivatelyConstruct(), parent, labelContent, labelStyle, inputBoxStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIVector2Field* GUIVector2Field::create(GUIWidget& parent, const GUIContent& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIVector2Field>(PrivatelyConstruct(), parent, labelContent, labelStyle, inputBoxStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIVector2Field* GUIVector2Field::create(GUIWidget& parent, const HString& labelContent, const GUIOptions& layoutOptions, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIVector2Field>(PrivatelyConstruct(), parent, GUIContent(labelContent), labelStyle, 
			inputBoxStyle, GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIVector2Field* GUIVector2Field::create(GUIWidget& parent, const HString& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIVector2Field>(PrivatelyConstruct(), parent, GUIContent(labelContent), labelStyle, inputBoxStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIVector2Field* GUIVector2Field::create(GUIWidget& parent, const GUIOptions& layoutOptions, GUIElementStyle* labelStyle, 
		GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIVector2Field>(PrivatelyConstruct(), parent, labelStyle, inputBoxStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIVector2Field* GUIVector2Field::create(GUIWidget& parent, GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIVector2Field>(PrivatelyConstruct(), parent, labelStyle, inputBoxStyle, GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	void GUIVector2Field::construct(GUIWidget& parent, const GUIContent& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
	{
		if(withLabel)
		{
			const GUIElementStyle* curLabelStyle = labelStyle;

			if(curLabelStyle == nullptr)
				curLabelStyle = parent.getSkin().getStyle("Label");

			mLabel = GUILabel::create(parent, labelContent, curLabelStyle);
		}

		mFieldX = GUIFloatField::create(parent, HString(L"X"), ELEMENT_LABEL_WIDTH, labelStyle, inputBoxStyle);
		mFieldY = GUIFloatField::create(parent, HString(L"Y"), ELEMENT_LABEL_WIDTH, labelStyle, inputBoxStyle);

		_registerChildElement(mLabel);
		_registerChildElement(mFieldX);
		_registerChildElement(mFieldY);
	}

	Vector2 GUIVector2Field::getValue() const
	{
		Vector2 value;
		value.x = mFieldX->getValue();
		value.y = mFieldY->getValue();

		return value;
	}

	void GUIVector2Field::setValue(const Vector2& value)
	{
		mFieldX->setValue(value.x);
		mFieldY->setValue(value.y);
	}

	void GUIVector2Field::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 inputBoxYOffset = 0;

		if(mLabel != nullptr)
		{
			Vector2I optimalSize = mLabel->_getOptimalSize();
			Vector2I offset(x, y);
			mLabel->_setOffset(offset);
			mLabel->_setWidth(width);
			mLabel->_setHeight(optimalSize.y);
			mLabel->_setAreaDepth(areaDepth);
			mLabel->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mLabel->_setClipRect(elemClipRect);

			inputBoxYOffset = optimalSize.y;
		}

		GUIFloatField* fields[] = { mFieldX, mFieldY };
		UINT32 numFields = sizeof(fields) / sizeof(fields[0]);

		UINT32 sizePerField = width / numFields;
		UINT32 inputBoxXOffset = 0;

		for(UINT32 i = 0; i < numFields; i++)
		{
			Vector2I optimalSize = fields[i]->_getOptimalSize();
			INT32 yOffset = Math::roundToInt((height - optimalSize.y) * 0.5f);

			Vector2I offset(x + inputBoxXOffset, y + inputBoxYOffset);
			fields[i]->_setOffset(offset);
			fields[i]->_setWidth(sizePerField);
			fields[i]->_setHeight(optimalSize.y);
			fields[i]->_setAreaDepth(areaDepth);
			fields[i]->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			fields[i]->_setClipRect(elemClipRect);

			RectI newClipRect(offset.x, offset.y, sizePerField, optimalSize.y);
			newClipRect.clip(clipRect);
			fields[i]->_updateLayoutInternal(offset.x, offset.y, sizePerField, optimalSize.y, newClipRect, widgetDepth, areaDepth);

			inputBoxXOffset += sizePerField;
		}
	}

	Vector2I GUIVector2Field::_getOptimalSize() const
	{
		GUIFloatField* fields[] = { mFieldX, mFieldY };
		UINT32 numFields = sizeof(fields) / sizeof(fields[0]);

		Vector2I optimalSize;

		for(UINT32 i = 0; i < numFields; i++)
		{
			optimalSize.x += fields[i]->_getOptimalSize().x;
			optimalSize.y = std::max(optimalSize.y, fields[i]->_getOptimalSize().y);
		}

		if(mLabel != nullptr)
		{
			optimalSize.x = std::max(optimalSize.x, mLabel->_getOptimalSize().x);
			optimalSize.y += mLabel->_getOptimalSize().y;
		}

		return optimalSize;
	}

	const String& GUIVector2Field::getGUITypeName()
	{
		static String typeName = "GUIVector2Field";
		return typeName;
	}
}
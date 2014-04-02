#include "BsGUIIntField.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIInputBox.h"
#include "BsGUISpace.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include <regex>

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const float GUIIntField::SPLIT_POSITION = 0.5f;

	GUIIntField::GUIIntField(const PrivatelyConstruct& dummy, GUIWidget& parent, const GUIContent& labelContent, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mInputBox(nullptr)
	{
		const GUIElementStyle* curLabelStyle = labelStyle;
		const GUIElementStyle* curInputBoxStyle = inputBoxStyle;

		if(curLabelStyle == nullptr)
			curLabelStyle = parent.getSkin().getStyle("Label");

		if(curInputBoxStyle == nullptr)
			curInputBoxStyle = parent.getSkin().getStyle("InputBox");

		mLabel = GUILabel::create(parent, labelContent, curLabelStyle);
		mInputBox = GUIInputBox::create(parent, false, inputBoxStyle);
		mInputBox->setFilter(&GUIIntField::intFilter);

		_registerChildElement(mLabel);
		_registerChildElement(mInputBox);
	}

	GUIIntField::GUIIntField(const PrivatelyConstruct& dummy, GUIWidget& parent, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mInputBox(nullptr)
	{
		const GUIElementStyle* curInputBoxStyle = inputBoxStyle;

		if(curInputBoxStyle == nullptr)
			curInputBoxStyle = parent.getSkin().getStyle("InputBox");

		mInputBox = GUIInputBox::create(parent, false, inputBoxStyle);
		mInputBox->setFilter(&GUIIntField::intFilter);

		_registerChildElement(mInputBox);
	}

	GUIIntField::~GUIIntField()
	{

	}

	GUIIntField* GUIIntField::create(GUIWidget& parent, const GUIContent& labelContent, const GUIOptions& layoutOptions, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIIntField>(PrivatelyConstruct(), parent, labelContent, labelStyle, inputBoxStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIIntField* GUIIntField::create(GUIWidget& parent, const GUIContent& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIIntField>(PrivatelyConstruct(), parent, labelContent, labelStyle, inputBoxStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIIntField* GUIIntField::create(GUIWidget& parent, const HString& labelContent, const GUIOptions& layoutOptions, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIIntField>(PrivatelyConstruct(), parent, GUIContent(labelContent), labelStyle, 
			inputBoxStyle, GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIIntField* GUIIntField::create(GUIWidget& parent, const HString& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIIntField>(PrivatelyConstruct(), parent, GUIContent(labelContent), labelStyle, inputBoxStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIIntField* GUIIntField::create(GUIWidget& parent, const GUIOptions& layoutOptions, GUIElementStyle* labelStyle, 
		GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIIntField>(PrivatelyConstruct(), parent, labelStyle, inputBoxStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIIntField* GUIIntField::create(GUIWidget& parent, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIIntField>(PrivatelyConstruct(), parent, labelStyle, inputBoxStyle, GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	bool GUIIntField::mouseEvent(const GUIMouseEvent& event)
	{
		GUIElementContainer::mouseEvent(event);

		if(event.getType() == GUIMouseEventType::MouseDragStart)
		{
			// TODO -If over draggable area start drag

			return true;
		}
		else if(event.getType() == GUIMouseEventType::MouseDrag)
		{
			// TODO - If drag is started increase/lower the value

			return true;
		}
		else if(event.getType() == GUIMouseEventType::MouseOut)
		{
			// TODO - Ensure cursor is set back to arrow
			
			return true;
		}
		else if(event.getType() == GUIMouseEventType::MouseMove)
		{
			// TODO - If mouse is over drag area change cursor to Arrow Left/Right

			return true;
		}

		return false;
	}

	CM::INT32 GUIIntField::getValue() const
	{
		return parseInt(mInputBox->getText());
	}

	void GUIIntField::setValue(CM::INT32 value)
	{
		mInputBox->setText(toWString(value));
	}

	void GUIIntField::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 inputBoxOffset = 0;
		UINT32 inputBoxWidth = width;

		if(mLabel != nullptr)
		{
			UINT32 labelWidth = Math::roundToInt(width * SPLIT_POSITION);

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

			inputBoxOffset = labelWidth;
			inputBoxWidth = width - labelWidth;
		}

		Vector2I inputBoxSize = mInputBox->_getOptimalSize();
		
		{
			Vector2I optimalSize = mInputBox->_getOptimalSize();
			INT32 yOffset = Math::roundToInt((height - optimalSize.y) * 0.5f);
			
			Vector2I offset(x + inputBoxOffset, y + yOffset);
			mInputBox->_setOffset(offset);
			mInputBox->_setWidth(inputBoxWidth);
			mInputBox->_setHeight(optimalSize.y);
			mInputBox->_setAreaDepth(areaDepth);
			mInputBox->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mInputBox->_setClipRect(elemClipRect);
		}
	}

	const String& GUIIntField::getGUITypeName()
	{
		static String typeName = "GUIIntField";
		return typeName;
	}

	bool GUIIntField::intFilter(const CM::WString& str)
	{
		return std::regex_match(str, std::wregex(L"-?(\\d+)?"));
	}
}
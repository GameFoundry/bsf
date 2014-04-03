#include "BsGUIFloatField.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIInputBox.h"
#include "BsGUISpace.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsCursor.h"
#include "BsGUIWidget.h"
#include "CmViewport.h"
#include <regex>

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const float GUIFloatField::SPLIT_POSITION = 0.5f;
	const float GUIFloatField::DRAG_SPEED = 0.05f;

	GUIFloatField::GUIFloatField(const PrivatelyConstruct& dummy, GUIWidget& parent, const GUIContent& labelContent, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mInputBox(nullptr), mIsDragging(false),
		mLastDragPos(0), mIsDragCursorSet(false)
	{
		const GUIElementStyle* curLabelStyle = labelStyle;
		const GUIElementStyle* curInputBoxStyle = inputBoxStyle;

		if(curLabelStyle == nullptr)
			curLabelStyle = parent.getSkin().getStyle("Label");

		if(curInputBoxStyle == nullptr)
			curInputBoxStyle = parent.getSkin().getStyle("InputBox");

		mLabel = GUILabel::create(parent, labelContent, curLabelStyle);
		mInputBox = GUIInputBox::create(parent, false, inputBoxStyle);
		mInputBox->setFilter(&GUIFloatField::floatFilter);

		_registerChildElement(mLabel);
		_registerChildElement(mInputBox);

		setValue(0);
	}

	GUIFloatField::GUIFloatField(const PrivatelyConstruct& dummy, GUIWidget& parent, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mInputBox(nullptr), mIsDragging(false),
		mLastDragPos(0)
	{
		const GUIElementStyle* curInputBoxStyle = inputBoxStyle;

		if(curInputBoxStyle == nullptr)
			curInputBoxStyle = parent.getSkin().getStyle("InputBox");

		mInputBox = GUIInputBox::create(parent, false, inputBoxStyle);
		mInputBox->setFilter(&GUIFloatField::floatFilter);

		_registerChildElement(mInputBox);

		setValue(0);
	}

	GUIFloatField::~GUIFloatField()
	{

	}

	GUIFloatField* GUIFloatField::create(GUIWidget& parent, const GUIContent& labelContent, const GUIOptions& layoutOptions, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIFloatField>(PrivatelyConstruct(), parent, labelContent, labelStyle, inputBoxStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIFloatField* GUIFloatField::create(GUIWidget& parent, const GUIContent& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIFloatField>(PrivatelyConstruct(), parent, labelContent, labelStyle, inputBoxStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIFloatField* GUIFloatField::create(GUIWidget& parent, const HString& labelContent, const GUIOptions& layoutOptions, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIFloatField>(PrivatelyConstruct(), parent, GUIContent(labelContent), labelStyle, 
			inputBoxStyle, GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIFloatField* GUIFloatField::create(GUIWidget& parent, const HString& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIFloatField>(PrivatelyConstruct(), parent, GUIContent(labelContent), labelStyle, inputBoxStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIFloatField* GUIFloatField::create(GUIWidget& parent, const GUIOptions& layoutOptions, GUIElementStyle* labelStyle, 
		GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIFloatField>(PrivatelyConstruct(), parent, labelStyle, inputBoxStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIFloatField* GUIFloatField::create(GUIWidget& parent, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle)
	{
		return cm_new<GUIFloatField>(PrivatelyConstruct(), parent, labelStyle, inputBoxStyle, GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	bool GUIFloatField::mouseEvent(const GUIMouseEvent& event)
	{
		GUIElementContainer::mouseEvent(event);

		RectI draggableArea;

		if(mLabel != nullptr)
			draggableArea = mLabel->getBounds();

		if(event.getType() == GUIMouseEventType::MouseDragStart)
		{
			if(draggableArea.contains(event.getPosition()))
			{
				mLastDragPos = event.getPosition().x;
				mIsDragging = true;

				Cursor::instance().setCursor(CursorType::ArrowLeftRight);
				mIsDragCursorSet = true;
			}

			return true;
		}
		else if(event.getType() == GUIMouseEventType::MouseDrag)
		{
			if(mIsDragging)
			{
				INT32 xDiff = event.getPosition().x - mLastDragPos;

				INT32 jumpAmount = 0;
				if(event.getPosition().x < 0)
				{
					Vector2I cursorScreenPos = Cursor::instance().getScreenPosition();
					cursorScreenPos.x += _getParentWidget().getTarget()->getWidth();
					jumpAmount = _getParentWidget().getTarget()->getWidth();

					Cursor::instance().setScreenPosition(cursorScreenPos);
				}
				else if(event.getPosition().x >= _getParentWidget().getTarget()->getWidth())
				{
					Vector2I cursorScreenPos = Cursor::instance().getScreenPosition();
					cursorScreenPos.x -= _getParentWidget().getTarget()->getWidth();
					jumpAmount = -_getParentWidget().getTarget()->getWidth();

					Cursor::instance().setScreenPosition(cursorScreenPos);
				}

				float oldValue = getValue();
				float newValue = oldValue + xDiff * DRAG_SPEED;

				mLastDragPos = event.getPosition().x + jumpAmount;

				if(oldValue != newValue)
					setValue(newValue);
			}

			return true;
		}
		else if(event.getType() == GUIMouseEventType::MouseDragEnd)
		{
			mIsDragging = false;

			if(mIsDragCursorSet)
			{
				Cursor::instance().setCursor(CursorType::Arrow);
				mIsDragCursorSet = false;
			}

			return true;
		}
		else if(event.getType() == GUIMouseEventType::MouseOut)
		{
			if(!mIsDragging)
			{
				if(mIsDragCursorSet)
				{
					Cursor::instance().setCursor(CursorType::Arrow);
					mIsDragCursorSet = false;
				}
			}
		}
		else if(event.getType() == GUIMouseEventType::MouseMove)
		{
			if(draggableArea.contains(event.getPosition()))
			{
				Cursor::instance().setCursor(CursorType::ArrowLeftRight);
				mIsDragCursorSet = true;
			}
			else
			{
				if(!mIsDragging)
				{
					if(mIsDragCursorSet)
					{
						Cursor::instance().setCursor(CursorType::Arrow);
						mIsDragCursorSet = false;
					}
				}
			}
		}

		return false;
	}

	float GUIFloatField::getValue() const
	{
		return parseFloat(mInputBox->getText());
	}

	void GUIFloatField::setValue(float value)
	{
		mInputBox->setText(toWString(value));
	}

	void GUIFloatField::updateClippedBounds()
	{
		Vector2I offset = _getOffset();
		mClippedBounds = RectI(offset.x, offset.y, _getWidth(), _getHeight());
	}

	void GUIFloatField::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
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

	const String& GUIFloatField::getGUITypeName()
	{
		static String typeName = "GUIFloatField";
		return typeName;
	}

	bool GUIFloatField::floatFilter(const CM::WString& str)
	{
		return std::regex_match(str, std::wregex(L"-?(\\d+(\\.\\d*)?)?"));
	}
}
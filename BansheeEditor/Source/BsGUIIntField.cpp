#include "BsGUIIntField.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIInputBox.h"
#include "BsGUISpace.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"
#include "BsCursor.h"
#include "CmViewport.h"
#include <regex>

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const INT32 GUIIntField::DRAG_SPEED = 5;

	GUIIntField::GUIIntField(const PrivatelyConstruct& dummy, GUIWidget& parent, const GUIContent& labelContent, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mInputBox(nullptr), mIsDragging(false),
		mLastDragPos(0), mIsDragCursorSet(false), mLabelWidth(100)
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

		setValue(0);
	}

	GUIIntField::GUIIntField(const PrivatelyConstruct& dummy, GUIWidget& parent, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mInputBox(nullptr), mIsDragging(false),
		mLastDragPos(0), mLabelWidth(100)
	{
		const GUIElementStyle* curInputBoxStyle = inputBoxStyle;

		if(curInputBoxStyle == nullptr)
			curInputBoxStyle = parent.getSkin().getStyle("InputBox");

		mInputBox = GUIInputBox::create(parent, false, inputBoxStyle);
		mInputBox->setFilter(&GUIIntField::intFilter);

		_registerChildElement(mInputBox);

		setValue(0);
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

				INT32 oldValue = getValue();
				INT32 newValue = oldValue;

				if(xDiff >= DRAG_SPEED)
				{
					while(xDiff >= DRAG_SPEED)
					{
						newValue++;
						xDiff -= DRAG_SPEED;
					}
				}
				else if(xDiff <= -DRAG_SPEED)
				{
					while(xDiff <= -DRAG_SPEED)
					{
						newValue--;
						xDiff += DRAG_SPEED;
					}
				}
				
				mLastDragPos += (newValue - oldValue) * DRAG_SPEED + jumpAmount;

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

	CM::INT32 GUIIntField::getValue() const
	{
		return parseInt(mInputBox->getText());
	}

	void GUIIntField::setValue(CM::INT32 value)
	{
		mInputBox->setText(toWString(value));
	}

	void GUIIntField::setLabelWidth(UINT32 width)
	{
		mLabelWidth = width;

		markContentAsDirty();
	}

	void GUIIntField::updateClippedBounds()
	{
		Vector2I offset = _getOffset();
		mClippedBounds = RectI(offset.x, offset.y, _getWidth(), _getHeight());
	}

	void GUIIntField::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 inputBoxOffset = 0;
		UINT32 inputBoxWidth = width;

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

	Vector2I GUIIntField::_getOptimalSize() const
	{
		Vector2I optimalsize = mInputBox->_getOptimalSize();

		if(mLabel != nullptr)
		{
			optimalsize.x += mLabel->_getOptimalSize().x;
			optimalsize.y = std::max(optimalsize.y, mLabel->_getOptimalSize().y);
		}

		return optimalsize;
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
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

	GUIIntField::GUIIntField(const PrivatelyConstruct& dummy, GUIWidget& parent, const GUIContent& labelContent, UINT32 labelWidth,
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:TGUIField(dummy, parent, labelContent, labelWidth, labelStyle, layoutOptions, withLabel), mInputBox(nullptr), mIsDragging(false),
		mLastDragPos(0), mIsDragCursorSet(false)
	{
		const GUIElementStyle* curInputBoxStyle = inputBoxStyle;
		if(curInputBoxStyle == nullptr)
			curInputBoxStyle = parent.getSkin().getStyle("InputBox");

		mInputBox = GUIInputBox::create(parent, false, GUIOptions(GUIOption::flexibleWidth()), inputBoxStyle);
		mInputBox->setFilter(&GUIIntField::intFilter);

		mLayout->addElement(mInputBox);

		setValue(0);
	}

	GUIIntField::~GUIIntField()
	{

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

	void GUIIntField::updateClippedBounds()
	{
		Vector2I offset = _getOffset();
		mClippedBounds = RectI(offset.x, offset.y, _getWidth(), _getHeight());
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
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

namespace BansheeEngine
{
	const float GUIFloatField::DRAG_SPEED = 0.05f;

	GUIFloatField::GUIFloatField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth, 
		const String& labelStyle, const String& inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, labelStyle, layoutOptions, withLabel), mInputBox(nullptr), mIsDragging(false),
		mLastDragPos(0)
	{
		mInputBox = GUIInputBox::create(false, GUIOptions(GUIOption::flexibleWidth()), inputBoxStyle);
		mInputBox->setFilter(&GUIFloatField::floatFilter);

		mLayout->addElement(mInputBox);

		setValue(0);
	}

	GUIFloatField::~GUIFloatField()
	{

	}

	bool GUIFloatField::_hasCustomCursor(const Vector2I position, CursorType& type) const
	{
		RectI draggableArea;

		if(mLabel != nullptr)
			draggableArea = mLabel->getBounds();

		if(draggableArea.contains(position))
		{
			type = CursorType::ArrowLeftRight;
			return true;
		}

		return false;
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
					cursorScreenPos.x += _getParentWidget()->getTarget()->getWidth();
					jumpAmount = _getParentWidget()->getTarget()->getWidth();

					Cursor::instance().setScreenPosition(cursorScreenPos);
				}
				else if(event.getPosition().x >= _getParentWidget()->getTarget()->getWidth())
				{
					Vector2I cursorScreenPos = Cursor::instance().getScreenPosition();
					cursorScreenPos.x -= _getParentWidget()->getTarget()->getWidth();
					jumpAmount = -_getParentWidget()->getTarget()->getWidth();

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

			return true;
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

	const String& GUIFloatField::getGUITypeName()
	{
		static String typeName = "GUIFloatField";
		return typeName;
	}

	bool GUIFloatField::floatFilter(const WString& str)
	{
		return std::regex_match(str, std::wregex(L"-?(\\d+(\\.\\d*)?)?"));
	}
}
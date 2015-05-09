#include "BsGUIFloatField.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIInputBox.h"
#include "BsGUISpace.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsCursor.h"
#include "BsGUIWidget.h"
#include "BsViewport.h"
#include "BsCmdInputFieldValueChange.h"
#include <regex>

using namespace std::placeholders;

namespace BansheeEngine
{
	const float GUIFloatField::DRAG_SPEED = 0.05f;

	GUIFloatField::GUIFloatField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth, 
		const String& style, const GUIDimensions& dimensions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, style, dimensions, withLabel), mInputBox(nullptr), mIsDragging(false),
		mLastDragPos(0), mHasInputFocus(false), mValue(0.0f)
	{
		mInputBox = GUIInputBox::create(false, GUIOptions(GUIOption::flexibleWidth()), getSubStyleName(getInputStyleType()));
		mInputBox->setFilter(&GUIFloatField::floatFilter);

		mInputBox->onValueChanged.connect(std::bind(&GUIFloatField::valueChanged, this, _1));
		mInputBox->onFocusGained.connect(std::bind(&GUIFloatField::focusGained, this));
		mInputBox->onFocusLost.connect(std::bind(&GUIFloatField::focusLost, this));

		mLayout->addElement(mInputBox);

		setValue(0);
		mInputBox->setText(L"0");
	}

	GUIFloatField::~GUIFloatField()
	{

	}

	bool GUIFloatField::_hasCustomCursor(const Vector2I position, CursorType& type) const
	{
		Rect2I draggableArea;

		if(mLabel != nullptr)
			draggableArea = mLabel->_getLayoutData().area;

		if(draggableArea.contains(position))
		{
			type = CursorType::ArrowLeftRight;
			return true;
		}

		return false;
	}

	bool GUIFloatField::mouseEvent(const GUIMouseEvent& event)
	{
		GUIElementContainer::_mouseEvent(event);

		Rect2I draggableArea;

		if(mLabel != nullptr)
			draggableArea = mLabel->_getLayoutData().area;

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

	void GUIFloatField::setValue(float value)
	{
		mValue = value;

		// Only update with new value if it actually changed, otherwise
		// problems can occur when user types in "0." and the field
		// updates back to "0" effectively making "." unusable
		float curValue = parseFloat(mInputBox->getText());
		if (mValue != curValue)
			mInputBox->setText(toWString(mValue));
	}

	void GUIFloatField::setTint(const Color& color)
	{
		if (mLabel != nullptr)
			mLabel->setTint(color);

		mInputBox->setTint(color);
	}

	void GUIFloatField::updateClippedBounds()
	{
		mClippedBounds = mLayoutData.area;
	}

	const String& GUIFloatField::getGUITypeName()
	{
		static String typeName = "GUIFloatField";
		return typeName;
	}

	const String& GUIFloatField::getInputStyleType()
	{
		static String LABEL_STYLE_TYPE = "EditorFieldInput";
		return LABEL_STYLE_TYPE;
	}

	void GUIFloatField::styleUpdated()
	{
		if (mLabel != nullptr)
			mLabel->setStyle(getSubStyleName(getLabelStyleType()));

		mInputBox->setStyle(getSubStyleName(getInputStyleType()));
	}

	void GUIFloatField::valueChanged(const WString& newValue)
	{
		float newFloatValue = parseFloat(newValue);

		CmdInputFieldValueChange<GUIFloatField, float>::execute(this, newFloatValue);

		if (!onValueChanged.empty())
			onValueChanged(newFloatValue);
	}

	void GUIFloatField::focusGained()
	{
		UndoRedo::instance().pushGroup("InputBox");
		mHasInputFocus = true;
	}

	void GUIFloatField::focusLost()
	{
		UndoRedo::instance().popGroup("InputBox");
		mHasInputFocus = false;
	}

	bool GUIFloatField::floatFilter(const WString& str)
	{
		return std::regex_match(str, std::wregex(L"-?(\\d+(\\.\\d*)?)?"));
	}
}
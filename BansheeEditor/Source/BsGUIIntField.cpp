#include "BsGUIIntField.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIInputBox.h"
#include "BsGUISpace.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"
#include "BsCursor.h"
#include "BsUndoRedo.h"
#include "BsViewport.h"
#include "BsCmdInputFieldValueChange.h"
#include <regex>

using namespace std::placeholders;

namespace BansheeEngine
{
	const INT32 GUIIntField::DRAG_SPEED = 5;

	GUIIntField::GUIIntField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
		const String& style, const GUIDimensions& dimensions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, style, dimensions, withLabel), mInputBox(nullptr), mIsDragging(false),
		mLastDragPos(0), mIsDragCursorSet(false), mHasInputFocus(false), mMinValue(std::numeric_limits<INT32>::lowest()), 
		mMaxValue(std::numeric_limits<INT32>::max())
	{
		mInputBox = GUIInputBox::create(false, GUIOptions(GUIOption::flexibleWidth()), getSubStyleName(getInputStyleType()));
		mInputBox->setFilter(&GUIIntField::intFilter);

		mInputBox->onValueChanged.connect(std::bind(&GUIIntField::valueChanged, this, _1));
		mInputBox->onFocusGained.connect(std::bind(&GUIIntField::focusGained, this));
		mInputBox->onFocusLost.connect(std::bind(&GUIIntField::focusLost, this));

		mLayout->addElement(mInputBox);

		setValue(0);
		mInputBox->setText(L"0");
	}

	GUIIntField::~GUIIntField()
	{

	}

	bool GUIIntField::_hasCustomCursor(const Vector2I position, CursorType& type) const
	{
		Rect2I draggableArea;

		if(mLabel != nullptr)
			draggableArea = mLabel->_getCachedBounds();

		if(draggableArea.contains(position))
		{
			type = CursorType::ArrowLeftRight;
			return true;
		}

		return false;
	}

	bool GUIIntField::mouseEvent(const GUIMouseEvent& event)
	{
		GUIElementContainer::_mouseEvent(event);

		Rect2I draggableArea;

		if(mLabel != nullptr)
			draggableArea = mLabel->_getCachedBounds();

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

			return true;
		}

		return false;
	}

	void GUIIntField::styleUpdated()
	{
		if (mLabel != nullptr)
			mLabel->setStyle(getSubStyleName(getLabelStyleType()));

		mInputBox->setStyle(getSubStyleName(getInputStyleType()));
	}

	void GUIIntField::setValue(INT32 value)
	{
		mValue = Math::clamp(value, mMinValue, mMaxValue);

		// Only update with new value if it actually changed, otherwise
		// problems can occur when user types in "0." and the field
		// updates back to "0" effectively making "." unusable
		float curValue = parseFloat(mInputBox->getText());
		if (mValue != curValue)
			mInputBox->setText(toWString(mValue));
	}

	void GUIIntField::setRange(INT32 min, INT32 max)
	{
		mMinValue = min;
		mMaxValue = max;
	}

	void GUIIntField::setTint(const Color& color)
	{
		if (mLabel != nullptr)
			mLabel->setTint(color);

		mInputBox->setTint(color);
	}

	void GUIIntField::updateClippedBounds()
	{
		Vector2I offset = _getOffset();
		mClippedBounds = Rect2I(offset.x, offset.y, _getWidth(), _getHeight());
	}

	const String& GUIIntField::getGUITypeName()
	{
		static String typeName = "GUIIntField";
		return typeName;
	}

	const String& GUIIntField::getInputStyleType()
	{
		static String LABEL_STYLE_TYPE = "EditorFieldInput";
		return LABEL_STYLE_TYPE;
	}

	void GUIIntField::valueChanged(const WString& newValue)
	{
		INT32 newIntValue = parseInt(newValue);

		CmdInputFieldValueChange<GUIIntField, INT32>::execute(this, newIntValue);

		if(!onValueChanged.empty())
			onValueChanged(newIntValue);
	}

	void GUIIntField::focusGained()
	{
		UndoRedo::instance().pushGroup("InputBox");
		mHasInputFocus = true;
	}

	void GUIIntField::focusLost()
	{
		UndoRedo::instance().popGroup("InputBox");
		mHasInputFocus = false;
	}

	bool GUIIntField::intFilter(const WString& str)
	{
		return std::regex_match(str, std::wregex(L"-?(\\d+)?"));
	}
}
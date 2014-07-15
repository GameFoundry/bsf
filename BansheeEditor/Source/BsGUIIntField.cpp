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
#include "BsUndoRedo.h"
#include "BsViewport.h"
#include "BsCmdInputFieldValueChange.h"
#include <regex>

using namespace std::placeholders;

namespace BansheeEngine
{
	const INT32 GUIIntField::DRAG_SPEED = 5;

	GUIIntField::GUIIntField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
		const String& labelStyle, const String& inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, labelStyle, layoutOptions, withLabel), mInputBox(nullptr), mIsDragging(false),
		mLastDragPos(0), mIsDragCursorSet(false)
	{
		mInputBox = GUIInputBox::create(false, GUIOptions(GUIOption::flexibleWidth()), inputBoxStyle);
		mInputBox->setFilter(&GUIIntField::intFilter);

		mInputBox->onValueChanged.connect(std::bind(&GUIIntField::valueChanged, this, _1));
		mInputBox->onFocusGained.connect(std::bind(&GUIIntField::focusGained, this));
		mInputBox->onFocusLost.connect(std::bind(&GUIIntField::focusLost, this));

		mLayout->addElement(mInputBox);

		setValue(0);
	}

	GUIIntField::~GUIIntField()
	{

	}

	Vector2I GUIIntField::_getOptimalSize() const
	{
		UINT32 width = (UINT32)mInputBox->_getOptimalSize().x;
		UINT32 height = (UINT32)mInputBox->_getOptimalSize().y;

		if (mLabel != nullptr)
		{
			width += mLabel->_getOptimalSize().x;
			height = std::max(height, (UINT32)mLabel->_getOptimalSize().y);
		}

		return Vector2I(width, height);
	}

	bool GUIIntField::_hasCustomCursor(const Vector2I position, CursorType& type) const
	{
		RectI draggableArea;

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
		GUIElementContainer::mouseEvent(event);

		RectI draggableArea;

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

	void GUIIntField::setValue(INT32 value)
	{
		mValue = value;
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
	}

	void GUIIntField::focusLost()
	{
		UndoRedo::instance().popGroup("InputBox");
	}

	bool GUIIntField::intFilter(const WString& str)
	{
		return std::regex_match(str, std::wregex(L"-?(\\d+)?"));
	}
}
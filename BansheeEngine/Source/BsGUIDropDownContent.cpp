#include "BsGUIDropDownContent.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIButton.h"
#include "BsGUILabel.h"
#include "BsGUISpace.h"
#include "BsGUIWidget.h"
#include "BsGUIToggle.h"
#include "BsGUISkin.h"
#include "BsGUIMouseEvent.h"
#include "BsGUICommandEvent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const String GUIDropDownContent::ENTRY_STYLE_TYPE = "DropDownEntryBtn";
	const String GUIDropDownContent::ENTRY_EXP_STYLE_TYPE = "DropDownEntryExpBtn";
	const String GUIDropDownContent::SEPARATOR_STYLE_TYPE = "DropDownSeparator";

	GUIDropDownContent::GUIDropDownContent(GUIDropDownBox::DropDownSubMenu* parent, const GUIDropDownData& dropDownData, 
		const String& style, const GUIDimensions& dimensions)
		:GUIElementContainer(dimensions, style), mDropDownData(dropDownData),
		mSelectedIdx(UINT_MAX), mRangeStart(0), mRangeEnd(0), mParent(parent)
	{
		
	}

	GUIDropDownContent::~GUIDropDownContent()
	{

	}

	GUIDropDownContent* GUIDropDownContent::create(GUIDropDownBox::DropDownSubMenu* parent, 
		const GUIDropDownData& dropDownData, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIDropDownContent::getGUITypeName();

		return new (bs_alloc<GUIDropDownContent>()) GUIDropDownContent(parent, dropDownData, *curStyle, GUIDimensions::create());
	}

	GUIDropDownContent* GUIDropDownContent::create(GUIDropDownBox::DropDownSubMenu* parent, 
		const GUIDropDownData& dropDownData, const GUIOptions& options,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIDropDownContent::getGUITypeName();

		return new (bs_alloc<GUIDropDownContent>()) GUIDropDownContent(parent, dropDownData, *curStyle, GUIDimensions::create(options));
	}

	void GUIDropDownContent::styleUpdated()
	{
		for (auto& visElem : mVisibleElements)
		{
			GUIDropDownDataEntry& element = mDropDownData.entries[visElem.idx];

			if (element.isSeparator())
				visElem.separator->setStyle(getSubStyleName(SEPARATOR_STYLE_TYPE));
			else if (element.isSubMenu())
				visElem.button->setStyle(getSubStyleName(ENTRY_EXP_STYLE_TYPE));
			else
				visElem.button->setStyle(getSubStyleName(ENTRY_STYLE_TYPE));
		}
	}

	void GUIDropDownContent::setRange(UINT32 start, UINT32 end)
	{
		std::function<void(UINT32, UINT32)> onHover = 
			[&](UINT32 idx, UINT32 visIdx) 
		{ 
			setSelected(visIdx);
			mParent->elementSelected(idx); 
		};

		std::function<void(UINT32, UINT32)> onClick =
			[&](UINT32 idx, UINT32 visIdx)
		{ 
			setSelected(visIdx);
			mParent->elementActivated(idx, mVisibleElements[visIdx].button->_getCachedBounds());
		};

		// Remove all elements
		while (_getNumChildren() > 0)
		{
			GUIElementBase* child = _getChild(_getNumChildren() - 1);
			assert(child->_getType() == GUIElementBase::Type::Element);

			GUIElement::destroy(static_cast<GUIElement*>(child));
		}

		mRangeStart = start;
		mRangeEnd = end;
		
		mVisibleElements.clear();
		UINT32 curVisIdx = 0;
		for (UINT32 i = start; i < end; i++)
		{
			mVisibleElements.push_back(VisibleElement());
			VisibleElement& visElem = mVisibleElements.back();
			visElem.idx = i;
			GUIDropDownDataEntry& element = mDropDownData.entries[i];

			if (element.isSeparator())
			{
				visElem.separator = GUITexture::create(GUIImageScaleMode::StretchToFit, getSubStyleName(SEPARATOR_STYLE_TYPE));
				_registerChildElement(visElem.separator);
			}
			else if (element.isSubMenu())
			{
				visElem.button = GUIButton::create(getElementLocalizedName(i), getSubStyleName(ENTRY_EXP_STYLE_TYPE));
				visElem.button->onHover.connect(std::bind(onClick, i, curVisIdx));
				_registerChildElement(visElem.button);
			}
			else
			{
				visElem.button = GUIButton::create(getElementLocalizedName(i), getSubStyleName(ENTRY_STYLE_TYPE));
				visElem.button->onHover.connect(std::bind(onHover, i, curVisIdx));
				visElem.button->onClick.connect(std::bind(onClick, i, curVisIdx));
				_registerChildElement(visElem.button);

				const WString& shortcutTag = element.getShortcutTag();
				if (!shortcutTag.empty())
				{
					visElem.shortcutLabel = GUILabel::create(HString(shortcutTag), "RightAlignedLabel");
					_registerChildElement(visElem.shortcutLabel);
				}
			}

			curVisIdx++;
		}

		markContentAsDirty();
	}

	UINT32 GUIDropDownContent::getElementHeight(UINT32 idx) const
	{
		if (_getParentWidget() == nullptr)
			return 14; // Arbitrary

		if (mDropDownData.entries[idx].isSeparator())
			return _getParentWidget()->getSkin().getStyle(getSubStyleName(SEPARATOR_STYLE_TYPE))->height;
		else if (mDropDownData.entries[idx].isSubMenu())
			return _getParentWidget()->getSkin().getStyle(getSubStyleName(ENTRY_EXP_STYLE_TYPE))->height;
		else
			return _getParentWidget()->getSkin().getStyle(getSubStyleName(ENTRY_STYLE_TYPE))->height;
	}

	HString GUIDropDownContent::getElementLocalizedName(UINT32 idx) const
	{
		const WString& label = mDropDownData.entries[idx].getLabel();

		auto findLocalizedName = mDropDownData.localizedNames.find(label);
		if (findLocalizedName != mDropDownData.localizedNames.end())
			return findLocalizedName->second;
		else
			return HString(label);
	}

	void GUIDropDownContent::setKeyboardFocus(bool focus) 
	{ 
		mKeyboardFocus = focus; 
		setFocus(focus);
	}

	bool GUIDropDownContent::_commandEvent(const GUICommandEvent& ev)
	{
		if (!mKeyboardFocus)
			return false;

		UINT32 maxElemIdx = (UINT32)mDropDownData.entries.size();

		switch (ev.getType())
		{
		case GUICommandEventType::MoveDown:
			if (mSelectedIdx == UINT_MAX)
				selectNext(0);
			else
				selectNext(mSelectedIdx + 1);
			return true;
		case GUICommandEventType::MoveUp:
			if (mSelectedIdx == UINT_MAX)
				selectNext(0);
			else
				selectPrevious(mSelectedIdx - 1);
			return true;
		case GUICommandEventType::Escape:
		case GUICommandEventType::MoveLeft:
			mParent->close();
			return true;
		case GUICommandEventType::MoveRight:
		{
			if (mSelectedIdx == UINT_MAX)
				selectNext(0);
			else
			{
				GUIDropDownDataEntry& entry = mDropDownData.entries[mVisibleElements[mSelectedIdx].idx];
				if (entry.isSubMenu())
					mParent->elementActivated(mVisibleElements[mSelectedIdx].idx, mVisibleElements[mSelectedIdx].button->_getCachedBounds());
			}
		}
			return true;
		case GUICommandEventType::Return:
			if (mSelectedIdx == UINT_MAX)
				selectNext(0);
			else
				mParent->elementActivated(mVisibleElements[mSelectedIdx].idx, mVisibleElements[mSelectedIdx].button->_getCachedBounds());
			return true;
		}

		return false;
	}

	void GUIDropDownContent::setSelected(UINT32 idx)
	{
		if (mSelectedIdx != UINT_MAX)
			mVisibleElements[mSelectedIdx].button->_setOn(false);

		mSelectedIdx = idx;
		mVisibleElements[mSelectedIdx].button->_setOn(true);

		mParent->elementSelected(mVisibleElements[mSelectedIdx].idx);
	}

	void GUIDropDownContent::selectNext(UINT32 startIdx)
	{
		UINT32 numElements = (UINT32)mDropDownData.entries.size();

		bool gotNextIndex = false;
		UINT32 nextIdx = startIdx;
		for (UINT32 i = 0; i < numElements; i++)
		{
			if (nextIdx >= numElements)
				nextIdx = 0; // Wrap around

			GUIDropDownDataEntry& entry = mDropDownData.entries[nextIdx];
			if (!entry.isSeparator())
			{
				gotNextIndex = true;
				break;
			}

			nextIdx++;
		}

		if (gotNextIndex)
		{
			while (nextIdx < mRangeStart || nextIdx >= mRangeEnd)
				mParent->scrollDown();

			UINT32 visIdx = 0;
			for (auto& visElem : mVisibleElements)
			{
				if (visElem.idx == nextIdx)
				{
					setSelected(visIdx);
					break;
				}

				visIdx++;
			}
		}
	}

	void GUIDropDownContent::selectPrevious(UINT32 startIdx)
	{
		UINT32 numElements = (UINT32)mDropDownData.entries.size();

		bool gotNextIndex = false;
		INT32 prevIdx = (INT32)startIdx;

		for (UINT32 i = 0; i < numElements; i++)
		{
			if (prevIdx < 0)
				prevIdx = numElements - 1; // Wrap around

			GUIDropDownDataEntry& entry = mDropDownData.entries[prevIdx];
			if (!entry.isSeparator())
			{
				gotNextIndex = true;
				break;
			}

			prevIdx--;
		}

		if (gotNextIndex)
		{
			while (prevIdx < (INT32)mRangeStart || prevIdx >= (INT32)mRangeEnd)
				mParent->scrollUp();

			UINT32 visIdx = 0;
			for (auto& visElem : mVisibleElements)
			{
				if (visElem.idx == prevIdx)
				{
					setSelected(visIdx);
					break;
				}

				visIdx++;
			}
		}
	}

	Vector2I GUIDropDownContent::_getOptimalSize() const
	{
		Vector2I optimalSize;
		for (auto& visElem : mVisibleElements)
		{
			const GUIDropDownDataEntry& element = mDropDownData.entries[visElem.idx];

			optimalSize.y += (INT32)getElementHeight(visElem.idx);

			if (element.isSeparator())
				optimalSize.x = std::max(optimalSize.x, visElem.separator->_getOptimalSize().x);
			else
				optimalSize.x = std::max(optimalSize.x, visElem.button->_getOptimalSize().x);
		}

		return optimalSize;
	}

	void GUIDropDownContent::updateClippedBounds()
	{
		Vector2I offset = _getOffset();
		mClippedBounds = Rect2I(offset.x, offset.y, _getWidth(), _getHeight());

		Rect2I localClipRect(mClipRect.x + mOffset.x, mClipRect.y + mOffset.y, mClipRect.width, mClipRect.height);
		mClippedBounds.clip(localClipRect);
	}

	void GUIDropDownContent::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		INT32 yOffset = y;
		for (auto& visElem : mVisibleElements)
		{
			const GUIDropDownDataEntry& element = mDropDownData.entries[visElem.idx];

			GUIElement* guiMainElement = nullptr;
			if (element.isSeparator())
				guiMainElement = visElem.separator;
			else
				guiMainElement = visElem.button;

			UINT32 elemHeight = getElementHeight(visElem.idx);
			Vector2I offset(x, yOffset);
			yOffset += elemHeight;

			guiMainElement->_setPosition(offset);
			guiMainElement->_setWidth(width);
			guiMainElement->_setHeight(elemHeight);
			guiMainElement->_setAreaDepth(areaDepth);
			guiMainElement->_setWidgetDepth(widgetDepth);

			Rect2I elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			guiMainElement->_setClipRect(elemClipRect);

			// Shortcut label
			GUILabel* shortcutLabel = visElem.shortcutLabel;
			if (shortcutLabel != nullptr)
			{
				shortcutLabel->_setPosition(offset);
				shortcutLabel->_setWidth(width);
				shortcutLabel->_setHeight(elemHeight);
				shortcutLabel->_setAreaDepth(areaDepth);
				shortcutLabel->_setWidgetDepth(widgetDepth);
				shortcutLabel->_setClipRect(elemClipRect);
			}
		}
	}

	const String& GUIDropDownContent::getGUITypeName()
	{
		static String typeName = "GUIDropDownContent";
		return typeName;
	}
}
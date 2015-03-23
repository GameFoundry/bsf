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
		const String& style, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions, style), mDropDownData(dropDownData), 
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

		return new (bs_alloc<GUIDropDownContent>()) GUIDropDownContent(parent, dropDownData, *curStyle, GUILayoutOptions::create());
	}

	GUIDropDownContent* GUIDropDownContent::create(GUIDropDownBox::DropDownSubMenu* parent, 
		const GUIDropDownData& dropDownData, const GUIOptions& layoutOptions,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIDropDownContent::getGUITypeName();

		return new (bs_alloc<GUIDropDownContent>()) GUIDropDownContent(parent, dropDownData, *curStyle, GUILayoutOptions::create(layoutOptions));
	}

	void GUIDropDownContent::styleUpdated()
	{
		for (auto& visElem : mVisibleElements)
		{
			GUIDropDownDataEntry& element = mDropDownData.entries[visElem.idx];

			if (element.isSeparator())
				visElem.separator->setStyle(getSubStyleName(SEPARATOR_STYLE_TYPE));
			else if (element.isSubMenu())
				visElem.separator->setStyle(getSubStyleName(ENTRY_EXP_STYLE_TYPE));
			else
				visElem.separator->setStyle(getSubStyleName(ENTRY_STYLE_TYPE));
		}
	}

	void GUIDropDownContent::setRange(UINT32 start, UINT32 end)
	{
		std::function<void(UINT32, UINT32)> onHover = 
			[&](UINT32 idx, UINT32 visIdx) 
		{ 
			mSelectedIdx = visIdx; 
			mParent->elementSelected(idx, mVisibleElements[visIdx].button->_getCachedBounds()); 
		};

		std::function<void(UINT32)> onClick = [&](UINT32 idx) { mParent->elementActivated(idx); };

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
				visElem.button->onHover.connect(std::bind(onHover, i, curVisIdx));
				_registerChildElement(visElem.button);
			}
			else
			{
				visElem.button = GUIButton::create(getElementLocalizedName(i), getSubStyleName(ENTRY_STYLE_TYPE));
				visElem.button->onHover.connect(std::bind(onHover, i, curVisIdx));
				visElem.button->onClick.connect(std::bind(onClick, i));
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

		if (mSelectedIdx == UINT_MAX)
			setSelected(0);
	}

	UINT32 GUIDropDownContent::getElementHeight(UINT32 idx) const
	{
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

	bool GUIDropDownContent::_commandEvent(const GUICommandEvent& ev)
	{
		if (!mKeyboardFocus)
			return false;

		UINT32 maxElemIdx = (UINT32)mDropDownData.entries.size() - 1;

		switch (ev.getType())
		{
		case GUICommandEventType::MoveDown:
		{
			{
				UINT32 curIdx = mVisibleElements[mSelectedIdx].idx;
				UINT32 nextIdx = curIdx;
				bool gotNextIndex = false;

				for (nextIdx = curIdx + 1; nextIdx <= maxElemIdx; nextIdx++)
				{
					GUIDropDownDataEntry& entry = mDropDownData.entries[nextIdx];
					if (!entry.isSeparator())
					{
						gotNextIndex = true;
						break;
					}
				}

				if (gotNextIndex)
				{
					while (nextIdx > mRangeEnd)
					{
						if (maxElemIdx == mRangeEnd)
							break;

						mParent->scrollDown();
					}

					if (nextIdx <= mRangeEnd)
					{
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
			}
		}
			return true;
		case GUICommandEventType::MoveUp:
		{
			{
				UINT32 curIdx = mVisibleElements[mSelectedIdx].idx;
				INT32 prevIdx = curIdx;
				bool gotNextIndex = false;

				for (prevIdx = (INT32)curIdx - 1; prevIdx >= 0; prevIdx--)
				{
					GUIDropDownDataEntry& entry = mDropDownData.entries[prevIdx];
					if (!entry.isSeparator())
					{
						gotNextIndex = true;
						break;
					}
				}

				if (gotNextIndex)
				{
					while (prevIdx < (INT32)mRangeStart)
					{
						if (mRangeStart == 0)
							break;

						mParent->scrollUp();
					}

					if (prevIdx >= (INT32)mRangeStart)
					{
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
			}
		}
			return true;
		case GUICommandEventType::Escape:
		case GUICommandEventType::MoveLeft:
			mParent->close();
			return true;
		case GUICommandEventType::MoveRight:
		{
			GUIDropDownDataEntry& entry = mDropDownData.entries[mVisibleElements[mSelectedIdx].idx];
			if (entry.isSubMenu())
				mParent->elementActivated(mVisibleElements[mSelectedIdx].idx);
		}
			return true;
		case GUICommandEventType::Return:
			mParent->elementActivated(mVisibleElements[mSelectedIdx].idx);
			return true;
		}

		// TODO - When closing a sub-menu I might need to also call GUIDropDownBoxManager::instance().closeDropDownBox();
		// TODO - I should be able to set focus to this element, so only the most recent element receives keyboard input

		return false;
	}

	void GUIDropDownContent::setSelected(UINT32 idx)
	{
		if (mSelectedIdx != UINT_MAX)
			mVisibleElements[mSelectedIdx].button->_setOn(false);

		mSelectedIdx = idx;
		mVisibleElements[mSelectedIdx].button->_setOn(true);

		mParent->elementSelected(mVisibleElements[mSelectedIdx].idx, mVisibleElements[mSelectedIdx].button->_getCachedBounds());
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
		INT32 yOffset = 0;
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

			guiMainElement->setOffset(offset);
			guiMainElement->setWidth(width);
			guiMainElement->setHeight(elemHeight);
			guiMainElement->_setAreaDepth(areaDepth);
			guiMainElement->_setWidgetDepth(widgetDepth);

			Rect2I elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			guiMainElement->_setClipRect(elemClipRect);

			// Shortcut label
			GUILabel* shortcutLabel = visElem.shortcutLabel;
			if (shortcutLabel != nullptr)
			{
				shortcutLabel->setOffset(offset);
				shortcutLabel->setWidth(width);
				shortcutLabel->setHeight(elemHeight);
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
#include "BsGUIListBox.h"
#include "BsImageSprite.h"
#include "BsCGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUIDimensions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIManager.h"
#include "BsGUIHelper.h"
#include "BsGUIDropDownBoxManager.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	const String& GUIListBox::getGUITypeName()
	{
		static String name = "ListBox";
		return name;
	}

	GUIListBox::GUIListBox(const String& styleName, const Vector<HString>& elements, bool isMultiselect, const GUIDimensions& dimensions)
		:GUIButtonBase(styleName, GUIContent(HString(L"")), dimensions), mElements(elements), mIsMultiselect(isMultiselect)
	{
		mElementStates.resize(elements.size(), false);
		if (!mIsMultiselect && mElementStates.size() > 0)
			mElementStates[0] = true;

		updateContents();
	}

	GUIListBox::~GUIListBox()
	{
		closeListBox();
	}

	GUIListBox* GUIListBox::create(const Vector<HString>& elements, bool isMultiselect, const String& styleName)
	{
		return new (bs_alloc<GUIListBox>()) GUIListBox(getStyleName<GUIListBox>(styleName), elements, isMultiselect, GUIDimensions::create());
	}

	GUIListBox* GUIListBox::create(const Vector<HString>& elements, bool isMultiselect, const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUIListBox>()) GUIListBox(getStyleName<GUIListBox>(styleName), elements, isMultiselect, GUIDimensions::create(options));
	}

	GUIListBox* GUIListBox::create(const Vector<HString>& elements, const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUIListBox>()) GUIListBox(getStyleName<GUIListBox>(styleName), elements, false, GUIDimensions::create(options));
	}

	void GUIListBox::setElements(const Vector<HString>& elements)
	{
		bool wasOpen = mDropDownBox != nullptr;

		if(wasOpen)
			closeListBox();

		mElements = elements;

		mElementStates.clear();
		mElementStates.resize(mElements.size(), false);
		if (!mIsMultiselect && mElementStates.size() > 0)
			mElementStates[0] = true;

		updateContents();

		if(wasOpen)
			openListBox();
	}

	void GUIListBox::selectElement(UINT32 idx)
	{
		if (idx >= (UINT32)mElements.size())
			return;

		if (mElementStates[idx] != true)
			elementSelected(idx);
	}

	void GUIListBox::deselectElement(UINT32 idx)
	{
		if (!mIsMultiselect || idx >= (UINT32)mElements.size())
			return;

		if (mElementStates[idx] != false)
			elementSelected(idx);
	}

	void GUIListBox::setElementStates(const Vector<bool>& states)
	{
		bool wasOpen = mDropDownBox != nullptr;

		if (wasOpen)
			closeListBox();

		UINT32 numElements = (UINT32)mElementStates.size();
		UINT32 min = std::min(numElements, (UINT32)states.size());

		for (UINT32 i = 0; i < min; i++)
		{
			mElementStates[i] = states[i];

			if (mElementStates[i] && !mIsMultiselect)
			{
				for (UINT32 j = i + 1; j < numElements; j++)
					mElementStates[j] = false;

				break;
			}
		}

		updateContents();

		if (wasOpen)
			openListBox();
	}

	bool GUIListBox::_mouseEvent(const GUIMouseEvent& ev)
	{
		bool processed = GUIButtonBase::_mouseEvent(ev);

		if(ev.getType() == GUIMouseEventType::MouseDown)
		{
			if (mDropDownBox == nullptr)
				openListBox();
			else
				closeListBox();

			processed = true;
		}

		return processed;
	}

	void GUIListBox::elementSelected(UINT32 idx)
	{
		if (idx >= (UINT32)mElements.size())
			return;

		if (mIsMultiselect)
		{
			bool selected = mElementStates[idx];
			mElementStates[idx] = !selected;

			if (!onSelectionToggled.empty())
				onSelectionToggled(idx, !selected);
		}
		else
		{
			for (UINT32 i = 0; i < (UINT32)mElementStates.size(); i++)
				mElementStates[i] = false;

			mElementStates[idx] = true;

			if (!onSelectionToggled.empty())
				onSelectionToggled(idx, true);

			closeListBox();
		}

		updateContents();
	}

	void GUIListBox::openListBox()
	{
		closeListBox();

		DROP_DOWN_BOX_DESC desc;

		UINT32 i = 0;
		for(auto& elem : mElements)
		{
			WString identifier = toWString(i);
			desc.dropDownData.entries.push_back(GUIDropDownDataEntry::button(identifier, std::bind(&GUIListBox::elementSelected, this, i)));
			desc.dropDownData.localizedNames[identifier] = elem;
			i++;
		}

		CGUIWidget* widget = _getParentWidget();

		desc.camera = widget->getCamera();
		desc.skin = widget->getSkinResource();
		desc.placement = DropDownAreaPlacement::aroundBoundsHorz(mClippedBounds);
		desc.dropDownData.states = mElementStates;

		GUIDropDownType type;
		if (mIsMultiselect)
			type = GUIDropDownType::MultiListBox;
		else
			type = GUIDropDownType::ListBox;

		mDropDownBox = GUIDropDownBoxManager::instance().openDropDownBox(
			desc, type, std::bind(&GUIListBox::onListBoxClosed, this));

		_setOn(true);
	}

	void GUIListBox::closeListBox()
	{
		if (mDropDownBox != nullptr)
		{
			GUIDropDownBoxManager::instance().closeDropDownBox();

			_setOn(false);
			mDropDownBox = nullptr;
		}
	}

	void GUIListBox::updateContents()
	{
		UINT32 selectedIdx = 0;
		UINT32 numSelected = 0;
		for (UINT32 i = 0; i < (UINT32)mElementStates.size(); i++)
		{
			if (mElementStates[i])
			{
				selectedIdx = i;
				numSelected++;
			}
		}

		if (mIsMultiselect)
		{
			if (numSelected == 1)
				setContent(GUIContent(mElements[selectedIdx]));
			else if (numSelected == 0)
				setContent(GUIContent(HEString(L"None")));
			else
				setContent(GUIContent(HEString(L"Multiple")));
		}
		else
		{
			setContent(GUIContent(mElements[selectedIdx]));
		}
	}

	void GUIListBox::onListBoxClosed()
	{
		_setOn(false);
		mDropDownBox = nullptr;
	}
}
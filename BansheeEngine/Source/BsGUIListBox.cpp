#include "BsGUIListBox.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIManager.h"
#include "BsGUIHelper.h"
#include "BsGUIDropDownBoxManager.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIListBox::getGUITypeName()
	{
		static String name = "ListBox";
		return name;
	}

	GUIListBox::GUIListBox(GUIWidget& parent, const GUIElementStyle* style, const Vector<HString>::type& elements, const GUILayoutOptions& layoutOptions)
		:GUIButtonBase(parent, style, GUIContent(HString(L"")), layoutOptions), mElements(elements), mSelectedIdx(0), mIsListBoxOpen(false)
	{
		if(elements.size() > 0)
			setContent(GUIContent(mElements[mSelectedIdx]));
	}

	GUIListBox::~GUIListBox()
	{
		closeListBox();
	}

	GUIListBox* GUIListBox::create(GUIWidget& parent, const Vector<HString>::type& elements, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIListBox, PoolAlloc>()) GUIListBox(parent, style, elements, GUILayoutOptions::create(style));
	}

	GUIListBox* GUIListBox::create(GUIWidget& parent, const Vector<HString>::type& elements, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIListBox, PoolAlloc>()) GUIListBox(parent, style, elements, GUILayoutOptions::create(layoutOptions, style));
	}

	bool GUIListBox::mouseEvent(const GUIMouseEvent& ev)
	{
		bool processed = GUIButtonBase::mouseEvent(ev);

		if(ev.getType() == GUIMouseEventType::MouseDown)
		{
			if(!mIsListBoxOpen)
				openListBox();
			else
				closeListBox();

			processed = true;
		}

		return processed;
	}

	void GUIListBox::elementSelected(CM::UINT32 idx)
	{
		if(!onSelectionChanged.empty())
			onSelectionChanged(idx);

		mSelectedIdx = idx;
		setContent(GUIContent(mElements[idx]));

		closeListBox();
	}

	void GUIListBox::openListBox()
	{
		closeListBox();

		GUIDropDownData dropDownData;
		UINT32 i = 0;
		for(auto& elem : mElements)
		{
			WString identifier = toWString(i);
			dropDownData.entries.push_back(GUIDropDownDataEntry::button(identifier, boost::bind(&GUIListBox::elementSelected, this, i)));
			dropDownData.localizedNames[identifier] = elem;
			i++;
		}

		GUIWidget& widget = _getParentWidget();
		GUIDropDownAreaPlacement placement = GUIDropDownAreaPlacement::aroundBoundsHorz(getBounds());

		GameObjectHandle<GUIDropDownBox> dropDownBox = GUIDropDownBoxManager::instance().openDropDownBox(widget.getTarget(), widget.getOwnerWindow(), 
			placement, dropDownData, widget.getSkin(), GUIDropDownType::MenuBar, boost::bind(&GUIListBox::onListBoxClosed, this));

		GUIManager::instance().enableSelectiveInput(boost::bind(&GUIListBox::closeListBox, this));
		GUIManager::instance().addSelectiveInputWidget(dropDownBox.get());
		GUIManager::instance().addSelectiveInputElement(this);

		_setOn(true);
		mIsListBoxOpen = true;
	}

	void GUIListBox::closeListBox()
	{
		if(mIsListBoxOpen)
		{
			GUIDropDownBoxManager::instance().closeDropDownBox();
			GUIManager::instance().disableSelectiveInput();

			_setOn(false);
			mIsListBoxOpen = false;
		}
	}

	void GUIListBox::onListBoxClosed()
	{
		GUIManager::instance().disableSelectiveInput();

		_setOn(false);
		mIsListBoxOpen = false;
	}
}
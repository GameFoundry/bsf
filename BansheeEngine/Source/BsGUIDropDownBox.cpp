#include "BsGUIDropDownBox.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIButton.h"
#include "BsGUIContent.h"
#include "BsGUISkin.h"
#include "CmViewport.h"
#include "BsGUIListBox.h"
#include "CmSceneObject.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const UINT32 GUIDropDownBox::DROP_DOWN_BOX_WIDTH = 150;

	GUIDropDownData GUIDropDownData::separator()
	{
		GUIDropDownData data;
		data.mType = Type::Separator;
		data.mCallback = nullptr;

		return data;
	}

	GUIDropDownData GUIDropDownData::button(const WString& label, std::function<void()> callback)
	{
		GUIDropDownData data;
		data.mLabel = label;
		data.mType = Type::Entry;
		data.mCallback = callback;

		return data;
	}

	GUIDropDownData GUIDropDownData::subMenu(const WString& label, const Vector<GUIDropDownData>::type& entries)
	{
		GUIDropDownData data;
		data.mLabel = label;
		data.mType = Type::SubMenu;
		data.mChildEntries = entries;

		return data;
	}

	GUIDropDownBox::GUIDropDownBox(const HSceneObject& parent)
		:GUIWidget(parent), mPage(0), mBackgroundFrame(nullptr), mScrollUpStyle(nullptr),
		mScrollDownStyle(nullptr), mEntryBtnStyle(nullptr), mEntryExpBtnStyle(nullptr), 
		mSeparatorStyle(nullptr), mBackgroundStyle(nullptr), mBackgroundArea(nullptr), mContentArea(nullptr), 
		mContentLayout(nullptr), mScrollUpBtn(nullptr), mScrollDownBtn(nullptr), x(0), y(0), width(0), height(0), 
		mType(GUIDropDownType::ListBox)
	{

	}

	GUIDropDownBox::~GUIDropDownBox()
	{
		closeSubMenu();
	}

	void GUIDropDownBox::initialize(Viewport* target, RenderWindow* window, GUIElement* parentElem, 
		const CM::Vector<GUIDropDownData>::type& elements, const GUISkin& skin, GUIDropDownType type)
	{
		GUIWidget::initialize(target, window);

		String stylePrefix = "";
		switch(type)
		{
		case GUIDropDownType::ContextMenu:
			stylePrefix = "ContextMenu";
			break;
		case GUIDropDownType::ListBox:
			stylePrefix = "ListBox";
			break;
		case GUIDropDownType::MenuBar:
			stylePrefix = "MenuBar";
			break;
		}

		mType = type;
		mElements = elements;

		mScrollUpStyle = skin.getStyle(stylePrefix + "ScrollUpBtn");
		mScrollDownStyle = skin.getStyle(stylePrefix + "ScrollDownBtn");
		mEntryBtnStyle = skin.getStyle(stylePrefix + "EntryBtn");
		mEntryExpBtnStyle = skin.getStyle(stylePrefix + "EntryExpBtn");
		mSeparatorStyle = skin.getStyle(stylePrefix + "Separator");
		mBackgroundStyle = skin.getStyle(stylePrefix + "Frame");

		mScrollUpBtnArrow = skin.getStyle("ScrollUpBtnArrow")->normal.texture;
		mScrollDownBtnArrow = skin.getStyle("ScrollDownBtnArrow")->normal.texture;

		setDepth(0); // Needs to be in front of everything
		setSkin(skin);

		Rect dropDownListBounds = parentElem->getBounds();

		// Determine x position and whether to align to left or right side of the drop down list
		UINT32 availableRightwardWidth = (UINT32)std::max(0, (target->getLeft() + target->getWidth()) - dropDownListBounds.x);
		UINT32 availableLeftwardWidth = (UINT32)std::max(0, (dropDownListBounds.x + dropDownListBounds.width) - target->getLeft());

		//// Prefer right if possible
		if(DROP_DOWN_BOX_WIDTH <= availableRightwardWidth)
			x = dropDownListBounds.x;
		else
		{
			if(availableRightwardWidth >= availableLeftwardWidth)
				x = dropDownListBounds.x;
			else
				x = dropDownListBounds.x - std::min(DROP_DOWN_BOX_WIDTH, availableLeftwardWidth);
		}

		// Determine maximum width
		UINT32 maxPossibleWidth = (UINT32)std::max(0, (target->getLeft() + target->getWidth()) - x);
		width = std::min(DROP_DOWN_BOX_WIDTH, maxPossibleWidth);

		// Determine y position and whether to open upward or downward
		UINT32 helperElementHeight = mScrollUpStyle->height + mScrollDownStyle->height + mBackgroundStyle->margins.top + mBackgroundStyle->margins.bottom;

		UINT32 maxNeededHeight = helperElementHeight;
		UINT32 numElements = (UINT32)mElements.size();
		for(UINT32 i = 0; i < numElements; i++)
			maxNeededHeight += getElementHeight(i);

		UINT32 availableDownwardHeight = (UINT32)std::max(0, (target->getTop() + target->getHeight()) - (dropDownListBounds.y + dropDownListBounds.height));
		UINT32 availableUpwardHeight = (UINT32)std::max(0, dropDownListBounds.y - target->getTop());

		//// Prefer down if possible
		height = 0;
		if(maxNeededHeight <= availableDownwardHeight)
		{
			y = dropDownListBounds.y + dropDownListBounds.height;
			height = availableDownwardHeight;
		}
		else
		{
			if(availableDownwardHeight >= availableUpwardHeight)
			{
				y = dropDownListBounds.y + dropDownListBounds.height;
				height = availableDownwardHeight;
			}
			else
			{
				y = dropDownListBounds.y - std::min(maxNeededHeight, availableUpwardHeight);
				height = availableUpwardHeight;
			}
		}

		// Content area
		mContentArea = GUIArea::create(*this, x, y, width, height);
		mContentLayout = &mContentArea->getLayout().addLayoutY();

		// Background frame
		mBackgroundArea = GUIArea::create(*this, x, y, width, height);
		mBackgroundArea->setDepth(102);
		mBackgroundArea->getLayout().addElement(GUITexture::create(*this, GUIImageScaleMode::ScaleToFit, mBackgroundStyle));

		updateGUIElements();
	}

	void GUIDropDownBox::updateGUIElements()
	{
		// Remove all elements from content layout
		while(mContentLayout->getNumChildren() > 0)
			mContentLayout->removeChildAt(mContentLayout->getNumChildren() - 1);

		// Determine if we need scroll up and/or down buttons, number of visible elements and actual height
		bool needsScrollUp = mPage > 0;
		UINT32 numElements = (UINT32)mElements.size();

		UINT32 usedHeight = mBackgroundStyle->margins.top + mBackgroundStyle->margins.bottom;

		UINT32 pageStart = 0, pageEnd = 0;
		UINT32 curPage = 0;
		bool needsScrollDown = false;
		for(UINT32 i = 0; i < numElements; i++)
		{
			usedHeight += getElementHeight(i);
			pageEnd++;

			if(usedHeight > height)
			{
				usedHeight += mScrollDownStyle->height;
				needsScrollDown = true;

				// Remove last few elements until we fit again
				while(usedHeight > height && i >= 0)
				{
					usedHeight -= getElementHeight(i);
					pageEnd--;

					i--;
				}

				// We found our page and are done
				if(curPage == mPage)
					break;

				// Nothing fits, break out of infinite loop
				if(pageStart == pageEnd)
					break;

				pageStart = pageEnd;
				usedHeight = mBackgroundStyle->margins.top + mBackgroundStyle->margins.bottom;
				usedHeight += mScrollUpStyle->height;

				curPage++;
			}
		}

		// Add scroll up button
		if(needsScrollUp)
		{
			if(mScrollUpBtn == nullptr)
			{
				mScrollUpBtn = GUIButton::create(*this, GUIContent(L"", mScrollUpBtnArrow), mScrollUpStyle);
				mScrollUpBtn->onClick.connect(boost::bind(&GUIDropDownBox::scrollUp, this));
			}

			mContentLayout->addElement(mScrollUpBtn);			
		}
		else
		{
			if(mScrollUpBtn != nullptr)
			{
				GUIElement::destroy(mScrollUpBtn);
				mScrollUpBtn = nullptr;
			}
		}

		CM::Vector<GUITexture*>::type newSeparators;
		CM::Vector<GUIButton*>::type newEntryBtns;
		CM::Vector<GUIButton*>::type newExpEntryBtns;
		for(UINT32 i = pageStart; i < pageEnd; i++)
		{
			GUIDropDownData& element = mElements[i];

			if(element.isSeparator())
			{
				GUITexture* separator = nullptr;
				if(!mCachedSeparators.empty())
				{
					separator = mCachedSeparators.back();
					mCachedSeparators.erase(mCachedSeparators.end() - 1);
				}
				else
				{
					separator = GUITexture::create(*this, GUIImageScaleMode::StretchToFit, mSeparatorStyle);
				}

				mContentLayout->addElement(separator);
				newSeparators.push_back(separator);
			}
			else if(element.isSubMenu())
			{
				GUIButton* expEntryBtn = nullptr;
				if(!mCachedExpEntryBtns.empty())
				{
					expEntryBtn = mCachedExpEntryBtns.back();
					mCachedExpEntryBtns.erase(mCachedExpEntryBtns.end() - 1);
				}
				else
				{
					expEntryBtn = GUIButton::create(*this, mElements[i].getLabel(), mEntryExpBtnStyle);
					expEntryBtn->onHover.connect(boost::bind(&GUIDropDownBox::openSubMenu, this, i));
				}

				mContentLayout->addElement(expEntryBtn);
				newExpEntryBtns.push_back(expEntryBtn);
			}
			else
			{
				GUIButton* entryBtn = nullptr;
				if(!mCachedEntryBtns.empty())
				{
					entryBtn = mCachedEntryBtns.back();
					mCachedEntryBtns.erase(mCachedEntryBtns.end() - 1);
				}
				else
				{
					entryBtn = GUIButton::create(*this, mElements[i].getLabel(), mEntryBtnStyle);
					entryBtn->onClick.connect(boost::bind(&GUIDropDownBox::elementClicked, this,  i));
				}

				mContentLayout->addElement(entryBtn);
				newEntryBtns.push_back(entryBtn);
			}
		}

		// Destroy all unused cached elements
		for(auto& elem : mCachedSeparators)
			GUIElement::destroy(elem);

		for(auto& elem : mCachedEntryBtns)
			GUIElement::destroy(elem);

		for(auto& elem : mCachedExpEntryBtns)
			GUIElement::destroy(elem);

		mCachedSeparators = newSeparators;
		mCachedEntryBtns = newEntryBtns;
		mCachedExpEntryBtns = newExpEntryBtns;

		// Add scroll down button
		if(needsScrollDown)
		{
			if(mScrollDownBtn == nullptr)
			{
				mScrollDownBtn = GUIButton::create(*this, GUIContent(L"", mScrollDownBtnArrow), mScrollDownStyle);
				mScrollDownBtn->onClick.connect(boost::bind(&GUIDropDownBox::scrollDown, this));
			}

			mContentLayout->addElement(mScrollDownBtn);			
		}
		else
		{
			if(mScrollDownBtn != nullptr)
			{
				GUIElement::destroy(mScrollDownBtn);
				mScrollDownBtn = nullptr;
			}
		}
		
		// Resize and reposition areas
		mBackgroundArea->setSize(width, usedHeight);
		mBackgroundArea->setPosition(x, y);

		UINT32 contentWidth = (UINT32)std::max(0, (INT32)width - (INT32)mBackgroundStyle->margins.left - (INT32)mBackgroundStyle->margins.right);
		UINT32 contentHeight = (UINT32)std::max(0, (INT32)usedHeight - (INT32)mBackgroundStyle->margins.top - (INT32)mBackgroundStyle->margins.bottom);
		mContentArea->setSize(contentWidth, contentHeight);
		mContentArea->setPosition(x + mBackgroundStyle->margins.left, y + mBackgroundStyle->margins.top);
	}

	UINT32 GUIDropDownBox::getElementHeight(CM::UINT32 idx) const
	{
		if(mElements[idx].isSeparator())
			return mSeparatorStyle->height;
		else if(mElements[idx].isSubMenu())
			return mEntryExpBtnStyle->height;
		else
			return mEntryBtnStyle->height;
	}

	void GUIDropDownBox::scrollDown()
	{
		mPage++;
		updateGUIElements();

		closeSubMenu();
	}

	void GUIDropDownBox::scrollUp()
	{
		if(mPage > 0)
		{
			mPage--;
			updateGUIElements();
		}

		closeSubMenu();
	}

	void GUIDropDownBox::closeSubMenu()
	{
		if(mSubMenuSO != nullptr)
		{
			mSubMenuSO->destroy();
			mSubMenuSO = HSceneObject();
			mSubMenuDropDownBox = GameObjectHandle<GUIDropDownBox>();
		}
	}

	void GUIDropDownBox::elementClicked(UINT32 idx)
	{
		closeSubMenu();

		mElements[idx].getCallback()();
	}

	void GUIDropDownBox::openSubMenu(UINT32 idx)
	{
		closeSubMenu();

		mSubMenuSO = SceneObject::create("DropDownBox");
		mSubMenuDropDownBox = mSubMenuSO->addComponent<GUIDropDownBox>();

		// TODO - Need to provide a parent element
		mSubMenuDropDownBox->initialize(getTarget(), getOwnerWindow(), nullptr, mElements[idx].getSubMenuEntries(), getSkin(), mType);
	}
}
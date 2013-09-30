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

	GUIDropDownAreaPlacement GUIDropDownAreaPlacement::aroundPosition(const CM::Int2& position)
	{
		GUIDropDownAreaPlacement instance;
		instance.mType = Type::Position;
		instance.mPosition = position;

		return instance;
	}

	GUIDropDownAreaPlacement GUIDropDownAreaPlacement::aroundBoundsVert(const CM::Rect& bounds)
	{
		GUIDropDownAreaPlacement instance;
		instance.mType = Type::BoundsVert;
		instance.mBounds = bounds;

		return instance;
	}
		
	GUIDropDownAreaPlacement GUIDropDownAreaPlacement::aroundBoundsHorz(const CM::Rect& bounds)
	{
		GUIDropDownAreaPlacement instance;
		instance.mType = Type::BoundsHorz;
		instance.mBounds = bounds;

		return instance;
	}

	GUIDropDownBox::GUIDropDownBox(const HSceneObject& parent, CM::Viewport* target, CM::RenderWindow* window, const GUIDropDownAreaPlacement& placement,
		const CM::Vector<GUIDropDownData>::type& elements, const GUISkin& skin, GUIDropDownType type)
		:GUIWidget(parent, target, window), mScrollUpStyle(nullptr),
		mScrollDownStyle(nullptr), mEntryBtnStyle(nullptr), mEntryExpBtnStyle(nullptr), 
		mSeparatorStyle(nullptr), mBackgroundStyle(nullptr)
	{
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

		mScrollUpStyle = skin.getStyle(stylePrefix + "ScrollUpBtn");
		mScrollDownStyle = skin.getStyle(stylePrefix + "ScrollDownBtn");
		mEntryBtnStyle = skin.getStyle(stylePrefix + "EntryBtn");
		mEntryExpBtnStyle = skin.getStyle(stylePrefix + "EntryExpBtn");
		mSeparatorStyle = skin.getStyle(stylePrefix + "Separator");
		mBackgroundStyle = skin.getStyle(stylePrefix + "Frame");

		mScrollUpBtnArrow = skin.getStyle(stylePrefix + "ScrollUpBtnArrow")->normal.texture;
		mScrollDownBtnArrow = skin.getStyle(stylePrefix + "ScrollDownBtnArrow")->normal.texture;

		setDepth(0); // Needs to be in front of everything
		setSkin(skin);

		Rect availableBounds(target->getLeft(), target->getTop(), target->getWidth(), target->getHeight());
		mRootMenu = cm_new<DropDownSubMenu>(this, placement, availableBounds, elements, type, 0);
	}

	GUIDropDownBox::~GUIDropDownBox()
	{
		cm_delete(mRootMenu);
	}

	GUIDropDownBox::DropDownSubMenu::DropDownSubMenu(GUIDropDownBox* owner, const GUIDropDownAreaPlacement& placement, 
		const Rect& availableBounds, const CM::Vector<GUIDropDownData>::type& elements, GUIDropDownType type, UINT32 depthOffset)
		:mOwner(owner), mPage(0), mBackgroundFrame(nullptr), mBackgroundArea(nullptr), mContentArea(nullptr), 
		mContentLayout(nullptr), mScrollUpBtn(nullptr), mScrollDownBtn(nullptr), x(0), y(0), width(0), height(0), 
		mType(type), mSubMenu(nullptr), mElements(elements), mOpenedUpward(false), mDepthOffset(depthOffset)
	{
		mAvailableBounds = availableBounds;

		Rect dropDownListBounds = placement.getBounds();
		int potentialLeftStart = 0;
		int potentialRightStart = 0;
		int potentialTopStart = 0;
		int potentialBottomStart = 0;

		switch(placement.getType())
		{
		case GUIDropDownAreaPlacement::Type::Position: 
			potentialLeftStart = potentialRightStart = placement.getPosition().x;
			potentialTopStart = potentialBottomStart = placement.getPosition().y;
			break;
		case GUIDropDownAreaPlacement::Type::BoundsHorz:
			potentialRightStart = placement.getBounds().x;
			potentialLeftStart = placement.getBounds().x + placement.getBounds().width;
			potentialBottomStart = placement.getBounds().y + placement.getBounds().height;
			potentialTopStart = placement.getBounds().y;
			break;
		case GUIDropDownAreaPlacement::Type::BoundsVert:
			potentialRightStart = placement.getBounds().x + placement.getBounds().width;
			potentialLeftStart = placement.getBounds().x;
			potentialBottomStart = placement.getBounds().y;
			potentialTopStart = placement.getBounds().y + placement.getBounds().height;
			break;
		}

		// Determine x position and whether to align to left or right side of the drop down list
		UINT32 availableRightwardWidth = (UINT32)std::max(0, (availableBounds.x + availableBounds.width) - potentialRightStart);
		UINT32 availableLeftwardWidth = (UINT32)std::max(0, potentialLeftStart - availableBounds.x);

		//// Prefer right if possible
		if(DROP_DOWN_BOX_WIDTH <= availableRightwardWidth)
		{
			x = potentialRightStart;
			width = DROP_DOWN_BOX_WIDTH;
		}
		else
		{
			if(availableRightwardWidth >= availableLeftwardWidth)
			{
				x = potentialRightStart;
				width = std::min(DROP_DOWN_BOX_WIDTH, availableRightwardWidth);
			}
			else
			{
				x = potentialLeftStart - std::min(DROP_DOWN_BOX_WIDTH, availableLeftwardWidth);
				width = std::min(DROP_DOWN_BOX_WIDTH, availableLeftwardWidth);
			}
		}

		// Determine y position and whether to open upward or downward
		UINT32 availableDownwardHeight = (UINT32)std::max(0, (availableBounds.y + availableBounds.height) - potentialBottomStart);
		UINT32 availableUpwardHeight = (UINT32)std::max(0, potentialTopStart - availableBounds.y);

		//// Prefer down if possible
		UINT32 helperElementHeight = mOwner->mScrollUpStyle->height + mOwner->mScrollDownStyle->height + 
			mOwner->mBackgroundStyle->margins.top + mOwner->mBackgroundStyle->margins.bottom;

		UINT32 maxNeededHeight = helperElementHeight;
		UINT32 numElements = (UINT32)mElements.size();
		for(UINT32 i = 0; i < numElements; i++)
			maxNeededHeight += getElementHeight(i);

		height = 0;
		if(maxNeededHeight <= availableDownwardHeight)
		{
			y = potentialBottomStart;
			height = availableDownwardHeight;
			mOpenedUpward = false;
		}
		else
		{
			if(availableDownwardHeight >= availableUpwardHeight)
			{
				y = potentialBottomStart;
				height = availableDownwardHeight;
				mOpenedUpward = false;
			}
			else
			{
				y = potentialTopStart;
				height = availableUpwardHeight;
				mOpenedUpward = true;
			}
		}

		INT32 actualY = y;
			
		if(mOpenedUpward)	
			actualY -= (INT32)std::min(maxNeededHeight, availableUpwardHeight);

		// Content area
		mContentArea = GUIArea::create(*mOwner, x, actualY, width, height);
		mContentArea->setDepth(10000 - depthOffset * 2 - 1);
		mContentLayout = &mContentArea->getLayout().addLayoutY();

		// Background frame
		mBackgroundArea = GUIArea::create(*mOwner, x, actualY, width, height);
		mBackgroundArea->setDepth(10000 - depthOffset * 2);

		mBackgroundFrame = GUITexture::create(*mOwner, GUIImageScaleMode::StretchToFit, mOwner->mBackgroundStyle);
		mBackgroundArea->getLayout().addElement(mBackgroundFrame);

		updateGUIElements();
	}

	GUIDropDownBox::DropDownSubMenu::~DropDownSubMenu()
	{
		closeSubMenu();

		for(auto& elem : mCachedSeparators)
			GUIElement::destroy(elem);

		for(auto& elem : mCachedEntryBtns)
			GUIElement::destroy(elem);

		for(auto& elem : mCachedExpEntryBtns)
			GUIElement::destroy(elem);

		if(mScrollUpBtn != nullptr)
			GUIElement::destroy(mScrollUpBtn);

		if(mScrollDownBtn != nullptr)
			GUIElement::destroy(mScrollDownBtn);

		GUIElement::destroy(mBackgroundFrame);

		GUIArea::destroy(mBackgroundArea);
		GUIArea::destroy(mContentArea);
	}

	void GUIDropDownBox::DropDownSubMenu::updateGUIElements()
	{
		// Remove all elements from content layout
		while(mContentLayout->getNumChildren() > 0)
			mContentLayout->removeChildAt(mContentLayout->getNumChildren() - 1);

		// Determine if we need scroll up and/or down buttons, number of visible elements and actual height
		bool needsScrollUp = mPage > 0;
		UINT32 numElements = (UINT32)mElements.size();

		UINT32 usedHeight = mOwner->mBackgroundStyle->margins.top + mOwner->mBackgroundStyle->margins.bottom;

		UINT32 pageStart = 0, pageEnd = 0;
		UINT32 curPage = 0;
		bool needsScrollDown = false;
		for(UINT32 i = 0; i < numElements; i++)
		{
			usedHeight += getElementHeight(i);
			pageEnd++;

			if(usedHeight > height)
			{
				usedHeight += mOwner->mScrollDownStyle->height;

				// Remove last few elements until we fit again
				while(usedHeight > height && i >= 0)
				{
					usedHeight -= getElementHeight(i);
					pageEnd--;

					i--;
				}

				// We found our page and are done
				if(curPage == mPage)
				{
					needsScrollDown = i != (numElements - 1);
					break;
				}

				// Nothing fits, break out of infinite loop
				if(pageStart == pageEnd)
				{
					needsScrollDown = i != (numElements - 1);
					break;
				}

				pageStart = pageEnd;
				usedHeight = mOwner->mBackgroundStyle->margins.top + mOwner->mBackgroundStyle->margins.bottom;
				usedHeight += mOwner->mScrollUpStyle->height;

				curPage++;
			}
		}

		// Add scroll up button
		if(needsScrollUp)
		{
			if(mScrollUpBtn == nullptr)
			{
				mScrollUpBtn = GUIButton::create(*mOwner, GUIContent(L"", mOwner->mScrollUpBtnArrow), mOwner->mScrollUpStyle);
				mScrollUpBtn->onClick.connect(boost::bind(&DropDownSubMenu::scrollUp, this));
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
					separator = GUITexture::create(*mOwner, GUIImageScaleMode::StretchToFit, mOwner->mSeparatorStyle);
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
					expEntryBtn = GUIButton::create(*mOwner, mElements[i].getLabel(), mOwner->mEntryExpBtnStyle);
					expEntryBtn->onHover.connect(boost::bind(&DropDownSubMenu::openSubMenu, this, expEntryBtn, i));
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
					entryBtn = GUIButton::create(*mOwner, mElements[i].getLabel(), mOwner->mEntryBtnStyle);
					entryBtn->onHover.connect(boost::bind(&DropDownSubMenu::closeSubMenu, this));
					entryBtn->onClick.connect(boost::bind(&DropDownSubMenu::elementClicked, this,  i));
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
				mScrollDownBtn = GUIButton::create(*mOwner, GUIContent(L"", mOwner->mScrollDownBtnArrow), mOwner->mScrollDownStyle);
				mScrollDownBtn->onClick.connect(boost::bind(&DropDownSubMenu::scrollDown, this));
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
		INT32 actualY = y;

		if(mOpenedUpward)	
			actualY -= (INT32)usedHeight;

		mBackgroundArea->setSize(width, usedHeight);
		mBackgroundArea->setPosition(x, actualY);

		UINT32 contentWidth = (UINT32)std::max(0, (INT32)width - (INT32)mOwner->mBackgroundStyle->margins.left - (INT32)mOwner->mBackgroundStyle->margins.right);
		UINT32 contentHeight = (UINT32)std::max(0, (INT32)usedHeight - (INT32)mOwner->mBackgroundStyle->margins.top - (INT32)mOwner->mBackgroundStyle->margins.bottom);
		mContentArea->setSize(contentWidth, contentHeight);
		mContentArea->setPosition(x + mOwner->mBackgroundStyle->margins.left, actualY + mOwner->mBackgroundStyle->margins.top);
	}

	UINT32 GUIDropDownBox::DropDownSubMenu::getElementHeight(CM::UINT32 idx) const
	{
		if(mElements[idx].isSeparator())
			return mOwner->mSeparatorStyle->height;
		else if(mElements[idx].isSubMenu())
			return mOwner->mEntryExpBtnStyle->height;
		else
			return mOwner->mEntryBtnStyle->height;
	}

	void GUIDropDownBox::DropDownSubMenu::scrollDown()
	{
		mPage++;
		updateGUIElements();

		closeSubMenu();
	}

	void GUIDropDownBox::DropDownSubMenu::scrollUp()
	{
		if(mPage > 0)
		{
			mPage--;
			updateGUIElements();
		}

		closeSubMenu();
	}

	void GUIDropDownBox::DropDownSubMenu::closeSubMenu()
	{
		if(mSubMenu != nullptr)
		{
			cm_delete(mSubMenu);
			mSubMenu = nullptr;
		}
	}

	void GUIDropDownBox::DropDownSubMenu::elementClicked(UINT32 idx)
	{
		closeSubMenu();

		mElements[idx].getCallback()();
	}

	void GUIDropDownBox::DropDownSubMenu::openSubMenu(GUIButton* source, UINT32 idx)
	{
		closeSubMenu();

		mSubMenu = cm_new<DropDownSubMenu>(mOwner, GUIDropDownAreaPlacement::aroundBoundsVert(source->getBounds()), 
			mAvailableBounds, mElements[idx].getSubMenuEntries(), mType, mDepthOffset + 1);
	}
}
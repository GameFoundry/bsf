#include "BsGUIDropDownBox.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILayoutX.h"
#include "BsGUITexture.h"
#include "BsGUILabel.h"
#include "BsGUIButton.h"
#include "BsGUISpace.h"
#include "BsGUIContent.h"
#include "BsGUISkin.h"
#include "BsViewport.h"
#include "BsGUIListBox.h"
#include "BsGUIDropDownBoxManager.h"
#include "BsSceneObject.h"
#include "BsGUIDropDownHitBox.h"
#include "BsGUIDropDownContent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 GUIDropDownBox::DROP_DOWN_BOX_WIDTH = 250;

	GUIDropDownDataEntry GUIDropDownDataEntry::separator()
	{
		GUIDropDownDataEntry data;
		data.mType = Type::Separator;
		data.mCallback = nullptr;

		return data;
	}

	GUIDropDownDataEntry GUIDropDownDataEntry::button(const WString& label, std::function<void()> callback, const WString& shortcutTag)
	{
		GUIDropDownDataEntry data;
		data.mLabel = label;
		data.mType = Type::Entry;
		data.mCallback = callback;
		data.mShortcutTag = shortcutTag;

		return data;
	}

	GUIDropDownDataEntry GUIDropDownDataEntry::subMenu(const WString& label, const GUIDropDownData& data)
	{
		GUIDropDownDataEntry dataEntry;
		dataEntry.mLabel = label;
		dataEntry.mType = Type::SubMenu;
		dataEntry.mChildData = data;

		return dataEntry;
	}

	GUIDropDownAreaPlacement GUIDropDownAreaPlacement::aroundPosition(const Vector2I& position)
	{
		GUIDropDownAreaPlacement instance;
		instance.mType = Type::Position;
		instance.mPosition = position;

		return instance;
	}

	GUIDropDownAreaPlacement GUIDropDownAreaPlacement::aroundBoundsVert(const Rect2I& bounds)
	{
		GUIDropDownAreaPlacement instance;
		instance.mType = Type::BoundsVert;
		instance.mBounds = bounds;

		return instance;
	}
		
	GUIDropDownAreaPlacement GUIDropDownAreaPlacement::aroundBoundsHorz(const Rect2I& bounds)
	{
		GUIDropDownAreaPlacement instance;
		instance.mType = Type::BoundsHorz;
		instance.mBounds = bounds;

		return instance;
	}

	GUIDropDownBox::GUIDropDownBox(const HSceneObject& parent, Viewport* target, const GUIDropDownAreaPlacement& placement,
		const GUIDropDownData& dropDownData, const GUISkin& skin, GUIDropDownType type)
		:GUIWidget(parent, target), mRootMenu(nullptr), mHitBox(nullptr), mCaptureHitBox(nullptr)
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

		mScrollUpStyle = stylePrefix + "ScrollUpBtn";
		mScrollDownStyle = stylePrefix + "ScrollDownBtn";
		mBackgroundStyle = stylePrefix + "Frame";
		mContentStyle = stylePrefix + "Content";

		mScrollUpBtnArrow = skin.getStyle(stylePrefix + "ScrollUpBtnArrow")->normal.texture;
		mScrollDownBtnArrow = skin.getStyle(stylePrefix + "ScrollDownBtnArrow")->normal.texture;

		setDepth(0); // Needs to be in front of everything
		setSkin(skin);

		mHitBox = GUIDropDownHitBox::create(false);
		mHitBox->onFocusLost.connect(std::bind(&GUIDropDownBox::dropDownFocusLost, this));
		mHitBox->setFocus(true);
		mHitBox->_setWidgetDepth(0);
		mHitBox->_setAreaDepth(0);
		mHitBox->_changeParentWidget(this);

		mCaptureHitBox = GUIDropDownHitBox::create(true);
		mCaptureHitBox->setBounds(Rect2I(0, 0, target->getWidth(), target->getHeight()));
		mCaptureHitBox->_setWidgetDepth(0);
		mCaptureHitBox->_setAreaDepth(200);
		mCaptureHitBox->_changeParentWidget(this);

		Rect2I availableBounds(target->getX(), target->getY(), target->getWidth(), target->getHeight());
		mRootMenu = bs_new<DropDownSubMenu>(this, nullptr, placement, availableBounds, dropDownData, type, 0);
	}

	GUIDropDownBox::~GUIDropDownBox()
	{
		GUIElement::destroy(mHitBox);
		GUIElement::destroy(mCaptureHitBox);
		bs_delete(mRootMenu);
	}

	void GUIDropDownBox::dropDownFocusLost()
	{
		mRootMenu->closeSubMenu();
		GUIDropDownBoxManager::instance().closeDropDownBox();
	}

	void GUIDropDownBox::notifySubMenuOpened(DropDownSubMenu* subMenu)
	{
		Vector<Rect2I> bounds;

		while(subMenu != nullptr)
		{
			bounds.push_back(subMenu->getVisibleBounds());

			subMenu = subMenu->mSubMenu;
		}

		mHitBox->setBounds(bounds);
	}

	void GUIDropDownBox::notifySubMenuClosed(DropDownSubMenu* subMenu)
	{
		Vector<Rect2I> bounds;

		while(subMenu != nullptr)
		{
			bounds.push_back(subMenu->getVisibleBounds());

			subMenu = subMenu->mSubMenu;
		}

		mHitBox->setBounds(bounds);
	}

	GUIDropDownBox::DropDownSubMenu::DropDownSubMenu(GUIDropDownBox* owner, DropDownSubMenu* parent, const GUIDropDownAreaPlacement& placement,
		const Rect2I& availableBounds, const GUIDropDownData& dropDownData, GUIDropDownType type, UINT32 depthOffset)
		:mOwner(owner), mParent(parent), mPage(0), mBackgroundFrame(nullptr), mBackgroundArea(nullptr), mContentArea(nullptr),
		mContentLayout(nullptr), mScrollUpBtn(nullptr), mScrollDownBtn(nullptr), x(0), y(0), width(0), height(0), 
		mType(type), mSubMenu(nullptr), mData(dropDownData), mOpenedUpward(false), mDepthOffset(depthOffset), mContent(nullptr)
	{
		mAvailableBounds = availableBounds;

		const GUIElementStyle* scrollUpStyle = mOwner->getSkin().getStyle(mOwner->mScrollUpStyle);
		const GUIElementStyle* scrollDownStyle = mOwner->getSkin().getStyle(mOwner->mScrollDownStyle);
		const GUIElementStyle* backgroundStyle = mOwner->getSkin().getStyle(mOwner->mBackgroundStyle);

		Rect2I dropDownListBounds = placement.getBounds();
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

		// Create content GUI element
		mContent = GUIDropDownContent::create(this, dropDownData, mOwner->mContentStyle);
		mContent->setKeyboardFocus(true);

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
		UINT32 helperElementHeight = scrollUpStyle->height + scrollDownStyle->height + 
			backgroundStyle->margins.top + backgroundStyle->margins.bottom;

		UINT32 maxNeededHeight = helperElementHeight;
		UINT32 numElements = (UINT32)dropDownData.entries.size();
		for(UINT32 i = 0; i < numElements; i++)
			maxNeededHeight += mContent->getElementHeight(i);

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
		mContentArea->setDepth(100 - depthOffset * 2 - 1);
		mContentLayout = &mContentArea->getLayout().addLayoutY();

		// Background frame
		mBackgroundArea = GUIArea::create(*mOwner, x, actualY, width, height);
		mBackgroundArea->setDepth(100 - depthOffset * 2);

		mBackgroundFrame = GUITexture::create(GUIImageScaleMode::StretchToFit, mOwner->mBackgroundStyle);
		mBackgroundArea->getLayout().addElement(mBackgroundFrame);

		updateGUIElements();

		mOwner->notifySubMenuOpened(this);
	}

	GUIDropDownBox::DropDownSubMenu::~DropDownSubMenu()
	{
		closeSubMenu();

		mOwner->notifySubMenuClosed(this);

		GUIElement::destroy(mContent);

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

		const GUIElementStyle* scrollUpStyle = mOwner->getSkin().getStyle(mOwner->mScrollUpStyle);
		const GUIElementStyle* scrollDownStyle = mOwner->getSkin().getStyle(mOwner->mScrollDownStyle);
		const GUIElementStyle* backgroundStyle = mOwner->getSkin().getStyle(mOwner->mBackgroundStyle);

		// Determine if we need scroll up and/or down buttons, number of visible elements and actual height
		bool needsScrollUp = mPage > 0;
		UINT32 numElements = (UINT32)mData.entries.size();

		UINT32 usedHeight = backgroundStyle->margins.top + backgroundStyle->margins.bottom;

		UINT32 pageStart = 0, pageEnd = 0;
		UINT32 curPage = 0;
		bool needsScrollDown = false;
		for(UINT32 i = 0; i < numElements; i++)
		{
			usedHeight += mContent->getElementHeight(i);
			pageEnd++;

			if(usedHeight > height)
			{
				usedHeight += scrollDownStyle->height;

				// Remove last few elements until we fit again
				while(usedHeight > height && i >= 0)
				{
					usedHeight -= mContent->getElementHeight(i);
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
				usedHeight = backgroundStyle->margins.top + backgroundStyle->margins.bottom;
				usedHeight += scrollUpStyle->height;

				curPage++;
			}
		}

		// Add scroll up button
		if(needsScrollUp)
		{
			if(mScrollUpBtn == nullptr)
			{
				mScrollUpBtn = GUIButton::create(GUIContent(HString(L""), mOwner->mScrollUpBtnArrow), mOwner->mScrollUpStyle);
				mScrollUpBtn->onClick.connect(std::bind(&DropDownSubMenu::scrollUp, this));
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

		mContent->setRange(pageStart, pageEnd);
		mContentLayout->addElement(mContent);

		// Add scroll down button
		if(needsScrollDown)
		{
			if(mScrollDownBtn == nullptr)
			{
				mScrollDownBtn = GUIButton::create(GUIContent(HString(L""), mOwner->mScrollDownBtnArrow), mOwner->mScrollDownStyle);
				mScrollDownBtn->onClick.connect(std::bind(&DropDownSubMenu::scrollDown, this));
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

		mVisibleBounds = Rect2I(x, actualY, width, usedHeight);

		UINT32 contentWidth = (UINT32)std::max(0, (INT32)width - (INT32)backgroundStyle->margins.left - (INT32)backgroundStyle->margins.right);
		UINT32 contentHeight = (UINT32)std::max(0, (INT32)usedHeight - (INT32)backgroundStyle->margins.top - (INT32)backgroundStyle->margins.bottom);
		mContentArea->setSize(contentWidth, contentHeight);
		mContentArea->setPosition(x + backgroundStyle->margins.left, actualY + backgroundStyle->margins.top);
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
			bs_delete(mSubMenu);
			mSubMenu = nullptr;

			mContent->setKeyboardFocus(true);
		}
	}

	void GUIDropDownBox::DropDownSubMenu::elementActivated(UINT32 idx)
	{
		closeSubMenu();

		auto callback = mData.entries[idx].getCallback();
		if(callback != nullptr)
			callback();

		GUIDropDownBoxManager::instance().closeDropDownBox();
	}

	void GUIDropDownBox::DropDownSubMenu::close()
	{
		if (mParent != nullptr)
			mParent->closeSubMenu();
		else // We're the last sub-menu, close the whole thing
			GUIDropDownBoxManager::instance().closeDropDownBox();
	}

	void GUIDropDownBox::DropDownSubMenu::elementSelected(UINT32 idx, const Rect2I& bounds)
	{
		closeSubMenu();

		if (mData.entries[idx].isSubMenu())
		{
			mContent->setKeyboardFocus(false);

			mSubMenu = bs_new<DropDownSubMenu>(mOwner, this, GUIDropDownAreaPlacement::aroundBoundsVert(bounds),
				mAvailableBounds, mData.entries[idx].getSubMenuData(), mType, mDepthOffset + 1);
		}
	}
}
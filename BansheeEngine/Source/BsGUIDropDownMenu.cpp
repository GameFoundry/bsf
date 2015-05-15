#include "BsGUIDropDownMenu.h"
#include "BsGUIPanel.h"
#include "BsGUILayoutY.h"
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
	const UINT32 GUIDropDownMenu::DROP_DOWN_BOX_WIDTH = 250;

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

	GUIDropDownMenu::GUIDropDownMenu(const HSceneObject& parent, Viewport* target, const DropDownAreaPlacement& placement,
		const GUIDropDownData& dropDownData, const HGUISkin& skin, GUIDropDownType type)
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

		mScrollUpBtnArrow = skin->getStyle(stylePrefix + "ScrollUpBtnArrow")->normal.texture;
		mScrollDownBtnArrow = skin->getStyle(stylePrefix + "ScrollDownBtnArrow")->normal.texture;

		setDepth(0); // Needs to be in front of everything
		setSkin(skin);

		mHitBox = GUIDropDownHitBox::create(false);
		mHitBox->onFocusLost.connect(std::bind(&GUIDropDownMenu::dropDownFocusLost, this));
		mHitBox->setFocus(true);
		GUILayoutData hitboxLayoutData = mHitBox->_getLayoutData();
		hitboxLayoutData.setWidgetDepth(0);
		hitboxLayoutData.setPanelDepth(0);
		mHitBox->_setLayoutData(hitboxLayoutData);
		mHitBox->_changeParentWidget(this);
		mHitBox->_markContentAsDirty();

		mCaptureHitBox = GUIDropDownHitBox::create(true);
		mCaptureHitBox->setBounds(Rect2I(0, 0, target->getWidth(), target->getHeight()));
		GUILayoutData captureHitboxLayoutData = mCaptureHitBox->_getLayoutData();
		captureHitboxLayoutData.setWidgetDepth(0);
		captureHitboxLayoutData.setPanelDepth(200);
		mCaptureHitBox->_setLayoutData(captureHitboxLayoutData);
		mCaptureHitBox->_changeParentWidget(this);
		mCaptureHitBox->_markContentAsDirty();

		Rect2I availableBounds(target->getX(), target->getY(), target->getWidth(), target->getHeight());
		mRootMenu = bs_new<DropDownSubMenu>(this, nullptr, placement, availableBounds, dropDownData, type, 0);
	}

	GUIDropDownMenu::~GUIDropDownMenu()
	{

	}

	void GUIDropDownMenu::onDestroyed()
	{
		GUIElement::destroy(mHitBox);
		GUIElement::destroy(mCaptureHitBox);
		bs_delete(mRootMenu);

		GUIWidget::onDestroyed();
	}

	void GUIDropDownMenu::dropDownFocusLost()
	{
		mRootMenu->closeSubMenu();
		GUIDropDownBoxManager::instance().closeDropDownBox();
	}

	void GUIDropDownMenu::notifySubMenuOpened(DropDownSubMenu* subMenu)
	{
		Vector<Rect2I> bounds;

		while(subMenu != nullptr)
		{
			bounds.push_back(subMenu->getVisibleBounds());

			subMenu = subMenu->mSubMenu;
		}

		mHitBox->setBounds(bounds);
	}

	void GUIDropDownMenu::notifySubMenuClosed(DropDownSubMenu* subMenu)
	{
		Vector<Rect2I> bounds;

		while(subMenu != nullptr)
		{
			bounds.push_back(subMenu->getVisibleBounds());

			subMenu = subMenu->mSubMenu;
		}

		mHitBox->setBounds(bounds);
	}

	GUIDropDownMenu::DropDownSubMenu::DropDownSubMenu(GUIDropDownMenu* owner, DropDownSubMenu* parent, const DropDownAreaPlacement& placement,
		const Rect2I& availableBounds, const GUIDropDownData& dropDownData, GUIDropDownType type, UINT32 depthOffset)
		:mOwner(owner), mParent(parent), mPage(0), mBackgroundFrame(nullptr), mBackgroundPanel(nullptr), mContentPanel(nullptr),
		mContentLayout(nullptr), mScrollUpBtn(nullptr), mScrollDownBtn(nullptr), x(0), y(0), width(0), height(0), 
		mType(type), mSubMenu(nullptr), mData(dropDownData), mOpenedUpward(false), mDepthOffset(depthOffset), mContent(nullptr)
	{
		mAvailableBounds = availableBounds;

		const GUIElementStyle* scrollUpStyle = mOwner->getSkin().getStyle(mOwner->mScrollUpStyle);
		const GUIElementStyle* scrollDownStyle = mOwner->getSkin().getStyle(mOwner->mScrollDownStyle);
		const GUIElementStyle* backgroundStyle = mOwner->getSkin().getStyle(mOwner->mBackgroundStyle);

		// Create content GUI element
		mContent = GUIDropDownContent::create(this, dropDownData, mOwner->mContentStyle);
		mContent->setKeyboardFocus(true);

		// Content area
		mContentPanel = mOwner->getPanel()->addNewElement<GUIPanel>();
		mContentPanel->setWidth(width);
		mContentPanel->setHeight(height);
		mContentPanel->setDepthRange(100 - depthOffset * 2 - 1);

		mContentLayout = mContentPanel->addNewElement<GUILayoutY>();

		// Background frame
		mBackgroundPanel = mOwner->getPanel()->addNewElement<GUIPanel>();
		mBackgroundPanel->setWidth(width);
		mBackgroundPanel->setHeight(height);
		mBackgroundPanel->setDepthRange(100 - depthOffset * 2);

		GUILayout* backgroundLayout = mBackgroundPanel->addNewElement<GUILayoutX>();

		mBackgroundFrame = GUITexture::create(GUIImageScaleMode::StretchToFit, mOwner->mBackgroundStyle);
		backgroundLayout->addElement(mBackgroundFrame);

		mContentLayout->addElement(mContent); // Note: It's important this is added to the layout before we 
		// use it for size calculations, in order for its skin to be assigned

		UINT32 helperElementHeight = scrollUpStyle->height + scrollDownStyle->height +
			backgroundStyle->margins.top + backgroundStyle->margins.bottom;

		UINT32 maxNeededHeight = helperElementHeight;
		UINT32 numElements = (UINT32)dropDownData.entries.size();
		for (UINT32 i = 0; i < numElements; i++)
			maxNeededHeight += mContent->getElementHeight(i);

		DropDownAreaPlacement::HorzDir horzDir;
		DropDownAreaPlacement::VertDir vertDir;
		Rect2I placementBounds = placement.getOptimalBounds(DROP_DOWN_BOX_WIDTH, maxNeededHeight, availableBounds, horzDir, vertDir);

		mOpenedUpward = vertDir == DropDownAreaPlacement::VertDir::Up;

		x = placementBounds.x;
		y = placementBounds.y;
		width = placementBounds.width;
		height = placementBounds.height;

		mContentPanel->setPosition(x, y);
		mBackgroundPanel->setPosition(x, y);

		updateGUIElements();

		mOwner->notifySubMenuOpened(this);
	}

	GUIDropDownMenu::DropDownSubMenu::~DropDownSubMenu()
	{
		closeSubMenu();

		mOwner->notifySubMenuClosed(this);

		GUIElement::destroy(mContent);

		if (mScrollUpBtn != nullptr)
			GUIElement::destroy(mScrollUpBtn);

		if (mScrollDownBtn != nullptr)
			GUIElement::destroy(mScrollDownBtn);

		GUIElement::destroy(mBackgroundFrame);

		GUILayout::destroy(mBackgroundPanel);
		GUILayout::destroy(mContentPanel);
	}

	Vector<GUIDropDownMenu::DropDownSubMenu::PageInfo> GUIDropDownMenu::DropDownSubMenu::getPageInfos() const
	{
		const GUIElementStyle* scrollUpStyle = mOwner->getSkin().getStyle(mOwner->mScrollUpStyle);
		const GUIElementStyle* scrollDownStyle = mOwner->getSkin().getStyle(mOwner->mScrollDownStyle);
		const GUIElementStyle* backgroundStyle = mOwner->getSkin().getStyle(mOwner->mBackgroundStyle);

		INT32 numElements = (INT32)mData.entries.size();

		PageInfo curPageInfo;
		curPageInfo.start = 0;
		curPageInfo.end = 0;
		curPageInfo.idx = 0;
		curPageInfo.height = backgroundStyle->margins.top + backgroundStyle->margins.bottom;
		
		Vector<PageInfo> pageInfos;
		for (INT32 i = 0; i < numElements; i++)
		{
			curPageInfo.height += mContent->getElementHeight((UINT32)i);
			curPageInfo.end++;

			if (curPageInfo.height > height)
			{
				curPageInfo.height += scrollDownStyle->height;

				// Remove last few elements until we fit again
				while (curPageInfo.height > height && i >= 0)
				{
					curPageInfo.height -= mContent->getElementHeight((UINT32)i);
					curPageInfo.end--;

					i--;
				}

				// Nothing fits, break out of infinite loop
				if (curPageInfo.start >= curPageInfo.end)
					break;

				pageInfos.push_back(curPageInfo);

				curPageInfo.start = curPageInfo.end;
				curPageInfo.height = backgroundStyle->margins.top + backgroundStyle->margins.bottom;
				curPageInfo.height += scrollUpStyle->height;

				curPageInfo.idx++;
			}
		}

		if (curPageInfo.start < curPageInfo.end)
			pageInfos.push_back(curPageInfo);

		return pageInfos;
	}

	void GUIDropDownMenu::DropDownSubMenu::updateGUIElements()
	{
		// Remove all elements from content layout
		while(mContentLayout->getNumChildren() > 0)
			mContentLayout->removeElementAt(mContentLayout->getNumChildren() - 1);

		mContentLayout->addElement(mContent); // Note: Needs to be added first so that size calculations have proper skin to work with

		const GUIElementStyle* scrollUpStyle = mOwner->getSkin().getStyle(mOwner->mScrollUpStyle);
		const GUIElementStyle* scrollDownStyle = mOwner->getSkin().getStyle(mOwner->mScrollDownStyle);
		const GUIElementStyle* backgroundStyle = mOwner->getSkin().getStyle(mOwner->mBackgroundStyle);

		// Determine if we need scroll up and/or down buttons, number of visible elements and actual height
		bool needsScrollUp = mPage > 0;

		UINT32 pageStart = 0, pageEnd = 0;
		UINT32 pageHeight = 0;
		Vector<PageInfo> pageInfos = getPageInfos();

		if (pageInfos.size() > mPage)
		{
			pageStart = pageInfos[mPage].start;
			pageEnd = pageInfos[mPage].end;
			pageHeight = pageInfos[mPage].height;
		}

		UINT32 numElements = (UINT32)mData.entries.size();
		bool needsScrollDown = pageEnd != numElements;

		// Add scroll up button
		if(needsScrollUp)
		{
			if(mScrollUpBtn == nullptr)
			{
				mScrollUpBtn = GUIButton::create(GUIContent(HString(L""), mOwner->mScrollUpBtnArrow), mOwner->mScrollUpStyle);
				mScrollUpBtn->onClick.connect(std::bind(&DropDownSubMenu::scrollUp, this));
			}

			mContentLayout->insertElement(0, mScrollUpBtn);			
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
			actualY -= (INT32)pageHeight;

		mBackgroundPanel->setWidth(width);
		mBackgroundPanel->setHeight(pageHeight);
		mBackgroundPanel->setPosition(x, actualY);

		mVisibleBounds = Rect2I(x, actualY, width, pageHeight);

		UINT32 contentWidth = (UINT32)std::max(0, (INT32)width - (INT32)backgroundStyle->margins.left - (INT32)backgroundStyle->margins.right);
		UINT32 contentHeight = (UINT32)std::max(0, (INT32)pageHeight - (INT32)backgroundStyle->margins.top - (INT32)backgroundStyle->margins.bottom);

		mContentPanel->setWidth(contentWidth);
		mContentPanel->setHeight(contentHeight);
		mContentPanel->setPosition(x + backgroundStyle->margins.left, actualY + backgroundStyle->margins.top);
	}

	void GUIDropDownMenu::DropDownSubMenu::scrollDown()
	{
		mPage++;
		if (mPage == (UINT32)getPageInfos().size())
			mPage = 0;

		updateGUIElements();

		closeSubMenu();
	}

	void GUIDropDownMenu::DropDownSubMenu::scrollUp()
	{
		if (mPage > 0)
			mPage--;
		else
			mPage = (UINT32)getPageInfos().size() - 1;

		updateGUIElements();
		closeSubMenu();
	}

	void GUIDropDownMenu::DropDownSubMenu::scrollToTop()
	{
		mPage = 0;
		updateGUIElements();

		closeSubMenu();
	}

	void GUIDropDownMenu::DropDownSubMenu::scrollToBottom()
	{
		mPage = (UINT32)(getPageInfos().size() - 1);
		updateGUIElements();

		closeSubMenu();
	}

	void GUIDropDownMenu::DropDownSubMenu::closeSubMenu()
	{
		if(mSubMenu != nullptr)
		{
			bs_delete(mSubMenu);
			mSubMenu = nullptr;

			mContent->setKeyboardFocus(true);
		}
	}

	void GUIDropDownMenu::DropDownSubMenu::elementActivated(UINT32 idx, const Rect2I& bounds)
	{
		closeSubMenu();

		if (!mData.entries[idx].isSubMenu())
		{
			auto callback = mData.entries[idx].getCallback();
			if (callback != nullptr)
				callback();

			GUIDropDownBoxManager::instance().closeDropDownBox();
		}
		else
		{
			mContent->setKeyboardFocus(false);

			mSubMenu = bs_new<DropDownSubMenu>(mOwner, this, DropDownAreaPlacement::aroundBoundsVert(bounds),
				mAvailableBounds, mData.entries[idx].getSubMenuData(), mType, mDepthOffset + 1);
		}
	}

	void GUIDropDownMenu::DropDownSubMenu::close()
	{
		if (mParent != nullptr)
			mParent->closeSubMenu();
		else // We're the last sub-menu, close the whole thing
			GUIDropDownBoxManager::instance().closeDropDownBox();
	}

	void GUIDropDownMenu::DropDownSubMenu::elementSelected(UINT32 idx)
	{
		closeSubMenu();
	}
}
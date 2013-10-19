#include "BsGUITabbedTitleBar.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIButton.h"
#include "BsGUITabButton.h"
#include "BsGUISpace.h"
#include "BsGUIWindowDropArea.h"
#include "BsEngineGUI.h"
#include "BsGUIWidget.h"
#include "CmMath.h"
#include "CmPlatform.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	GUITabbedTitleBar::GUITabbedTitleBar(BS::GUIWidget* parent)
		:mLastDropElement(nullptr), mMinBtn(nullptr), mCloseBtn(nullptr), 
		mMainArea(nullptr), mMainLayout(nullptr), mParentWidget(parent), mBackgroundArea(nullptr), mUniqueTabIdx(0), mActiveTabIdx(0)
	{
		mBackgroundArea = GUIArea::create(*parent, 0, 0, 1, 13, 9900);
		GUIWindowDropArea* titleBarBg = GUIWindowDropArea::create(*parent, parent->getSkin().getStyle("TitleBarBackground"));
		mBackgroundArea->getLayout().addSpace(1);
		mBackgroundArea->getLayout().addElement(titleBarBg);
		mBackgroundArea->getLayout().addSpace(1);

		mMainArea = GUIArea::create(*parent, 0, 0, 1, 13, 9899);

		GUIWindowDropArea* dragDropElement = GUIWindowDropArea::create(*parent, GUIOptions(GUIOption::flexibleWidth(20)), parent->getSkin().getStyle("TabbedBarDropArea"));
		mLastDropElement = dragDropElement;

		mMinBtn = GUIButton::create(*parent, HString(L""), parent->getSkin().getStyle("WinMinimizeBtn"));
		mCloseBtn = GUIButton::create(*parent, HString(L""), parent->getSkin().getStyle("WinCloseBtn"));

		mCloseBtn->onClick.connect(boost::bind(&GUITabbedTitleBar::tabClosed, this));

		mMainArea->getLayout().addSpace(1);
		mMainLayout = &mMainArea->getLayout().addLayoutX();
		mMainLayout->addElement(dragDropElement);
		mMainLayout->addElement(mMinBtn);
		mMainLayout->addSpace(3);
		mMainLayout->addElement(mCloseBtn);
		mMainArea->getLayout().addSpace(3);

		refreshNonClientAreas();
	}

	GUITabbedTitleBar::~GUITabbedTitleBar()
	{
		GUIArea::destroy(mMainArea);
		GUIArea::destroy(mBackgroundArea);

		GUIElement::destroy(mLastDropElement);
		GUIElement::destroy(mMinBtn);
		GUIElement::destroy(mCloseBtn);

		for(auto& tabButton : mTabButtons)
		{
			GUIElement::destroy(tabButton);
		}

		for(auto& dragDropButton : mDragDropElements)
		{
			GUIElement::destroy(dragDropButton);
		}
	}

	void GUITabbedTitleBar::addTab(const CM::HString& name)
	{
		insertTab((UINT32)mTabButtons.size(), name);
	}

	void GUITabbedTitleBar::insertTab(UINT32 idx, const CM::HString& name)
	{
		GUITabButton* newTabToggle = GUITabButton::create(*mParentWidget, this, mUniqueTabIdx, name, EngineGUI::instance().getSkin().getStyle("TabbedBarBtn"));
		GUIWindowDropArea* newDragDropElement = GUIWindowDropArea::create(*mParentWidget, EngineGUI::instance().getSkin().getStyle("TabbedBarDropArea"));

		idx = Math::Clamp(idx, 0U, (UINT32)mTabButtons.size());

		newTabToggle->onToggled.connect(boost::bind(&GUITabbedTitleBar::tabToggled, this, mUniqueTabIdx));
		newTabToggle->onDragged.connect(boost::bind(&GUITabbedTitleBar::tabDraggedOff, this, _1));

		newDragDropElement->onDraggedItemDropped.connect(boost::bind(&GUITabbedTitleBar::tabDraggedOn, this, mUniqueTabIdx));

		mTabButtons.insert(mTabButtons.begin() + idx, newTabToggle);
		mDragDropElements.insert(mDragDropElements.begin() + idx, newDragDropElement);

		mMainLayout->insertElement(idx * 2, newTabToggle);
		mMainLayout->insertElement(idx * 2, newDragDropElement);

		mUniqueTabIdx++;

		refreshNonClientAreas();
	}

	void GUITabbedTitleBar::removeTab(UINT32 idx)
	{
		if(mTabButtons.size() == 0)
			return;

		idx = Math::Clamp(idx, 0U, (UINT32)mTabButtons.size() - 1);

		GUIElement::destroy(mTabButtons[idx]);
		GUIElement::destroy(mDragDropElements[idx]);

		mTabButtons.erase(mTabButtons.begin() + idx);
		mDragDropElements.erase(mDragDropElements.begin() + idx);

		refreshNonClientAreas();
	}

	void GUITabbedTitleBar::setPosition(INT32 x, INT32 y)
	{
		mMainArea->setPosition(x, y);
		mBackgroundArea->setPosition(x, y);

		refreshNonClientAreas();
	}

	void GUITabbedTitleBar::setSize(UINT32 width, UINT32 height)
	{
		mMainArea->setSize(width, height);
		mBackgroundArea->setSize(width, height);

		refreshNonClientAreas();
	}

	void GUITabbedTitleBar::tabToggled(CM::UINT32 tabIdx)
	{
		INT32 idx = uniqueIdxToSeqIdx(tabIdx);
		if(idx != -1)
		{
			if(!onTabActivated.empty())
				onTabActivated(idx);
		}

		mActiveTabIdx = tabIdx;
	}

	void GUITabbedTitleBar::tabClosed()
	{
		INT32 idx = uniqueIdxToSeqIdx(mActiveTabIdx);
		if(idx != -1)
		{
			removeTab(idx);

			if(mTabButtons.size() > 0)
				mActiveTabIdx = mTabButtons[0]->getIndex();

			if(!onTabClosed.empty())
				onTabClosed(idx);
		}
	}

	void GUITabbedTitleBar::tabDraggedOff(CM::UINT32 tabIdx)
	{
		INT32 idx = uniqueIdxToSeqIdx(tabIdx);
		if(idx != -1)
		{
			if(!onTabDraggedOff.empty())
				onTabDraggedOff(idx);
		}
	}

	void GUITabbedTitleBar::tabDraggedOn(CM::UINT32 tabIdx)
	{
		INT32 idx = uniqueIdxToSeqIdx(tabIdx);
		if(idx != -1)
		{
			if(!onTabDraggedOn.empty())
				onTabDraggedOn(idx + 1);
		}
	}

	CM::INT32 GUITabbedTitleBar::uniqueIdxToSeqIdx(CM::UINT32 uniqueIdx) const
	{
		UINT32 idx = 0;
		for(auto& tab : mTabButtons)
		{
			if(tab->getIndex() == uniqueIdx)
				return idx;

			idx++;
		}

		return -1;
	}

	void GUITabbedTitleBar::refreshNonClientAreas()
	{
		// If the size or contents of the area changed this frame the layout won't be updated yet,
		// so force the update right away so we get correct element bounds
		mMainArea->_update();

		CM::Vector<CM::Rect>::type nonClientAreas;
		for(auto& elem : mDragDropElements)
		{
			nonClientAreas.push_back(elem->getBounds());
		}

		nonClientAreas.push_back(mLastDropElement->getBounds());

		Platform::setCaptionNonClientAreas(*mParentWidget->getOwnerWindow(), nonClientAreas);
	}
}
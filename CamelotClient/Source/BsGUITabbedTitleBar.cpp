#include "BsGUITabbedTitleBar.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIButton.h"
#include "BsGUIToggle.h"
#include "BsGUISpace.h"
#include "BsGUIWindowMover.h"
#include "BsEngineGUI.h"
#include "BsGUIWidget.h"
#include "CmMath.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	GUITabbedTitleBar::GUITabbedTitleBar(BS::GUIWidget* parent)
		:mLastDropElement(nullptr), mMinBtn(nullptr), mCloseBtn(nullptr), 
		mMainArea(nullptr), mMainLayout(nullptr), mParentWidget(parent), mBackgroundArea(nullptr)
	{
		mBackgroundArea = GUIArea::create(*parent, 0, 0, 1, 13, 9900);
		GUIWindowMover* titleBarBg = GUIWindowMover::create(*parent, parent->getSkin()->getStyle("TitleBarBackground"));
		mBackgroundArea->getLayout().addSpace(1);
		mBackgroundArea->getLayout().addElement(titleBarBg);
		mBackgroundArea->getLayout().addSpace(1);

		mMainArea = GUIArea::create(*parent, 0, 0, 1, 13, 9899);

		GUIWindowMover* dragDropElement = GUIWindowMover::create(*parent, GUILayoutOptions::expandableX(13, 20), parent->getSkin()->getStyle("TabbedBarDropArea"));
		mLastDropElement = dragDropElement;

		mMinBtn = GUIButton::create(*parent, L"", parent->getSkin()->getStyle("WinMinimizeBtn"));
		mCloseBtn = GUIButton::create(*parent, L"", parent->getSkin()->getStyle("WinCloseBtn"));

		mMainArea->getLayout().addSpace(1);
		mMainLayout = &mMainArea->getLayout().addLayoutX();
		mMainLayout->addElement(dragDropElement);
		mMainLayout->addElement(mMinBtn);
		mMainLayout->addSpace(3);
		mMainLayout->addElement(mCloseBtn);
		mMainArea->getLayout().addSpace(3);
	}

	GUITabbedTitleBar::~GUITabbedTitleBar()
	{
		// TODO - Clean up buttons, layouts, areas
	}

	void GUITabbedTitleBar::addTab(const CM::WString& name)
	{
		insertTab((UINT32)mTabButtons.size(), name);
	}

	void GUITabbedTitleBar::insertTab(UINT32 idx, const CM::WString& name)
	{
		GUIToggle* newTabToggle = GUIToggle::create(*mParentWidget, name, EngineGUI::instance().getSkin().getStyle("TabbedBarBtn"));
		GUIWindowMover* newDragDropElement = GUIWindowMover::create(*mParentWidget, EngineGUI::instance().getSkin().getStyle("TabbedBarDropArea"));

		idx = Math::Clamp(idx, 0U, (UINT32)mTabButtons.size());

		mTabButtons.insert(mTabButtons.begin() + idx, newTabToggle);
		mDragDropElements.insert(mDragDropElements.begin() + idx, newDragDropElement);

		mMainLayout->insertElement(idx * 2, newTabToggle);
		mMainLayout->insertElement(idx * 2, newDragDropElement);
	}

	void GUITabbedTitleBar::removeTab(UINT32 idx)
	{
		if(mTabButtons.size() == 0)
			return;

		idx = Math::Clamp(idx, 0U, (UINT32)mTabButtons.size() - 1);

		mMainLayout->removeElement(mTabButtons[idx]);
		mMainLayout->removeElement(mDragDropElements[idx]);

		mTabButtons.erase(mTabButtons.begin() + idx);
		mDragDropElements.erase(mDragDropElements.begin() + idx);
	}

	void GUITabbedTitleBar::setPosition(INT32 x, INT32 y)
	{
		mMainArea->setPosition(x, y);
		mBackgroundArea->setPosition(x, y);
	}

	void GUITabbedTitleBar::setSize(UINT32 width, UINT32 height)
	{
		mMainArea->setSize(width, height);
		mBackgroundArea->setSize(width, height);
	}
}
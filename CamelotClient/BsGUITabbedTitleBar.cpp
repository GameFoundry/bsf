#include "BsGUITabbedTitleBar.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIButton.h"
#include "BsGUIToggle.h"
#include "BsGUIWindowMover.h"
#include "BsEngineGUI.h"
#include "CmMath.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	TabbedTitleBar::TabbedTitleBar(const HSceneObject& parent)
		:GUIWidget(parent), mLastDropElement(nullptr), mMinBtn(nullptr), mCloseBtn(nullptr), mMainArea(nullptr), mMainLayout(nullptr)
	{

	}

	TabbedTitleBar::~TabbedTitleBar()
	{

	}

	void TabbedTitleBar::addTab(const CM::String& name)
	{
		insertTab((UINT32)mTabButtons.size(), name);
	}

	void TabbedTitleBar::insertTab(UINT32 idx, const CM::String& name)
	{
		GUIToggle* newTabToggle = GUIToggle::create(*this, toWString(name), EngineGUI::instance().getSkin().getStyle("TabbedBarBtn"));
		GUIWindowMover* newDragDropElement = GUIWindowMover::create(*this, EngineGUI::instance().getSkin().getStyle("TabbedBarDropArea"));

		idx = Math::Clamp(idx, 0U, (UINT32)mTabButtons.size());

		mTabButtons.insert(mTabButtons.begin() + idx, newTabToggle);
		mDragDropElements.insert(mDragDropElements.begin() + idx, newDragDropElement);

		mMainLayout->insertElement(idx * 2, newTabToggle);
		mMainLayout->insertElement(idx * 2, newDragDropElement);
	}

	void TabbedTitleBar::removeTab(UINT32 idx)
	{
		if(mTabButtons.size() == 0)
			return;

		idx = Math::Clamp(idx, 0U, (UINT32)mTabButtons.size() - 1);

		mMainLayout->removeElement(mTabButtons[idx]);
		mMainLayout->removeElement(mDragDropElements[idx]);

		mTabButtons.erase(mTabButtons.begin() + idx);
		mDragDropElements.erase(mDragDropElements.begin() + idx);
	}

	void TabbedTitleBar::initialize(CM::Viewport* target, CM::RenderWindow* ownerWindow)
	{
		GUIWidget::initialize(target, ownerWindow);

		GUIArea* backgroundArea = GUIArea::create(*this, 0, 1, 0, 13, 500);
		GUIWindowMover* titleBarBg = GUIWindowMover::create(*this, getSkin()->getStyle("TitleBarBackground"));
		GUIFixedSpace& space1 = backgroundArea->getLayout().addSpace(1);
		backgroundArea->getLayout().addElement(titleBarBg);
		GUIFixedSpace& space2 = backgroundArea->getLayout().addSpace(1);

		mMainArea = GUIArea::create(*this, 0, 1, 0, 13, 499);

		GUIWindowMover* dragDropElement = GUIWindowMover::create(*this, GUILayoutOptions::expandableX(13, 20), getSkin()->getStyle("TabbedBarDropArea"));
		mLastDropElement = dragDropElement;

		mMinBtn = GUIButton::create(*this, L"", getSkin()->getStyle("WinMinimizeBtn"));
		mCloseBtn = GUIButton::create(*this, L"", getSkin()->getStyle("WinCloseBtn"));

		GUIFixedSpace& space3 = mMainArea->getLayout().addSpace(1);
		mMainLayout = &mMainArea->getLayout().addLayoutX();
		mMainLayout->addElement(dragDropElement);
		mMainLayout->addElement(mMinBtn);
		GUIFixedSpace& space4 = mMainLayout->addSpace(3);
		mMainLayout->addElement(mCloseBtn);
		GUIFixedSpace& space5 = mMainArea->getLayout().addSpace(3);

		addTab("TEST!");
	}

	void TabbedTitleBar::update()
	{

	}

	bool TabbedTitleBar::_mouseEvent(GUIElement* element, const GUIMouseEvent& ev)
	{
		return GUIWidget::_mouseEvent(element, ev);
	}
}
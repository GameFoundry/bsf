#include "BsGUITabbedTitleBar.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIButton.h"
#include "BsGUIToggle.h"
#include "BsEngineGUI.h"
#include "CmMath.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	TabbedTitleBar::TabbedTitleBar(const HSceneObject& parent)
		:GUIWidget(parent), mLastDropElement(nullptr), mMinBtn(nullptr), mCloseBtn(nullptr), mMainArea(nullptr)
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
		GUIToggle* newTabToggle = GUIToggle::create(*this, GUILayoutOptions::expandableX(13, 100, 200), name);
		GUITexture* newDragDropElement = GUITexture::create(*this, GUIImageScaleMode::StretchToFit, EngineGUI::instance().getSkin().getStyle("TabbedBarDropArea"));

		idx = Math::Clamp(idx, 0U, (UINT32)mTabButtons.size());

		mTabButtons.insert(mTabButtons.begin() + idx, newTabToggle);
		mDragDropElements.insert(mDragDropElements.begin() + idx, newDragDropElement);

		mMainArea->getLayout().insertElement(idx * 2, newTabToggle);
		mMainArea->getLayout().insertElement(idx * 2, newTabToggle);
	}

	void TabbedTitleBar::removeTab(UINT32 idx)
	{
		if(mTabButtons.size() == 0)
			return;

		idx = Math::Clamp(idx, 0U, (UINT32)mTabButtons.size() - 1);

		mMainArea->getLayout().removeElement(mTabButtons[idx]);
		mMainArea->getLayout().removeElement(mDragDropElements[idx]);

		mTabButtons.erase(mTabButtons.begin() + idx);
		mDragDropElements.erase(mDragDropElements.begin() + idx);
	}

	void TabbedTitleBar::initialize(CM::Viewport* target, const CM::RenderWindow* ownerWindow)
	{
		GUIWidget::initialize(target, ownerWindow);

		GUIArea* backgroundArea = GUIArea::create(*this, 0, 0, 0, 0, 500);
		GUITexture* titleBarBg = GUITexture::create(*this, GUIImageScaleMode::StretchToFit, EngineGUI::instance().getSkin().getStyle("TitleBarBackground"));
		backgroundArea->getLayout().addElement(titleBarBg);

		GUIArea* mainArea = GUIArea::create(*this, 0, 0, 0, 0, 499);

		GUITexture* dragDropElement = GUITexture::create(*this, GUILayoutOptions::expandableX(13, 20), GUIImageScaleMode::StretchToFit, EngineGUI::instance().getSkin().getStyle("TabbedBarDropArea"));
		mLastDropElement = dragDropElement;

		mMinBtn = GUIButton::create(*this, "");
		mCloseBtn = GUIButton::create(*this, "");

		mainArea->getLayout().addElement(dragDropElement);
		mainArea->getLayout().addElement(mMinBtn);
		mainArea->getLayout().addElement(mCloseBtn);
	}

	void TabbedTitleBar::update()
	{

	}

	bool TabbedTitleBar::_mouseEvent(GUIElement* element, const GUIMouseEvent& ev)
	{
		return GUIWidget::_mouseEvent(element, ev);
	}
}
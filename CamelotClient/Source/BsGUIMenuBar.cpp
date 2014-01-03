#include "BsGUIMenuBar.h"
#include "BsGUIArea.h"
#include "BsGUIElement.h"
#include "BsGUIButton.h"
#include "BsGUITexture.h"
#include "BsGUILayout.h"
#include "BsGUISpace.h"
#include "BsGUIMenu.h"
#include "BsGUIManager.h"
#include "BsEngineGUI.h"
#include "BsGUIDropDownBoxManager.h"
#include "CmSceneObject.h"
#include "CmPlatform.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const UINT32 GUIMenuBar::NUM_ELEMENTS_AFTER_CONTENT = 8;

	GUIMenuBar::GUIMenuBar(BS::GUIWidget* parent, RenderWindow* parentWindow)
		:mParentWidget(parent), mParentWindow(parentWindow), mMainArea(nullptr), mBackgroundArea(nullptr), 
		mBgTexture(nullptr), mLogoTexture(nullptr), mSubMenuOpen(false), mSubMenuButton(nullptr)
	{
		mBackgroundArea = GUIArea::create(*parent, 0, 0, 1, 13, 9900);
		mMainArea = GUIArea::create(*parent, 0, 0, 1, 13, 9899);

		mBgTexture = GUITexture::create(*parent, GUIImageScaleMode::StretchToFit, GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleHeight()),
			EngineGUI::instance().getSkin().getStyle("MenuBarBg"));
		mBackgroundArea->getLayout().addElement(mBgTexture);

		mLogoTexture = GUITexture::create(*parent, GUIImageScaleMode::StretchToFit, EngineGUI::instance().getSkin().getStyle("MenuBarBansheeLogo"));
		GUILayout& mainLayout = mMainArea->getLayout();

		mainLayout.addElement(mLogoTexture);
		mainLayout.addSpace(5);
		mainLayout.addFlexibleSpace();

		mMinBtn = GUIButton::create(*parent, HString(L""), parent->getSkin().getStyle("WinMinimizeBtn"));
		mMaxBtn = GUIButton::create(*parent, HString(L""), parent->getSkin().getStyle("WinMaximizeBtn"));
		mCloseBtn = GUIButton::create(*parent, HString(L""), parent->getSkin().getStyle("WinCloseBtn"));

		mainLayout.addSpace(3);
		mainLayout.addElement(mMinBtn);
		mainLayout.addSpace(3);
		mainLayout.addElement(mMaxBtn);
		mainLayout.addSpace(3);
		mainLayout.addElement(mCloseBtn);
		mainLayout.addSpace(3);

		mMinBtn->onClick.connect(boost::bind(&GUIMenuBar::onMinimizeClicked, this));
		mMaxBtn->onClick.connect(boost::bind(&GUIMenuBar::onMaximizeClicked, this));
		mCloseBtn->onClick.connect(boost::bind(&GUIMenuBar::onCloseClicked, this));

		refreshNonClientAreas();
	}

	GUIMenuBar::~GUIMenuBar()
	{
		closeSubMenu();

		for(auto& menu : mChildMenus)
		{
			cm_delete<PoolAlloc>(menu.menu);
			GUIElement::destroy(menu.button);
		}

		GUIElement::destroy(mMinBtn);
		GUIElement::destroy(mMaxBtn);
		GUIElement::destroy(mCloseBtn);

		GUIElement::destroy(mBgTexture);
		GUIElement::destroy(mLogoTexture);

		GUIArea::destroy(mMainArea);
		GUIArea::destroy(mBackgroundArea);
	}

	void GUIMenuBar::setArea(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height)
	{
		mMainArea->setPosition(x, y);
		mBackgroundArea->setPosition(x, y);

		mMainArea->setSize(width, height);
		mBackgroundArea->setSize(width, height);

		refreshNonClientAreas();
	}

	const GUIMenuItem* GUIMenuBar::addMenuItem(const CM::WString& path, std::function<void()> callback)
	{
		WString strippedPath = path;
		WString rootName;

		if(!stripPath(strippedPath, rootName))
			return nullptr;

		const GUIMenuBarData* subMenu = getSubMenu(rootName);
		if(subMenu == nullptr)
		{
			subMenu = addNewButton(rootName);

			refreshNonClientAreas();
		}

		return subMenu->menu->addMenuItem(strippedPath, callback);
	}

	const GUIMenuItem* GUIMenuBar::addSeparator(const CM::WString& path)
	{
		WString strippedPath = path;
		WString rootName;

		if(!stripPath(strippedPath, rootName))
			return nullptr;

		const GUIMenuBarData* subMenu = getSubMenu(rootName);
		if(subMenu == nullptr)
		{
			subMenu = addNewButton(rootName);

			refreshNonClientAreas();
		}

		return subMenu->menu->addSeparator(strippedPath);
	}

	GUIMenuBar::GUIMenuBarData* GUIMenuBar::addNewButton(const CM::WString& name)
	{
		mChildMenus.push_back(GUIMenuBarData());

		GUIMenuBarData& newSubMenu = mChildMenus.back();
		newSubMenu.name = name;
		newSubMenu.menu = cm_new<GUIMenu>();

		GUIButton* newButton = GUIButton::create(*mParentWidget, HString(name), EngineGUI::instance().getSkin().getStyle("MenuBarBtn"));
		newButton->onClick.connect(boost::bind(&GUIMenuBar::openSubMenu, this, name));
		newButton->onHover.connect(boost::bind(&GUIMenuBar::onSubMenuHover, this, name));
		mMainArea->getLayout().insertElement(mMainArea->getLayout().getNumChildren() - NUM_ELEMENTS_AFTER_CONTENT, newButton);

		newSubMenu.button = newButton;

		return &newSubMenu;
	}

	const GUIMenuItem* GUIMenuBar::getMenuItem(const CM::WString& path) const
	{
		WString strippedPath = path;
		WString rootName;

		if(!stripPath(strippedPath, rootName))
			return nullptr;

		const GUIMenuBarData* subMenu = getSubMenu(rootName);
		if(subMenu == nullptr)
			return nullptr;

		return subMenu->menu->getMenuItem(strippedPath);
	}

	void GUIMenuBar::removeMenuItem(const CM::WString& path)
	{
		WString strippedPath = path;
		WString rootName;

		if(!stripPath(strippedPath, rootName))
			return;

		if(strippedPath == L"")
		{
			UINT32 curIdx = 0;
			GUIMenuBarData* subMenuToRemove = nullptr;
			for(auto& subMenuData : mChildMenus)
			{
				if(subMenuData.name == rootName)
				{
					subMenuToRemove = &subMenuData;
					break;
				}

				curIdx++;
			}

			if(subMenuToRemove == nullptr)
				return;

			mMainArea->getLayout().removeElement(subMenuToRemove->button);
			GUIElement::destroy(subMenuToRemove->button);
			cm_delete<PoolAlloc>(subMenuToRemove->menu);

			mChildMenus.erase(mChildMenus.begin() + curIdx);

			refreshNonClientAreas();
			return;
		}

		const GUIMenuBarData* subMenu = getSubMenu(rootName);
		if(subMenu == nullptr)
			return;

		const GUIMenuItem* item = subMenu->menu->getMenuItem(strippedPath);
		if(item == nullptr)
			return;

		subMenu->menu->removeMenuItem(item);
	}

	const GUIMenuBar::GUIMenuBarData* GUIMenuBar::getSubMenu(const CM::WString& name) const
	{
		for(auto& subMenu : mChildMenus)
		{
			if(subMenu.name == name)
				return &subMenu;
		}

		return nullptr;
	}

	bool GUIMenuBar::stripPath(CM::WString& path, CM::WString& pathRoot) const
	{
		Vector<WString>::type pathElements = StringUtil::split(path, L"/");
		if(pathElements.size() == 0)
			return false;

		pathRoot = pathElements[0];
		path.erase(0, pathRoot.size());

		if(path.size() > 0)
			path.erase(0, 1); // Forward slash

		return true;
	}

	void GUIMenuBar::openSubMenu(const CM::WString& name)
	{
		const GUIMenuBarData* subMenu = getSubMenu(name);
		if(subMenu == nullptr)
			return;

		if(mSubMenuOpen)
		{
			bool closingExisting = subMenu->button == mSubMenuButton;

			closeSubMenu();

			if(closingExisting)
				return;
		}

		GUIDropDownData dropDownData = subMenu->menu->getDropDownData();
		GUIWidget& widget = subMenu->button->_getParentWidget();

		GUIDropDownAreaPlacement placement = GUIDropDownAreaPlacement::aroundBoundsHorz(subMenu->button->getBounds());

		GameObjectHandle<GUIDropDownBox> dropDownBox = GUIDropDownBoxManager::instance().openDropDownBox(widget.getTarget(), 
			placement, dropDownData, widget.getSkin(), GUIDropDownType::MenuBar, boost::bind(&GUIMenuBar::onSubMenuClosed, this));

		GUIManager::instance().enableSelectiveInput(boost::bind(&GUIMenuBar::closeSubMenu, this));
		GUIManager::instance().addSelectiveInputWidget(dropDownBox.get());

		for(auto& childMenu : mChildMenus)
		{
			GUIManager::instance().addSelectiveInputElement(childMenu.button);
		}

		subMenu->button->_setOn(true);

		mSubMenuButton = subMenu->button;
		mSubMenuOpen = true;
	}

	void GUIMenuBar::closeSubMenu()
	{
		if(mSubMenuOpen)
		{
			GUIDropDownBoxManager::instance().closeDropDownBox();
			GUIManager::instance().disableSelectiveInput();
			
			mSubMenuButton->_setOn(false);
			mSubMenuOpen = false;
		}		
	}

	void GUIMenuBar::onSubMenuHover(const CM::WString& name)
	{
		if(mSubMenuOpen)
		{
			const GUIMenuBarData* subMenu = getSubMenu(name);

			if(subMenu == nullptr)
				return;

			if(mSubMenuButton != subMenu->button)
				openSubMenu(name);
		}
	}

	void GUIMenuBar::onSubMenuClosed()
	{
		GUIManager::instance().disableSelectiveInput();

		mSubMenuButton->_setOn(false);
		mSubMenuOpen = false;
	}

	void GUIMenuBar::onMinimizeClicked()
	{
		// TODO
	}

	void GUIMenuBar::onMaximizeClicked()
	{
		// TODO
	}

	void GUIMenuBar::onCloseClicked()
	{
		// TODO
	}

	void GUIMenuBar::refreshNonClientAreas()
	{
		// If the size or contents of the area changed this frame the layout won't be updated yet,
		// so force the update right away so we get correct element bounds
		mMainArea->_update();

		CM::Vector<CM::RectI>::type nonClientAreas;
		nonClientAreas.push_back(mLogoTexture->getBounds());

		if(mChildMenus.size() > 0)
		{
			RectI lastButtonBounds = mChildMenus.back().button->getBounds();
			RectI minButtonBounds = mMinBtn->getBounds();

			RectI emptyArea(lastButtonBounds.x + lastButtonBounds.width, mMainArea->y(), 
				minButtonBounds.x - (lastButtonBounds.x + lastButtonBounds.width), mMainArea->height());

			nonClientAreas.push_back(emptyArea);
		}

		Platform::setCaptionNonClientAreas(*mParentWindow, nonClientAreas);
	}
}
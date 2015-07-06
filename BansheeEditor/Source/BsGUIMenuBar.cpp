#include "BsGUIMenuBar.h"
#include "BsGUIPanel.h"
#include "BsGUIElement.h"
#include "BsGUIButton.h"
#include "BsGUITexture.h"
#include "BsGUILayoutX.h"
#include "BsGUISpace.h"
#include "BsGUIMenu.h"
#include "BsGUIManager.h"
#include "BsBuiltinResources.h"
#include "BsGUIDropDownBoxManager.h"
#include "BsSceneObject.h"
#include "BsPlatform.h"
#include "BsCoreThread.h"
#include <BsShortcutManager.h>

namespace BansheeEngine
{
	const UINT32 GUIMenuBar::NUM_ELEMENTS_AFTER_CONTENT = 8;

	GUIMenuBar::GUIMenuBar(GUIWidget* parent, RenderWindow* parentWindow)
		:mParentWidget(parent), mParentWindow(parentWindow), mMainPanel(nullptr), mMainLayout(nullptr),
		mBgTexture(nullptr), mLogoTexture(nullptr), mSubMenuOpen(false), mSubMenuButton(nullptr), mBgPanel(nullptr)
	{
		mMainPanel = parent->getPanel()->addNewElement<GUIPanel>(std::numeric_limits<INT16>::min() + 10);
		mMainPanel->setWidth(1);
		mMainPanel->setHeight(13);

		mBgPanel = parent->getPanel()->addNewElement<GUIPanel>(std::numeric_limits<INT16>::min() + 11);
		mBgPanel->setWidth(1);
		mBgPanel->setHeight(13);

		mMainLayout = mMainPanel->addNewElement<GUILayoutX>();
		GUILayoutX* bgLayout = mBgPanel->addNewElement<GUILayoutX>();

		mBgTexture = GUITexture::create(GUIImageScaleMode::StretchToFit, GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleHeight()), "MenuBarBg");
		bgLayout->addElement(mBgTexture);

		mLogoTexture = GUITexture::create(GUIImageScaleMode::StretchToFit, "MenuBarBansheeLogo");

		mMainLayout->addElement(mLogoTexture);
		mMainLayout->addNewElement<GUIFixedSpace>(5);
		mMainLayout->addNewElement<GUIFlexibleSpace>();

		mMinBtn = GUIButton::create(HString(L""), "WinMinimizeBtn");
		mMaxBtn = GUIButton::create(HString(L""), "WinMaximizeBtn");
		mCloseBtn = GUIButton::create(HString(L""), "WinCloseBtn");

		mMainLayout->addNewElement<GUIFixedSpace>(3);
		mMainLayout->addElement(mMinBtn);
		mMainLayout->addNewElement<GUIFixedSpace>(3);
		mMainLayout->addElement(mMaxBtn);
		mMainLayout->addNewElement<GUIFixedSpace>(3);
		mMainLayout->addElement(mCloseBtn);
		mMainLayout->addNewElement<GUIFixedSpace>(3);

		mMinBtn->onClick.connect(std::bind(&GUIMenuBar::onMinimizeClicked, this));
		mMaxBtn->onClick.connect(std::bind(&GUIMenuBar::onMaximizeClicked, this));
		mCloseBtn->onClick.connect(std::bind(&GUIMenuBar::onCloseClicked, this));

		refreshNonClientAreas();
	}

	GUIMenuBar::~GUIMenuBar()
	{
		closeSubMenu();

		for(auto& menu : mChildMenus)
		{
			bs_delete<PoolAlloc>(menu.menu);
			GUIElement::destroy(menu.button);
		}

		GUILayout::destroy(mMainPanel);
		GUILayout::destroy(mBgPanel);
	}

	void GUIMenuBar::setArea(INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		mMainPanel->setPosition(x, y);

		mMainPanel->setWidth(width);
		mMainPanel->setHeight(height);

		mBgPanel->setPosition(x, y);

		mBgPanel->setWidth(width);
		mBgPanel->setHeight(height);

		refreshNonClientAreas();
	}

	const GUIMenuItem* GUIMenuBar::addMenuItem(const WString& path, std::function<void()> callback, 
		INT32 priority, const ShortcutKey& shortcut)
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

		if (shortcut.isValid() && callback != nullptr)
			registerShortcut(path, shortcut, callback);

		return subMenu->menu->addMenuItem(strippedPath, callback, priority, shortcut);
	}

	const GUIMenuItem* GUIMenuBar::addSeparator(const WString& path, INT32 priority)
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

		return subMenu->menu->addSeparator(strippedPath, priority);
	}

	GUIMenuBar::GUIMenuBarData* GUIMenuBar::addNewButton(const WString& name)
	{
		mChildMenus.push_back(GUIMenuBarData());

		GUIMenuBarData& newSubMenu = mChildMenus.back();
		newSubMenu.name = name;
		newSubMenu.menu = bs_new<GUIMenu>();

		GUIButton* newButton = GUIButton::create(HString(name), "MenuBarBtn");
		newButton->onClick.connect(std::bind(&GUIMenuBar::openSubMenu, this, name));
		newButton->onHover.connect(std::bind(&GUIMenuBar::onSubMenuHover, this, name));
		mMainLayout->insertElement(mMainLayout->getNumChildren() - NUM_ELEMENTS_AFTER_CONTENT, newButton);

		newSubMenu.button = newButton;

		return &newSubMenu;
	}

	const GUIMenuItem* GUIMenuBar::getMenuItem(const WString& path) const
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

	void GUIMenuBar::removeMenuItem(const WString& path)
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

			mMainLayout->removeElement(subMenuToRemove->button);
			GUIElement::destroy(subMenuToRemove->button);
			bs_delete<PoolAlloc>(subMenuToRemove->menu);

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

	const GUIMenuBar::GUIMenuBarData* GUIMenuBar::getSubMenu(const WString& name) const
	{
		for(auto& subMenu : mChildMenus)
		{
			if(subMenu.name == name)
				return &subMenu;
		}

		return nullptr;
	}

	void GUIMenuBar::registerShortcut(const WString& path, const ShortcutKey& shortcut, std::function<void()> callback)
	{
		ShortcutManager::instance().addShortcut(shortcut, callback);

		WString trimmedPath = path;
		StringUtil::trim(trimmedPath, L"/\\", false, true);

		mEntryShortcuts[trimmedPath] = shortcut;
	}

	void GUIMenuBar::unregisterShortcut(const WString& path)
	{
		WString trimmedPath = path;
		StringUtil::trim(trimmedPath, L"/\\", false, true);

		auto findIter = mEntryShortcuts.find(trimmedPath);
		if (findIter != mEntryShortcuts.end())
		{
			ShortcutManager::instance().removeShortcut(findIter->second);
			mEntryShortcuts.erase(findIter);
		}
	}

	bool GUIMenuBar::stripPath(WString& path, WString& pathRoot) const
	{
		Vector<WString> pathElements = StringUtil::split(path, L"/");
		if(pathElements.size() == 0)
			return false;

		pathRoot = pathElements[0];
		path.erase(0, pathRoot.size());

		if(path.size() > 0)
			path.erase(0, 1); // Forward slash

		return true;
	}

	void GUIMenuBar::openSubMenu(const WString& name)
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
		GUIWidget* widget = subMenu->button->_getParentWidget();

		DropDownAreaPlacement placement = DropDownAreaPlacement::aroundBoundsHorz(subMenu->button->_getLayoutData().area);

		GameObjectHandle<GUIDropDownMenu> dropDownBox = GUIDropDownBoxManager::instance().openDropDownBox(widget->getTarget(), 
			placement, dropDownData, widget->getSkinResource(), GUIDropDownType::MenuBar, std::bind(&GUIMenuBar::onSubMenuClosed, this));

		subMenu->button->_setOn(true);

		mSubMenuButton = subMenu->button;
		mSubMenuOpen = true;
	}

	void GUIMenuBar::closeSubMenu()
	{
		if(mSubMenuOpen)
		{
			GUIDropDownBoxManager::instance().closeDropDownBox();

			mSubMenuButton->_setOn(false);
			mSubMenuOpen = false;
		}		
	}

	void GUIMenuBar::onSubMenuHover(const WString& name)
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
		mSubMenuButton->_setOn(false);
		mSubMenuOpen = false;
	}

	void GUIMenuBar::onMinimizeClicked()
	{
		mParentWindow->minimize(gCoreAccessor());
	}

	void GUIMenuBar::onMaximizeClicked()
	{
		if(mParentWindow->getProperties().isMaximized())
			mParentWindow->restore(gCoreAccessor());
		else
			mParentWindow->maximize(gCoreAccessor());
	}

	void GUIMenuBar::onCloseClicked()
	{
		gCoreApplication().stopMainLoop();
	}

	void GUIMenuBar::refreshNonClientAreas()
	{
		Rect2I mainArea = mMainLayout->getBounds();

		Vector<Rect2I> nonClientAreas;
		nonClientAreas.push_back(mLogoTexture->getBounds());

		if(mChildMenus.size() > 0)
		{
			Rect2I lastButtonBounds = mChildMenus.back().button->getBounds();
			Rect2I minButtonBounds = mMinBtn->getBounds();

			Rect2I emptyArea(lastButtonBounds.x + lastButtonBounds.width, mainArea.y,
				minButtonBounds.x - (lastButtonBounds.x + lastButtonBounds.width), mainArea.height);

			nonClientAreas.push_back(emptyArea);
		}

		Platform::setCaptionNonClientAreas(*mParentWindow->getCore(), nonClientAreas);
	}
}
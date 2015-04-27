#include "BsGUIMenuBar.h"
#include "BsGUIArea.h"
#include "BsGUIElement.h"
#include "BsGUIButton.h"
#include "BsGUITexture.h"
#include "BsGUILayout.h"
#include "BsGUISpace.h"
#include "BsGUIMenu.h"
#include "BsGUIManager.h"
#include "BsBuiltinResources.h"
#include "BsGUIDropDownBoxManager.h"
#include "BsSceneObject.h"
#include "BsPlatform.h"
#include "BsCoreThread.h"

namespace BansheeEngine
{
	const UINT32 GUIMenuBar::NUM_ELEMENTS_AFTER_CONTENT = 8;

	GUIMenuBar::GUIMenuBar(GUIWidget* parent, RenderWindow* parentWindow)
		:mParentWidget(parent), mParentWindow(parentWindow), mMainArea(nullptr), mBgTextureArea(nullptr), 
		mBgTexture(nullptr), mLogoTexture(nullptr), mSubMenuOpen(false), mSubMenuButton(nullptr)
	{
		mBgTextureArea = GUIArea::create(*parent, 0, 0, 1, 13, 1);
		mMainArea = GUIArea::create(*parent, 0, 0, 1, 13, 0);

		mBgTexture = GUITexture::create(GUIImageScaleMode::StretchToFit, GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleHeight()), "MenuBarBg");
		mBgTextureArea->getLayout().addElement(mBgTexture);

		mLogoTexture = GUITexture::create(GUIImageScaleMode::StretchToFit, "MenuBarBansheeLogo");
		GUILayout& mainLayout = mMainArea->getLayout();

		mainLayout.addElement(mLogoTexture);
		mainLayout.addNewElement<GUIFixedSpace>(5);
		mainLayout.addNewElement<GUIFlexibleSpace>();

		mMinBtn = GUIButton::create(HString(L""), "WinMinimizeBtn");
		mMaxBtn = GUIButton::create(HString(L""), "WinMaximizeBtn");
		mCloseBtn = GUIButton::create(HString(L""), "WinCloseBtn");

		mainLayout.addNewElement<GUIFixedSpace>(3);
		mainLayout.addElement(mMinBtn);
		mainLayout.addNewElement<GUIFixedSpace>(3);
		mainLayout.addElement(mMaxBtn);
		mainLayout.addNewElement<GUIFixedSpace>(3);
		mainLayout.addElement(mCloseBtn);
		mainLayout.addNewElement<GUIFixedSpace>(3);

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

		GUIElement::destroy(mMinBtn);
		GUIElement::destroy(mMaxBtn);
		GUIElement::destroy(mCloseBtn);

		GUIElement::destroy(mBgTexture);
		GUIElement::destroy(mLogoTexture);

		GUIArea::destroy(mMainArea);
		GUIArea::destroy(mBgTextureArea);
	}

	void GUIMenuBar::setArea(INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		mMainArea->setPosition(x, y);
		mBgTextureArea->setPosition(x, y);

		mMainArea->setSize(width, height);
		mBgTextureArea->setSize(width, height);

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
		mMainArea->getLayout().insertElement(mMainArea->getLayout().getNumChildren() - NUM_ELEMENTS_AFTER_CONTENT, newButton);

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

			mMainArea->getLayout().removeElement(subMenuToRemove->button);
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

		GUIDropDownAreaPlacement placement = GUIDropDownAreaPlacement::aroundBoundsHorz(subMenu->button->_getCachedBounds());

		GameObjectHandle<GUIDropDownBox> dropDownBox = GUIDropDownBoxManager::instance().openDropDownBox(widget->getTarget(), 
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
		// If the size or contents of the area changed this frame the layout won't be updated yet,
		// so force the update right away so we get correct element bounds
		mMainArea->_update();

		Vector<Rect2I> nonClientAreas;
		nonClientAreas.push_back(mLogoTexture->_getCachedBounds());

		if(mChildMenus.size() > 0)
		{
			Rect2I lastButtonBounds = mChildMenus.back().button->_getCachedBounds();
			Rect2I minButtonBounds = mMinBtn->_getCachedBounds();

			Rect2I emptyArea(lastButtonBounds.x + lastButtonBounds.width, mMainArea->y(), 
				minButtonBounds.x - (lastButtonBounds.x + lastButtonBounds.width), mMainArea->height());

			nonClientAreas.push_back(emptyArea);
		}

		Platform::setCaptionNonClientAreas(*mParentWindow->getCore(), nonClientAreas);
	}
}
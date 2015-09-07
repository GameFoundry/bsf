#include "BsGUIMenuBar.h"
#include "BsGUIPanel.h"
#include "BsGUIElement.h"
#include "BsGUIButton.h"
#include "BsGUITexture.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUISpace.h"
#include "BsGUIMenu.h"
#include "BsGUIManager.h"
#include "BsBuiltinResources.h"
#include "BsGUIDropDownBoxManager.h"
#include "BsSceneObject.h"
#include "BsPlatform.h"
#include "BsCoreThread.h"
#include "BsShortcutManager.h"
#include "BsGUIHoverHitBox.h"

namespace BansheeEngine
{
	const UINT32 GUIMenuBar::NUM_ELEMENTS_AFTER_CONTENT = 8;
	const UINT32 GUIMenuBar::ELEMENT_SPACING = 10;

	const String& GUIMenuBar::getBackgroundStyleType()
	{
		static const String type = "MenuBarBg";
		return type;
	}

	const String& GUIMenuBar::getLineStyleType()
	{
		static const String type = "MenuBarLine";
		return type;
	}

	const String& GUIMenuBar::getLogoStyleType()
	{
		static const String type = "MenuBarBansheeLogo";
		return type;
	}

	const String& GUIMenuBar::getMenuItemButtonStyleType()
	{
		static const String type = "MenuBarBtn";
		return type;
	}

	GUIMenuBar::GUIMenuBar(CGUIWidget* parent, RenderWindow* parentWindow)
		:mParentWidget(parent), mParentWindow(parentWindow), mMainPanel(nullptr), mMenuItemLayout(nullptr),
		mBgTexture(nullptr), mLogoTexture(nullptr), mSubMenuOpen(false), mSubMenuButton(nullptr), mBgPanel(nullptr)
	{
		mOverlayPanel = parent->getPanel()->addNewElement<GUIPanel>(std::numeric_limits<INT16>::min() + 10);
		mOverlayPanel->setWidth(1);
		mOverlayPanel->setHeight(50);

		mMainPanel = parent->getPanel()->addNewElement<GUIPanel>(std::numeric_limits<INT16>::min() + 15);
		mMainPanel->setWidth(1);
		mMainPanel->setHeight(50);

		mMainPanel = parent->getPanel()->addNewElement<GUIPanel>(std::numeric_limits<INT16>::min() + 15);
		mMainPanel->setWidth(1);
		mMainPanel->setHeight(50);

		mBgPanel = parent->getPanel()->addNewElement<GUIPanel>(std::numeric_limits<INT16>::min() + 25);
		mBgPanel->setWidth(1);
		mBgPanel->setHeight(50);

		mBgTexture = GUITexture::create(GUIImageScaleMode::StretchToFit,
			GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleHeight()), getBackgroundStyleType());
		
		GUILayoutX* bgLayout = mBgPanel->addNewElement<GUILayoutX>();
		bgLayout->addElement(mBgTexture);

		mLogoTexture = GUITexture::create(GUIImageScaleMode::StretchToFit, getLogoStyleType());
		mMinBtn = GUIButton::create(HString(L""), "WinMinimizeBtn");
		mMaxBtn = GUIButton::create(HString(L""), "WinMaximizeBtn");
		mCloseBtn = GUIButton::create(HString(L""), "WinCloseBtn");
		mSplitterLine = GUITexture::create(GUIImageScaleMode::StretchToFit, getLineStyleType());

		GUILayout* mainLayout = mMainPanel->addNewElement<GUILayoutX>();
		mainLayout->addElement(mLogoTexture);
		mainLayout->addNewElement<GUIFixedSpace>(5);

		GUILayout* mainLayoutVert = mainLayout->addNewElement<GUILayoutY>();
		mMenuItemLayout = mainLayoutVert->addNewElement<GUILayoutX>();
		mainLayoutVert->addElement(mSplitterLine);
		mainLayoutVert->addNewElement<GUIFlexibleSpace>(); // Note: Insert layout for toolbar buttons here
		
		mMenuItemLayout->addNewElement<GUIFlexibleSpace>();
		mMenuItemLayout->addNewElement<GUIFixedSpace>(3);
		mMenuItemLayout->addElement(mMinBtn);
		mMenuItemLayout->addNewElement<GUIFixedSpace>(3);
		mMenuItemLayout->addElement(mMaxBtn);
		mMenuItemLayout->addNewElement<GUIFixedSpace>(3);
		mMenuItemLayout->addElement(mCloseBtn);
		mMenuItemLayout->addNewElement<GUIFixedSpace>(3);

		mMinBtn->onClick.connect(std::bind(&GUIMenuBar::onMinimizeClicked, this));
		mMaxBtn->onClick.connect(std::bind(&GUIMenuBar::onMaximizeClicked, this));
		mCloseBtn->onClick.connect(std::bind(&GUIMenuBar::onCloseClicked, this));

		mHoverHitBox = GUIHoverHitBox::create();
		mOverlayPanel->addElement(mHoverHitBox);

		mHoverHitBox->onHover.connect(std::bind(&GUIMenuBar::onMenuBarHover, this));
		mHoverHitBox->onOut.connect(std::bind(&GUIMenuBar::onMenuBarOut, this));

		refreshNonClientAreas();
	}

	GUIMenuBar::~GUIMenuBar()
	{
		closeSubMenu();

		for(auto& menu : mChildMenus)
		{
			bs_delete(menu.menu);
			GUIElement::destroy(menu.button);
			GUIFixedSpace::destroy(menu.space);
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

		GUIFixedSpace* space = mMenuItemLayout->insertNewElement<GUIFixedSpace>(mMenuItemLayout->getNumChildren() - NUM_ELEMENTS_AFTER_CONTENT, ELEMENT_SPACING);
		mMenuItemLayout->insertElement(mMenuItemLayout->getNumChildren() - NUM_ELEMENTS_AFTER_CONTENT, newButton);

		newSubMenu.button = newButton;
		newSubMenu.space = space;

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

			mMenuItemLayout->removeElement(subMenuToRemove->button);
			mMenuItemLayout->removeElement(subMenuToRemove->space);
			GUIElement::destroy(subMenuToRemove->button);
			GUIFixedSpace::destroy(subMenuToRemove->space);
			bs_delete(subMenuToRemove->menu);

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

		CGUIWidget* widget = subMenu->button->_getParentWidget();

		DROP_DOWN_BOX_DESC desc;
		desc.camera = widget->getCamera();
		desc.skin = widget->getSkinResource();
		desc.placement = DropDownAreaPlacement::aroundBoundsHorz(subMenu->button->_getLayoutData().area);
		desc.dropDownData = subMenu->menu->getDropDownData();

		for (auto& childMenu : mChildMenus)
		{
			Rect2I bounds = childMenu.button->getBounds();
			desc.additionalBounds.push_back(bounds);
		}
		
		GameObjectHandle<GUIDropDownMenu> dropDownBox = GUIDropDownBoxManager::instance().openDropDownBox(
			desc, GUIDropDownType::MenuBar, std::bind(&GUIMenuBar::onSubMenuClosed, this));

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

		setActiveState(true);
	}

	void GUIMenuBar::onSubMenuClosed()
	{
		mSubMenuButton->_setOn(false);
		mSubMenuOpen = false;

		setActiveState(false);
	}

	void GUIMenuBar::onMenuBarHover()
	{
		setActiveState(true);
	}

	void GUIMenuBar::onMenuBarOut()
	{
		if (!mSubMenuOpen)
			setActiveState(false);
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

	void GUIMenuBar::setActiveState(bool active)
	{
		const GUIElementStyle* style = mParentWidget->getSkin().getStyle(getLineStyleType());

		if (active)
			mSplitterLine->setTexture(style->normalOn.texture);
		else
			mSplitterLine->setTexture(style->normal.texture);
	}

	void GUIMenuBar::refreshNonClientAreas()
	{
		Rect2I mainArea = mMenuItemLayout->getBounds();

		Vector<Rect2I> nonClientAreas;
		nonClientAreas.push_back(mLogoTexture->getBounds());

		UINT32 menuWidth = 0;
		if(mChildMenus.size() > 0)
		{
			Rect2I lastButtonBounds = mChildMenus.back().button->getBounds();
			Rect2I minButtonBounds = mMinBtn->getBounds();
			menuWidth = lastButtonBounds.x + lastButtonBounds.width;

			Rect2I emptyArea(menuWidth, mainArea.y,
				minButtonBounds.x - (lastButtonBounds.x + lastButtonBounds.width), mainArea.height);

			nonClientAreas.push_back(emptyArea);
		}

		Platform::setCaptionNonClientAreas(*mParentWindow->getCore(), nonClientAreas);

		Rect2I menuBarBounds = mMenuItemLayout->getBounds();
		menuBarBounds.width = menuWidth;

		mHoverHitBox->setBounds(menuBarBounds);
	}
}
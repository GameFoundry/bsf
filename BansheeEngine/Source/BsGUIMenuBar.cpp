#include "BsGUIMenuBar.h"
#include "BsGUIArea.h"
#include "BsGUIElement.h"
#include "BsGUIButton.h"
#include "BsGUILayout.h"
#include "BsGUIMenu.h"
#include "BsGUIManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIMenuBar::GUIMenuBar(BS::GUIWidget* parent)
		:mParentWidget(parent), mMainArea(nullptr), mBackgroundArea(nullptr)
	{
		mBackgroundArea = GUIArea::create(*parent, 0, 0, 1, 13, 9900);
		mMainArea = GUIArea::create(*parent, 0, 0, 1, 13, 9899);
	}

	GUIMenuBar::~GUIMenuBar()
	{
		for(auto& menu : mChildMenus)
		{
			cm_delete<PoolAlloc>(menu.menu);
			GUIElement::destroy(menu.button);
		}

		GUIArea::destroy(mMainArea);
		GUIArea::destroy(mBackgroundArea);
	}

	void GUIMenuBar::setArea(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height)
	{
		mMainArea->setPosition(x, y);
		mBackgroundArea->setPosition(x, y);

		mMainArea->setSize(width, height);
		mBackgroundArea->setSize(width, height);
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
			mChildMenus.push_back(GUIMenuBarData());

			GUIMenuBarData& newSubMenu = mChildMenus.back();
			newSubMenu.name = rootName;
			newSubMenu.menu = cm_new<GUIMenu>();

			GUIButton* newButton = GUIButton::create(*mParentWidget, rootName);
			mMainArea->getLayout().addElement(newButton);

			newSubMenu.button = newButton;

			subMenu = &newSubMenu;
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
			mChildMenus.push_back(GUIMenuBarData());

			GUIMenuBarData& newSubMenu = mChildMenus.back();
			newSubMenu.name = rootName;
			newSubMenu.menu = cm_new<GUIMenu>();

			GUIButton* newButton = GUIButton::create(*mParentWidget, rootName);
			newButton->onClick.connect(boost::bind(&GUIMenuBar::openSubMenu, this, rootName));
			mMainArea->getLayout().addElement(newButton);

			newSubMenu.button = newButton;

			subMenu = &newSubMenu;
		}

		return subMenu->menu->addSeparator(strippedPath);
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

		GUIManager::instance().openMenuBarMenu(subMenu->button, subMenu->menu);
	}
}
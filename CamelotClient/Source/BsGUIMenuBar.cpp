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

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	GUIMenuBar::GUIMenuBar(BS::GUIWidget* parent)
		:mParentWidget(parent), mMainArea(nullptr), mBackgroundArea(nullptr), mBgTexture(nullptr), mLogoTexture(nullptr)
	{
		mBackgroundArea = GUIArea::create(*parent, 0, 0, 1, 13, 9900);
		mMainArea = GUIArea::create(*parent, 0, 0, 1, 13, 9899);

		mBgTexture = GUITexture::create(*parent, GUILayoutOptions::expandableXY(), GUIImageScaleMode::StretchToFit, EngineGUI::instance().getSkin().getStyle("MenuBarBg"));
		mBackgroundArea->getLayout().addElement(mBgTexture);

		mLogoTexture = GUITexture::create(*parent, GUIImageScaleMode::StretchToFit, EngineGUI::instance().getSkin().getStyle("MenuBarBansheeLogo"));
		mMainArea->getLayout().addElement(mLogoTexture);
		mMainArea->getLayout().addSpace(5);
		mMainArea->getLayout().addFlexibleSpace();
	}

	GUIMenuBar::~GUIMenuBar()
	{
		for(auto& menu : mChildMenus)
		{
			cm_delete<PoolAlloc>(menu.menu);
			GUIElement::destroy(menu.button);
		}

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

			GUIButton* newButton = GUIButton::create(*mParentWidget, rootName, EngineGUI::instance().getSkin().getStyle("MenuBarBtn"));
			newButton->onClick.connect(boost::bind(&GUIMenuBar::openSubMenu, this, rootName));
			mMainArea->getLayout().insertElement(mMainArea->getLayout().getNumChildren() - 1, newButton);

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

			GUIButton* newButton = GUIButton::create(*mParentWidget, rootName, EngineGUI::instance().getSkin().getStyle("MenuBarBtn"));
			newButton->onClick.connect(boost::bind(&GUIMenuBar::openSubMenu, this, rootName));
			mMainArea->getLayout().insertElement(mMainArea->getLayout().getNumChildren() - 1, newButton);

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

		GUIManager::instance().openMenuBarMenu(subMenu->button, subMenu->menu);

		for(auto& childMenu : mChildMenus)
		{
			GUIManager::instance().addSelectiveInputElement(childMenu.button);
		}
	}
}
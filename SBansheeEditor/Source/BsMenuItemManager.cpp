#include "BsMenuItemManager.h"
#include "BsScriptAssemblyManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoField.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsScriptObjectManager.h"
#include "BsShortcutKey.h"
#include "BsEditorWindowManager.h"
#include "BsMainEditorWindow.h"
#include "BsGUIMenuBar.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	MenuItemManager::MenuItemManager(ScriptAssemblyManager& scriptObjectManager)
		:mScriptObjectManager(scriptObjectManager), mMenuItemAttribute(nullptr), mPathField(nullptr), 
		mShortcutField(nullptr), mPriorityField(nullptr)
	{
		mDomainLoadedConn = ScriptObjectManager::instance().onRefreshDomainLoaded.connect(std::bind(&MenuItemManager::reloadAssemblyData, this));
		reloadAssemblyData();
	}

	MenuItemManager::~MenuItemManager()
	{
		mDomainLoadedConn.disconnect();

		clearMenuItems();
	}

	void MenuItemManager::clearMenuItems()
	{
		MainEditorWindow* mainWindow = EditorWindowManager::instance().getMainWindow();

		for (auto& menuItem : mMenuItems)
			mainWindow->getMenuBar().removeMenuItem(menuItem);
	}

	void MenuItemManager::reloadAssemblyData()
	{
		clearMenuItems();

		// Reload MenuItem attribute from editor assembly
		MonoAssembly* editorAssembly = MonoManager::instance().getAssembly(EDITOR_ASSEMBLY);
		mMenuItemAttribute = editorAssembly->getClass("BansheeEditor", "MenuItem");
		if (mMenuItemAttribute == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find MenuItem managed class.");

		mPathField = mMenuItemAttribute->getField("path");
		mShortcutField = mMenuItemAttribute->getField("shortcut");
		mPriorityField = mMenuItemAttribute->getField("priority");

		MainEditorWindow* mainWindow = EditorWindowManager::instance().getMainWindow();

		Vector<String> scriptAssemblyNames = mScriptObjectManager.getScriptAssemblies();
		for (auto& assemblyName : scriptAssemblyNames)
		{
			MonoAssembly* assembly = MonoManager::instance().getAssembly(assemblyName);

			// Find new menu item methods
			const Vector<MonoClass*>& allClasses = assembly->getAllClasses();
			for (auto curClass : allClasses)
			{
				const Vector<MonoMethod*>& methods = curClass->getAllMethods();
				for (auto& curMethod : methods)
				{
					WString path;
					ShortcutKey shortcutKey = ShortcutKey::NONE;
					INT32 priority = 0;
					if (parseMenuItemMethod(curMethod, path, shortcutKey, priority))
					{
						std::function<void()> callback = std::bind(&MenuItemManager::menuItemCallback, curMethod);

						mainWindow->getMenuBar().addMenuItem(path, callback, priority, shortcutKey);
						mMenuItems.push_back(path);
					}
				}
			}
		}
	}

	bool MenuItemManager::parseMenuItemMethod(MonoMethod* method, WString& path, ShortcutKey& shortcut, INT32& priority) const
	{
		if (!method->hasAttribute(mMenuItemAttribute))
			return false;

		if (method->getNumParameters() != 0)
			return false;

		if (!method->isStatic())
			return false;

		MonoObject* menuItemAttrib = method->getAttribute(mMenuItemAttribute);

		MonoString* monoPath;
		mPathField->getValue(menuItemAttrib, &monoPath);

		mShortcutField->getValue(menuItemAttrib, &shortcut);
		path = MonoUtil::monoToWString(monoPath);
		mPriorityField->getValue(menuItemAttrib, &priority);

		return true;
	}

	void MenuItemManager::menuItemCallback(MonoMethod* method)
	{
		method->invoke(nullptr, nullptr);
	}
}
#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT MenuItemManager : public Module<MenuItemManager>
	{
	public:
		MenuItemManager(ScriptAssemblyManager& scriptObjectManager);
		~MenuItemManager();

	private:
		void clearMenuItems();
		void reloadAssemblyData();
		bool parseMenuItemMethod(MonoMethod* method, WString& path, ShortcutKey& shortcut, INT32& priority) const;

		static void menuItemCallback(MonoMethod* method);

		ScriptAssemblyManager& mScriptObjectManager;
		HEvent mDomainLoadedConn;

		MonoClass* mMenuItemAttribute;
		MonoField* mPathField;
		MonoField* mShortcutField;
		MonoField* mPriorityField;

		Vector<WString> mMenuItems;
	};
}
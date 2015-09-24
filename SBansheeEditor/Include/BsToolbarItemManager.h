#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Tracks toolbar items that are registered in managed code using the ToolbarItem
	 *			attribute.
	 */
	class BS_SCR_BED_EXPORT ToolbarItemManager : public Module<ToolbarItemManager>
	{
	public:
		ToolbarItemManager(ScriptAssemblyManager& scriptObjectManager);
		~ToolbarItemManager();

	private:
		/**
		 * @brief	Removes all managed toolbar items from the main menu.
		 */
		void clearToolbarItems();

		/**
		 * @brief	Reloads all assembly types and attempts to find uses of ToolbarItem. Old
		 *			toolbar items are cleared and new are added.
		 */
		void reloadAssemblyData();

		/**
		 * @brief	Parse the provided method and detect whether it has a ToolbarItem attribute.
		 *			If it has extract needed data from it.
		 *
		 * @param	method		Managed method to parse.
		 * @param	name		Output name defined in the ToolbarItem attribute.
		 * @param	icon		Texture defined in the ToolbarItem attribute.
		 * @param	tooltip		Optional tooltip defined in the ToolbarItem attribute.
		 * @param	priority	Menu item priority defined in the MenuItem attribute.
		 * @param	separator	Should the separator be inserted before the menu item.
		 *
		 * @return	True if the method has a ToolbarItem attribute. If false is returned output parameters
		 *			from this method are undefined.
		 */
		bool parseToolbarItemMethod(MonoMethod* method, String& name, HSpriteTexture& icon, HString& tooltip,
			INT32& priority, bool& separator) const;

		/**
		 * @brief	Triggered when one of the managed toolbar items is clicked. 
		 *
		 * @param	method	Managed method that the ToolbarItem is referencing.
		 */
		static void toolbarItemCallback(MonoMethod* method);

		ScriptAssemblyManager& mScriptObjectManager;
		HEvent mDomainLoadedConn;

		MonoClass* mToolbarItemAttribute;
		MonoField* mNameField;
		MonoField* mIconField;
		MonoField* mTooltipField;
		MonoField* mPriorityField;
		MonoField* mSeparatorField;

		Vector<String> mToolbarItems;
	};
}
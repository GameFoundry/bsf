//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"
#include "BsGUIDropDownBox.h"

namespace BansheeEngine
{
	/**
	 * Holds information about a single element in a GUI menu.
	 */
	class BS_EXPORT GUIMenuItem
	{
	public:
		/**
		 * @brief	Constructs a new non-separator menu item.
		 *
		 * @param	parent		Parent item, if any.
		 * @param	name		Name of the item to be displayed.
		 * @param	callback	Callback to be triggered when menu items is selected.
		 */
		GUIMenuItem(GUIMenuItem* parent, const WString& name, std::function<void()> callback);

		/**
		 * @brief	Constructs a new separator menu item.
		 *
		 * @param	parent		Parent item, if any.
		 */
		GUIMenuItem(GUIMenuItem* parent);
		~GUIMenuItem();

		/**
		 * @brief	Registers a new child with the item.
		 */
		void addChild(GUIMenuItem* child) { mChildren.push_back(child); }

		/**
		 * @brief	Returns number of child menu items.
		 */
		UINT32 getNumChildren() const { return (UINT32)mChildren.size(); }

		/**
		 * @brief	Returns the parent menu item, or null if none.
		 */
		const GUIMenuItem* getParent() const { return mParent; }

		/**
		 * @brief	Returns name of the menu item. Empty if separator.
		 */
		const WString& getName() const { return mName; }

		/**
		 * @brief	Returns callback that will trigger when menu item is selected. Null for separators.
		 */
		std::function<void()> getCallback() const { return mCallback; }

		/**
		 * @brief	Checks is the menu item a separator or a normal named menu item.
		 */
		bool isSeparator() const { return mIsSeparator; }

		/**
		 * @brief	Attempts to find a child menu item with the specified name.
		 *			Only direct descendants are searched.
		 */
		const GUIMenuItem* findChild(const WString& name) const;

		/**
		 * @brief	Removes the first child with the specified name.
		 */
		void removeChild(const WString& name);

	private:
		friend class GUIMenu;

		/**
		 * @copydoc	GUIMenuitem::findChild(const WString& name) const
		 */
		GUIMenuItem* findChild(const WString& name);

		GUIMenuItem* mParent;
		bool mIsSeparator;
		WString mName;
		std::function<void()> mCallback;
		Vector<GUIMenuItem*> mChildren;
	};

	/**
	 * Class that allows creation of menus with drop down functionality.
	 * Menu consists out of a number of top level elements, each of which opens
	 * a drop down menu which may internally hold a deeper hierarchy of menus.
	 *
	 * @note	When specifying menu items you must provide a path. Path must be formated in a certain way.
	 *			All path elements must be separated by /, e.g. "View/Toolbars/Find". "View" would be the top
	 *			level path element, "Toolbars" a child in its menu that opens up its own submenu, and "Find"
	 *			a child in the "Toolbars" sub-menu with an optional callback.
	 *
	 *			This is an abstract class and you should provide specialized implementations for specific menu types.
	 */
	class BS_EXPORT GUIMenu
	{
	public:
		GUIMenu();
		virtual ~GUIMenu();

		/**
		 * @brief	Adds a new menu item with the specified callback. Element items are added in order.
		 *			
		 * @param	path		Path that determines where to add the element. See class information on how to specify paths.
		 *						All sub-elements of a path will be added automatically.
		 * @param	callback	Callback that triggers when the path element is selected.
		 *
		 * @returns	A menu item object that you may use for removing the menu item later. Its lifetime is managed internally.
		 */
		const GUIMenuItem* addMenuItem(const WString& path, std::function<void()> callback);

		/**
		 * @brief	Adds a new separator menu item with the specified callback. Element items are added in order.
		 *			
		 * @param	path		Path that determines where to add the element. See class information on how to specify paths.
		 *						All sub-elements of a path will be added automatically.
		 *
		 * @returns	A menu item object that you may use for removing the menu item later. Its lifetime is managed internally.
		 */
		const GUIMenuItem* addSeparator(const WString& path);

		/**
		 * @brief	Returns a menu item at the specified path, or null if one is not found.
		 */
		const GUIMenuItem* getMenuItem(const WString& path) const;

		/**
		 * @brief	Removes the specified menu item from the path. If the menu item has any sub-menus they will also be removed.
		 */
		void removeMenuItem(const GUIMenuItem* item);

		/**
		 * @brief	Normally menu items use values from their paths as their names. However path labels don't provide a way of
		 * 			localizing the menu item. This method allows you to set specific names (different from path labels)
		 * 			to each menu item. All the values are localized so they will also be updated according to the string table.
		 *
		 * @param	menuItemLabel	The menu item label. (e.g. if you have a menu like "View/Toolbars/Find, this parameter would be either
		 * 							"View", "Toolbars" or "Find" depending which entry you want to localize)
		 * @param	localizedName	Localized string with the name.
		 */
		void setLocalizedName(const WString& menuItemLabel, const HString& localizedName);

		/**
		 * @brief	Returns data used for initializing a drop down list, for all elements.
		 */
		GUIDropDownData getDropDownData() const;
	protected:
		/**
		 * @brief	Adds a menu item at the specified path, as a normal button or as a separator.
		 */
		const GUIMenuItem* addMenuItemInternal(const WString& path, std::function<void()> callback, bool isSeparator);

		/**
		 * @brief	Return drop down data for the specified menu.
		 */
		GUIDropDownData getDropDownDataInternal(const GUIMenuItem& menu) const;

		GUIMenuItem mRootElement;
		UnorderedMap<WString, HString> mLocalizedEntryNames;
	};
}
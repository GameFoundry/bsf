#pragma once

#include "BsEditorPrerequisites.h"
#include "BsShortcutKey.h"

namespace BansheeEngine
{
	/**
	 * @brief	A menu bar GUI element that contains a horizontal list of elements
	 *			that can each be expanded into a hierarchical sub-menus. Contents
	 *			of the menu bar are customizable once created.
	 *
	 *			The menu bar also displays the minimize, maximize and close buttons for the window.
	 */
	class BS_ED_EXPORT GUIMenuBar
	{
		/**
		 * @brief	Contains data about the top level menu elements.
		 */
		struct GUIMenuBarData
		{
			WString name;
			GUIMenu* menu;
			GUIButton* button;
		};

	public:
		/**
		 * @brief	Constructs a new menu bar.
		 *
		 * @param	parent			Parent GUI widget the menu bar will be docked in.
		 * @param	parentWindow	Window to trigger the min/max/close events on.
		 */
		GUIMenuBar(GUIWidget* parent, RenderWindow* parentWindow);
		virtual ~GUIMenuBar();

		/**
		 * @brief	Sets the area of the menu bar, in pixels relative
		 *			to the parent GUI widget.
		 */
		void setArea(INT32 x, INT32 y, UINT32 width, UINT32 height);

		/**
		 * @brief	Adds a new menu item to the menu bar. 
		 *
		 * @param	path		Path to the menu item. Each element of the path must be separated using "/". 
		 *						First element of the path will create the top level menu, and any further element
		 *						will create a new sub-menu. Last element will be the interactable element.
		 * @param	callback	Callback to trigger when user click on the interactable element (last element in the provided path).
		 *						Can be null.
		 * @param	priority	Determines where is the element positioned compared to other elements in the same sub-menu.
		 *						Higher priority elements get placed higher up in the sub-menu. This only applies to the last
		 *						element. If you need to customize its parent element priority call this method with
		 *						with their specific paths.
		 * @param	shortcut	Keyboard shortcut key to display next to the interactable element, and register with the
		 *						global shortcut manager.
		 */
		const GUIMenuItem* addMenuItem(const WString& path, std::function<void()> callback, INT32 priority = 0, const ShortcutKey& shortcut = ShortcutKey::NONE);

		/**
		 * @brief	Adds a separator element at the specified path. The separator is added as a child of the path.
		 *
		 * @param	path		Parent path of the sub-menu to add the separator.
		 * @param	priority	Determines where is the separator positioned compared to other elements in the same sub-menu.
		 *						Higher priority elements get placed higher up in the sub-menu.
		 */
		const GUIMenuItem* addSeparator(const WString& path, INT32 priority = 0);

		/**
		 * @brief	Returns an existing menu item at the specified path, or null if one cannot be found.
		 */
		const GUIMenuItem* getMenuItem(const WString& path) const;

		/**
		 * @brief	Removes a menu item from the specified path. If this path points to a sub-menu entire sub-menu will be removed.
		 */
		void removeMenuItem(const WString& path);
	private:
		static const UINT32 NUM_ELEMENTS_AFTER_CONTENT;

		RenderWindow* mParentWindow;
		GUIWidget* mParentWidget;
		GUIPanel* mMainPanel;
		GUIPanel* mBgPanel;
		GUILayout* mMainLayout;
		GUITexture* mBgTexture;
		GUITexture* mLogoTexture;

		GUIButton* mMinBtn;
		GUIButton* mMaxBtn;
		GUIButton* mCloseBtn;

		Vector<GUIMenuBarData> mChildMenus;
		UnorderedMap<WString, ShortcutKey> mEntryShortcuts;

		GUIButton* mSubMenuButton;
		bool mSubMenuOpen;

		/**
		 * @brief	Finds a top level sub-menu with the specified name.
		 */
		const GUIMenuBarData* getSubMenu(const WString& name) const;

		/**
		 * @brief	Adds a new top level sub-menu button.
		 */
		GUIMenuBarData* addNewButton(const WString& name);

		/**
		 * @brief	Attempts to remove the first element from the specified path. First element
		 * 			returned in specified in "pathRoot", and original "path" parameter is modified so
		 * 			it no longer includes the first element.
		 *
		 * @return	False if first element doesn't exist, true otherwise.
		 */
		bool stripPath(WString& path, WString& pathRoot) const;

		/**
		 * @brief	Registers a shortcut with the global shortcut manager. Pressing the shortcut will trigger
		 *			the provided callback.
		 */
		void registerShortcut(const WString& path, const ShortcutKey& shortcut, std::function<void()> callback);

		/**
		 * @brief	Unregisters a shortcut assigned to the provided path from the global shortcut manager.
		 */
		void unregisterShortcut(const WString& path);

		/**
		 * @brief	Opens a top level sub-menu with the provided name.
		 */
		void openSubMenu(const WString& name);

		/**
		 * @brief	Closes any currently active sub-menu.
		 */
		void closeSubMenu();

		/**
		 * @brief	Triggered when a sub-menu is open and a user hovers over another
		 *			top level sub-menu button
		 *
		 * @param	name	Name of the sub-menu the user is hovering over.
		 */
		void onSubMenuHover(const WString& name);

		/**
		 * @brief	Triggered when a sub-menu is closed.
		 */
		void onSubMenuClosed();

		/**
		 * @brief	Triggered when the minimize button is clicked.
		 *			Minimizes the attached window.
		 */
		void onMinimizeClicked();

		/**
		 * @brief	Triggered when the maximize button is clicked.
		 *			Maximizes the attached window.
		 */
		void onMaximizeClicked();

		/**
		 * @brief	Triggered when the close button is clicked.
		 *			Closes the attached window.
		 */
		void onCloseClicked();

		/**
		 * @brief	Refreshes the OS client area that allow the window to be dragged
		 *			by dragging the empty areas on the menu bar. Should be called when top
		 *			level button configuration changes or menu bar area changes.
		 */
		void refreshNonClientAreas();
	};
}
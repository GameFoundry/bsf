//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMenu.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manages display and logic for a context menu.
	 *			Context menus can be opened anywhere within the GUI
	 *			and can contain a hierarchy of menu items. 
	 */
	class BS_EXPORT GUIContextMenu : public GUIMenu
	{
	public:
		GUIContextMenu();
		~GUIContextMenu();

		/**
		 * @brief	Opens a context menu at the specified position relative
		 *			to the provided widget.
		 */
		void open(const Vector2I& position, GUIWidget& widget);

	private:
		/**
		 * @brief	Closes the context menu if open.
		 */
		void close();

		/**
		 * @brief	Called when the context menu is closed externally
		 *			(e.g. when user selects an item or clicks outside it).
		 */
		void onMenuClosed();

	private:
		bool mContextMenuOpen;
	};
}
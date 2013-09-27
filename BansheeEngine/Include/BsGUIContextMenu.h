#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMenu.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIContextMenu : public GUIMenu
	{
	public:
		GUIContextMenu();
		~GUIContextMenu();

		void open(const CM::Int2& position, GUIWidget& widget);

	private:
		bool mContextMenuOpen;

		void close();
		void onMenuClosed();
	};
}
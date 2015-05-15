#include "BsGUIContextMenu.h"
#include "BsGUIDropDownBoxManager.h"
#include "BsGUIManager.h"

namespace BansheeEngine
{
	GUIContextMenu::GUIContextMenu()
		:GUIMenu(), mContextMenuOpen(false)
	{

	}

	GUIContextMenu::~GUIContextMenu()
	{
		close();
	}

	void GUIContextMenu::open(const Vector2I& position, GUIWidget& widget)
	{
		DropDownAreaPlacement placement = DropDownAreaPlacement::aroundPosition(position);

		GameObjectHandle<GUIDropDownMenu> dropDownBox = GUIDropDownBoxManager::instance().openDropDownBox(widget.getTarget(), 
			placement, getDropDownData(), widget.getSkinResource(), GUIDropDownType::ContextMenu, std::bind(&GUIContextMenu::onMenuClosed, this));

		mContextMenuOpen = true;
	}

	void GUIContextMenu::close()
	{
		if(mContextMenuOpen)
		{
			GUIDropDownBoxManager::instance().closeDropDownBox();
			mContextMenuOpen = false;
		}
	}

	void GUIContextMenu::onMenuClosed()
	{
		mContextMenuOpen = false;
	}
}
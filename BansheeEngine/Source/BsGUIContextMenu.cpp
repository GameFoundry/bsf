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
		DROP_DOWN_BOX_DESC desc;
		desc.camera = widget.getCamera();
		desc.skin = widget.getSkinResource();
		desc.placement = DropDownAreaPlacement::aroundPosition(position);
		desc.dropDownData = getDropDownData();

		GameObjectHandle<GUIDropDownMenu> dropDownBox = GUIDropDownBoxManager::instance().openDropDownBox(
			desc, GUIDropDownType::ContextMenu, std::bind(&GUIContextMenu::onMenuClosed, this));

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
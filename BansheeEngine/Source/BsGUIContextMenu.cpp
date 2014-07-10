//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
		GUIDropDownAreaPlacement placement = GUIDropDownAreaPlacement::aroundPosition(position);

		GameObjectHandle<GUIDropDownBox> dropDownBox = GUIDropDownBoxManager::instance().openDropDownBox(widget.getTarget(), 
			placement, getDropDownData(), widget.getSkin(), GUIDropDownType::ContextMenu, std::bind(&GUIContextMenu::onMenuClosed, this));

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
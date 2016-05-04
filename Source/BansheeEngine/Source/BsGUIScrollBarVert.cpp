//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsGUIScrollBarVert.h"
#include "BsGUIDimensions.h"

namespace BansheeEngine
{
	GUIScrollBarVert::GUIScrollBarVert(const String& styleName, const GUIDimensions& dimensions)
		:GUIScrollBar(false, styleName, dimensions)
	{

	}

	GUIScrollBarVert::~GUIScrollBarVert()
	{

	}

	GUIScrollBarVert* GUIScrollBarVert::create(const String& styleName)
	{
		return new (bs_alloc<GUIScrollBarVert>()) GUIScrollBarVert(getStyleName<GUIScrollBarVert>(styleName), GUIDimensions::create());
	}

	GUIScrollBarVert* GUIScrollBarVert::create(const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUIScrollBarVert>()) GUIScrollBarVert(getStyleName<GUIScrollBarVert>(styleName), GUIDimensions::create(options));
	}

	const String& GUIScrollBarVert::getGUITypeName()
	{
		static String typeName = "ScrollBarVert";
		return typeName;
	}
}
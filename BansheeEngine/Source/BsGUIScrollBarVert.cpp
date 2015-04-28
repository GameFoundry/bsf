#include "BsGUIScrollBarVert.h"
#include "BsGUIElementStyle.h"
#include "BsGUISkin.h"
#include "BsGUIWidget.h"
#include "BsGUIDimensions.h"
#include "BsGUILayout.h"
#include "BsGUISkin.h"
#include "BsGUIButton.h"
#include "BsGUISliderHandle.h"
#include "BsGUISpace.h"
#include "BsException.h"

namespace BansheeEngine
{
	GUIScrollBarVert::GUIScrollBarVert(const String& styleName, const GUIDimensions& layoutOptions)
		:GUIScrollBar(false, styleName, layoutOptions)
	{

	}

	GUIScrollBarVert::~GUIScrollBarVert()
	{

	}

	GUIScrollBarVert* GUIScrollBarVert::create(const String& styleName)
	{
		return new (bs_alloc<GUIScrollBarVert, PoolAlloc>()) GUIScrollBarVert(getStyleName<GUIScrollBarVert>(styleName), GUIDimensions::create());
	}

	GUIScrollBarVert* GUIScrollBarVert::create(const GUIOptions& layoutOptions, const String& styleName)
	{
		return new (bs_alloc<GUIScrollBarVert, PoolAlloc>()) GUIScrollBarVert(getStyleName<GUIScrollBarVert>(styleName), GUIDimensions::create(layoutOptions));
	}

	const String& GUIScrollBarVert::getGUITypeName()
	{
		static String typeName = "ScrollBarVert";
		return typeName;
	}
}
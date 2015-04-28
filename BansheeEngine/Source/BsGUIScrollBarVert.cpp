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
	GUIScrollBarVert::GUIScrollBarVert(const String& styleName, const GUIDimensions& dimensions)
		:GUIScrollBar(false, styleName, dimensions)
	{

	}

	GUIScrollBarVert::~GUIScrollBarVert()
	{

	}

	GUIScrollBarVert* GUIScrollBarVert::create(const String& styleName)
	{
		return new (bs_alloc<GUIScrollBarVert, PoolAlloc>()) GUIScrollBarVert(getStyleName<GUIScrollBarVert>(styleName), GUIDimensions::create());
	}

	GUIScrollBarVert* GUIScrollBarVert::create(const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUIScrollBarVert, PoolAlloc>()) GUIScrollBarVert(getStyleName<GUIScrollBarVert>(styleName), GUIDimensions::create(options));
	}

	const String& GUIScrollBarVert::getGUITypeName()
	{
		static String typeName = "ScrollBarVert";
		return typeName;
	}
}
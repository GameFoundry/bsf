#include "BsGUIScrollBarVert.h"
#include "BsGUIElementStyle.h"
#include "BsGUISkin.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutOptions.h"
#include "BsGUILayout.h"
#include "BsGUISkin.h"
#include "BsGUIButton.h"
#include "BsGUIScrollBarHandle.h"
#include "BsGUISpace.h"
#include "CmException.h"

namespace BansheeEngine
{
	GUIScrollBarVert::GUIScrollBarVert(const String& styleName, const GUILayoutOptions& layoutOptions)
		:GUIScrollBar(false, styleName, layoutOptions)
	{

	}

	GUIScrollBarVert::~GUIScrollBarVert()
	{

	}

	GUIScrollBarVert* GUIScrollBarVert::create(const String& styleName)
	{
		return new (cm_alloc<GUIScrollBarVert, PoolAlloc>()) GUIScrollBarVert(getStyleName<GUIScrollBarVert>(styleName), GUILayoutOptions::create());
	}

	GUIScrollBarVert* GUIScrollBarVert::create(const GUIOptions& layoutOptions, const String& styleName)
	{
		return new (cm_alloc<GUIScrollBarVert, PoolAlloc>()) GUIScrollBarVert(getStyleName<GUIScrollBarVert>(styleName), GUILayoutOptions::create(layoutOptions));
	}

	const String& GUIScrollBarVert::getGUITypeName()
	{
		static String typeName = "ScrollBarVert";
		return typeName;
	}
}
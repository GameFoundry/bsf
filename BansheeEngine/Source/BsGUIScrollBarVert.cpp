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

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIScrollBarVert::GUIScrollBarVert(const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIScrollBar(false, style, layoutOptions)
	{

	}

	GUIScrollBarVert::~GUIScrollBarVert()
	{

	}

	GUIScrollBarVert* GUIScrollBarVert::create(const GUIElementStyle* style)
	{
		return new (cm_alloc<GUIScrollBarVert, PoolAlloc>()) GUIScrollBarVert(style, GUILayoutOptions::create(style));
	}

	GUIScrollBarVert* GUIScrollBarVert::create(const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUIScrollBarVert, PoolAlloc>()) GUIScrollBarVert(style, GUILayoutOptions::create(layoutOptions, style));
	}

	const String& GUIScrollBarVert::getGUITypeName()
	{
		static String typeName = "ScrollBarVert";
		return typeName;
	}
}
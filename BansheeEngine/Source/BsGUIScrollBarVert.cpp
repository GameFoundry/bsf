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
	GUIScrollBarVert::GUIScrollBarVert(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIScrollBar(parent, false, style, layoutOptions)
	{

	}

	GUIScrollBarVert::~GUIScrollBarVert()
	{

	}

	GUIScrollBarVert* GUIScrollBarVert::create(GUIWidget& parent, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollBarVert, PoolAlloc>()) GUIScrollBarVert(parent, style, GUILayoutOptions::create(style));
	}

	GUIScrollBarVert* GUIScrollBarVert::create(GUIWidget& parent, const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollBarVert, PoolAlloc>()) GUIScrollBarVert(parent, style, GUILayoutOptions::create(layoutOptions, style));
	}

	const String& GUIScrollBarVert::getGUITypeName()
	{
		static String typeName = "ScrollBarVert";
		return typeName;
	}
}
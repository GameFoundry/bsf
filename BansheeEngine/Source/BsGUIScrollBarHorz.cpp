#include "BsGUIScrollBarHorz.h"
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
	GUIScrollBarHorz::GUIScrollBarHorz(const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIScrollBar(true, style, layoutOptions)
	{

	}

	GUIScrollBarHorz::~GUIScrollBarHorz()
	{

	}

	GUIScrollBarHorz* GUIScrollBarHorz::create(const GUIElementStyle* style)
	{
		return new (cm_alloc<GUIScrollBarHorz, PoolAlloc>()) GUIScrollBarHorz(style, GUILayoutOptions::create(style));
	}

	GUIScrollBarHorz* GUIScrollBarHorz::create(const GUIOptions& layoutOptions, const GUIElementStyle* style)
	{
		return new (cm_alloc<GUIScrollBarHorz, PoolAlloc>()) GUIScrollBarHorz(style, GUILayoutOptions::create(layoutOptions, style));
	}

	const String& GUIScrollBarHorz::getGUITypeName()
	{
		static String typeName = "ScrollBarHorz";
		return typeName;
	}
}
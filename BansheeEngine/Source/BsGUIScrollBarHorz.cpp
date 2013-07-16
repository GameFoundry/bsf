#include "BsGUIScrollBarHorz.h"
#include "BsGUIElementStyle.h"
#include "BsGUISkin.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutOptions.h"
#include "BsGUILayout.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIScrollBarHorz::GUIScrollBarHorz(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mLayout(nullptr)
	{
		// TODO - Init mLayout
	}

	GUIScrollBarHorz::~GUIScrollBarHorz()
	{

	}

	UINT32 GUIScrollBarHorz::getNumRenderElements() const
	{
		return 0;
	}

	const HMaterial& GUIScrollBarHorz::getMaterial(UINT32 renderElementIdx) const
	{
		CM_EXCEPT(InternalErrorException, "Invalid render element index. This class has no render elements.");
	}

	UINT32 GUIScrollBarHorz::getNumQuads(UINT32 renderElementIdx) const
	{
		return 0;
	}

	void GUIScrollBarHorz::updateRenderElementsInternal()
	{ }

	UINT32 GUIScrollBarHorz::_getOptimalWidth() const
	{
		return mLayout->_getOptimalWidth();
	}

	UINT32 GUIScrollBarHorz::_getOptimalHeight() const
	{
		return mLayout->_getOptimalHeight();
	}

	void GUIScrollBarHorz::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{ }

	GUIScrollBarHorz* GUIScrollBarHorz::create(GUIWidget& parent, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollBarHorz, PoolAlloc>()) GUIScrollBarHorz(parent, style, getDefaultLayoutOptions(style));
	}

	GUIScrollBarHorz* GUIScrollBarHorz::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollBarHorz, PoolAlloc>()) GUIScrollBarHorz(parent, style, layoutOptions);
	}

	const String& GUIScrollBarHorz::getGUITypeName()
	{
		static String typeName = "ScrollBarHorz";
		return typeName;
	}
}
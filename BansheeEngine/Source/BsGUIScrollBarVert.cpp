#include "BsGUIScrollBarVert.h"
#include "BsGUIElementStyle.h"
#include "BsGUISkin.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutOptions.h"
#include "BsGUILayout.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIScrollBarVert::GUIScrollBarVert(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mLayout(nullptr)
	{
		// TODO - Init mLayout
	}

	GUIScrollBarVert::~GUIScrollBarVert()
	{

	}

	UINT32 GUIScrollBarVert::getNumRenderElements() const
	{
		return 0;
	}

	const HMaterial& GUIScrollBarVert::getMaterial(UINT32 renderElementIdx) const
	{
		CM_EXCEPT(InternalErrorException, "Invalid render element index. This class has no render elements.");
	}

	UINT32 GUIScrollBarVert::getNumQuads(UINT32 renderElementIdx) const
	{
		return 0;
	}

	void GUIScrollBarVert::updateRenderElementsInternal()
	{ }

	UINT32 GUIScrollBarVert::_getOptimalWidth() const
	{
		return mLayout->_getOptimalWidth();
	}

	UINT32 GUIScrollBarVert::_getOptimalHeight() const
	{
		return mLayout->_getOptimalHeight();
	}

	void GUIScrollBarVert::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{ }

	GUIScrollBarVert* GUIScrollBarVert::create(GUIWidget& parent, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollBarVert, PoolAlloc>()) GUIScrollBarVert(parent, style, getDefaultLayoutOptions(style));
	}

	GUIScrollBarVert* GUIScrollBarVert::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollBarVert, PoolAlloc>()) GUIScrollBarVert(parent, style, layoutOptions);
	}

	const String& GUIScrollBarVert::getGUITypeName()
	{
		static String typeName = "ScrollBarVert";
		return typeName;
	}
}
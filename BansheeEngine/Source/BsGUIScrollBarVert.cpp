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
		:GUIElement(parent, style, layoutOptions), mLayout(nullptr)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();

		mLayout = &addLayoutYInternal();

		GUIButton* upBtn = GUIButton::create(parent, L"", parent.getSkin()->getStyle("ScrollUpBtn"));
		GUIButton* downBtn = GUIButton::create(parent, L"", parent.getSkin()->getStyle("ScrollDownBtn"));
		GUIScrollBarHandle* handleBtn = GUIScrollBarHandle::create(parent, false, GUILayoutOptions::expandableY(6), parent.getSkin()->getStyle("ScrollBarVertBtn"));

		mLayout->addSpace(2);
		mLayout->addElement(upBtn);
		mLayout->addSpace(2);
		mLayout->addElement(handleBtn); // We might want a special element for this?
		mLayout->addSpace(2);
		mLayout->addElement(downBtn);
		mLayout->addSpace(2);
	}

	GUIScrollBarVert::~GUIScrollBarVert()
	{
		cm_delete<PoolAlloc>(mImageSprite);
	}

	UINT32 GUIScrollBarVert::getNumRenderElements() const
	{
		return mImageSprite->getNumRenderElements();
	}

	const HMaterial& GUIScrollBarVert::getMaterial(UINT32 renderElementIdx) const
	{
		return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUIScrollBarVert::getNumQuads(UINT32 renderElementIdx) const
	{
		return mImageSprite->getNumQuads(renderElementIdx);
	}

	void GUIScrollBarVert::updateRenderElementsInternal()
	{
		IMAGE_SPRITE_DESC desc;
		desc.texture = mStyle->normal.texture;
		desc.width = mWidth;
		desc.height = mHeight;

		mImageSprite->update(desc);
	}

	UINT32 GUIScrollBarVert::_getOptimalWidth() const
	{
		return mLayout->_getOptimalWidth();
	}

	UINT32 GUIScrollBarVert::_getOptimalHeight() const
	{
		return mLayout->_getOptimalHeight();
	}

	UINT32 GUIScrollBarVert::_getRenderElementDepth(UINT32 renderElementIdx) const
	{
		return _getDepth() + 2; // + 2 depth because child buttons use +1
	}

	void GUIScrollBarVert::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx, mOffset, mClipRect);
	}

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
#include "BsGUIWindowFrame.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUILayoutOptions.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIWindowFrame::getGUITypeName()
	{
		static String name = "WindowFrame";
		return name;
	}

	GUIWindowFrame::GUIWindowFrame(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions)
	{
		mImageSprite = CM_NEW(ImageSprite, PoolAlloc) ImageSprite();

		mDesc.texture = mStyle->normal.texture;

		if(mDesc.texture != nullptr)
		{
			mDesc.width = mDesc.texture->getTexture()->getWidth();
			mDesc.height = mDesc.texture->getTexture()->getHeight();
		}

		mDesc.borderLeft = mStyle->border.left;
		mDesc.borderRight = mStyle->border.right;
		mDesc.borderTop = mStyle->border.top;
		mDesc.borderBottom = mStyle->border.bottom;
	}

	GUIWindowFrame::~GUIWindowFrame()
	{
		CM_DELETE(mImageSprite, ImageSprite, PoolAlloc);
	}

	GUIWindowFrame* GUIWindowFrame::create(GUIWidget& parent, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getGUISkin();
			style = skin->getStyle(getGUITypeName());
		}

		return CM_NEW(GUIWindowFrame, PoolAlloc) GUIWindowFrame(parent, style, getDefaultLayoutOptions(style));
	}

	GUIWindowFrame* GUIWindowFrame::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getGUISkin();
			style = skin->getStyle(getGUITypeName());
		}

		return CM_NEW(GUIWindowFrame, PoolAlloc) GUIWindowFrame(parent, style, layoutOptions);
	}

	UINT32 GUIWindowFrame::getNumRenderElements() const
	{
		return mImageSprite->getNumRenderElements();
	}

	const HMaterial& GUIWindowFrame::getMaterial(UINT32 renderElementIdx) const
	{
		return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUIWindowFrame::getNumQuads(UINT32 renderElementIdx) const
	{
		return mImageSprite->getNumQuads(renderElementIdx);
	}

	void GUIWindowFrame::updateRenderElementsInternal()
	{		
		mDesc.offset = mOffset;
		mDesc.width = mWidth;
		mDesc.height = mHeight;
		mDesc.clipRect = mClipRect;

		mImageSprite->update(mDesc);
		mBounds = mImageSprite->getBounds();
	}

	UINT32 GUIWindowFrame::_getOptimalWidth() const
	{
		if(mDesc.texture != nullptr)
		{
			return mDesc.texture->getTexture()->getWidth();
		}

		return 0;
	}

	UINT32 GUIWindowFrame::_getOptimalHeight() const
	{
		if(mDesc.texture != nullptr)
		{
			return mDesc.texture->getTexture()->getHeight();
		}

		return 0;
	}

	void GUIWindowFrame::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx);
	}
}
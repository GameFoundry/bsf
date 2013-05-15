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

	GUIWindowFrame::GUIWindowFrame(GUIWidget& parent, const GUI_LAYOUT_OPTIONS& layoutOptions)
		:GUIElement(parent, layoutOptions)
	{
		const GUISkin* skin = parent.getGUISkin();

		mStyle = skin->getStyle(getGUITypeName());
		mImageSprite = CM_NEW(ImageSprite, PoolAlloc) ImageSprite();

		IMAGE_SPRITE_DESC desc;
		desc.texture = mStyle->normal.texture;

		if(desc.texture != nullptr)
		{
			desc.width = desc.texture->getTexture()->getWidth();
			desc.height = desc.texture->getTexture()->getHeight();
		}

		desc.borderLeft = mStyle->border.left;
		desc.borderRight = mStyle->border.right;
		desc.borderTop = mStyle->border.top;
		desc.borderBottom = mStyle->border.bottom;

		mImageSprite->update(desc);

		mBounds = mImageSprite->getBounds();
	}

	GUIWindowFrame::~GUIWindowFrame()
	{
		CM_DELETE(mImageSprite, ImageSprite, PoolAlloc);
	}

	GUIWindowFrame* GUIWindowFrame::create(GUIWidget& parent)
	{
		return CM_NEW(GUIWindowFrame, PoolAlloc) GUIWindowFrame(parent, getDefaultLayoutOptions());
	}

	GUIWindowFrame* GUIWindowFrame::create(GUIWidget& parent, const GUI_LAYOUT_OPTIONS& layoutOptions)
	{
		return CM_NEW(GUIWindowFrame, PoolAlloc) GUIWindowFrame(parent, layoutOptions);
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

	void GUIWindowFrame::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx);
	}

	const GUI_LAYOUT_OPTIONS& GUIWindowFrame::getDefaultLayoutOptions()
	{
		static GUI_LAYOUT_OPTIONS layoutOptions;
		static bool layoutOptionsInitialized = false;

		if(!layoutOptionsInitialized)
		{
			layoutOptions.fixedWidth = false;
			layoutOptions.fixedHeight = false;

			layoutOptionsInitialized = true;
		}

		return layoutOptions;
	}
}
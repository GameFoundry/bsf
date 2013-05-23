#include "BsGUIButton.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIButton::getGUITypeName()
	{
		static String name = "Button";
		return name;
	}

	GUIButton::GUIButton(GUIWidget& parent, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, layoutOptions)
	{
		const GUISkin* skin = parent.getGUISkin();

		mStyle = skin->getStyle(getGUITypeName());
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

	GUIButton::~GUIButton()
	{
		CM_DELETE(mImageSprite, ImageSprite, PoolAlloc);
	}

	GUIButton* GUIButton::create(GUIWidget& parent)
	{
		const GUISkin* skin = parent.getGUISkin();
		const GUIElementStyle* style = skin->getStyle(getGUITypeName());

		return CM_NEW(GUIButton, PoolAlloc) GUIButton(parent, getDefaultLayoutOptions(style));
	}

	GUIButton* GUIButton::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions)
	{
		return CM_NEW(GUIButton, PoolAlloc) GUIButton(parent, layoutOptions);
	}

	UINT32 GUIButton::getNumRenderElements() const
	{
		return mImageSprite->getNumRenderElements();
	}

	const HMaterial& GUIButton::getMaterial(UINT32 renderElementIdx) const
	{
		return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUIButton::getNumQuads(UINT32 renderElementIdx) const
	{
		return mImageSprite->getNumQuads(renderElementIdx);
	}

	void GUIButton::updateRenderElementsInternal()
	{		
		mDesc.offset = mOffset;
		mDesc.width = mWidth;
		mDesc.height = mHeight;
		mDesc.clipRect = mClipRect;

		mImageSprite->update(mDesc);
		mBounds = mImageSprite->getBounds();
	}

	UINT32 GUIButton::_getOptimalWidth() const
	{
		if(mDesc.texture != nullptr)
		{
			return mDesc.texture->getTexture()->getWidth();
		}

		return 0;
	}

	UINT32 GUIButton::_getOptimalHeight() const
	{
		if(mDesc.texture != nullptr)
		{
			return mDesc.texture->getTexture()->getHeight();
		}

		return 0;
	}

	void GUIButton::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx);
	}

	bool GUIButton::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseOver)
		{
			mDesc.texture = mStyle->normal.texture;
			markAsDirty();
			// TODO - What happens when a texture is not set?
			// 
			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseOut)
		{
			mDesc.texture = mStyle->hover.texture;
			markAsDirty();
			// TODO - What happens when a texture is not set?

			return true;
		}
		
		return false;
	}
}
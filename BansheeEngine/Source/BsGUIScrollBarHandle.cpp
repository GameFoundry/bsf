#include "BsGUIScrollBarHandle.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIScrollBarHandle::getGUITypeName()
	{
		static String name = "ScrollBarHandle";
		return name;
	}

	GUIScrollBarHandle::GUIScrollBarHandle(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mHorizontal(false), mHandleSize(2)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();
		mCurTexture = style->normal.texture;
	}

	GUIScrollBarHandle::~GUIScrollBarHandle()
	{
		cm_delete<PoolAlloc>(mImageSprite);
	}

	GUIScrollBarHandle* GUIScrollBarHandle::create(GUIWidget& parent, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollBarHandle, PoolAlloc>()) GUIScrollBarHandle(parent, style, getDefaultLayoutOptions(style));
	}

	GUIScrollBarHandle* GUIScrollBarHandle::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollBarHandle, PoolAlloc>()) GUIScrollBarHandle(parent, style, layoutOptions);
	}

	UINT32 GUIScrollBarHandle::getNumRenderElements() const
	{
		return mImageSprite->getNumRenderElements();
	}

	const HMaterial& GUIScrollBarHandle::getMaterial(UINT32 renderElementIdx) const
	{
		return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUIScrollBarHandle::getNumQuads(UINT32 renderElementIdx) const
	{
		return mImageSprite->getNumQuads(renderElementIdx);
	}

	void GUIScrollBarHandle::updateRenderElementsInternal()
	{		
		IMAGE_SPRITE_DESC desc;
		desc.texture = mCurTexture;

		if(mHorizontal)
		{
			desc.width = mHandleSize;
			desc.height = mHeight;
		}
		else
		{
			desc.width = mWidth;
			desc.height = mHandleSize;
		}

		mImageSprite->update(desc);
		mBounds = Rect(mOffset.x, mOffset.y, mWidth, mHeight);
		mBounds.clip(mClipRect);
	}

	UINT32 GUIScrollBarHandle::_getOptimalWidth() const
	{
		if(mCurTexture != nullptr)
		{
			return mCurTexture->getTexture()->getWidth();
		}

		return 0;
	}

	UINT32 GUIScrollBarHandle::_getOptimalHeight() const
	{
		if(mCurTexture != nullptr)
		{
			return mCurTexture->getTexture()->getHeight();
		}

		return 0;
	}

	void GUIScrollBarHandle::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
			vertexStride, indexStride, renderElementIdx, mOffset, mClipRect);
	}

	bool GUIScrollBarHandle::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseOver)
		{
			mCurTexture = mStyle->hover.texture;
			markContentAsDirty();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseOut)
		{
			mCurTexture = mStyle->normal.texture;
			markContentAsDirty();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDown)
		{
			mCurTexture = mStyle->active.texture;
			markContentAsDirty();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseUp)
		{
			mCurTexture = mStyle->hover.texture;
			markContentAsDirty();

			return true;
		}
		
		return false;
	}
}
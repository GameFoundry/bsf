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

	GUIScrollBarHandle::GUIScrollBarHandle(GUIWidget& parent, bool horizontal, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mHorizontal(horizontal), mHandleSize(2), mMouseOverHandle(false), mHandlePos(0), mDragStartPos(0),
		mHandleDragged(false)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();
		mCurTexture = style->normal.texture;
	}

	GUIScrollBarHandle::~GUIScrollBarHandle()
	{
		cm_delete<PoolAlloc>(mImageSprite);
	}

	GUIScrollBarHandle* GUIScrollBarHandle::create(GUIWidget& parent, bool horizontal, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollBarHandle, PoolAlloc>()) GUIScrollBarHandle(parent, horizontal, style, getDefaultLayoutOptions(style));
	}

	GUIScrollBarHandle* GUIScrollBarHandle::create(GUIWidget& parent, bool horizontal, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollBarHandle, PoolAlloc>()) GUIScrollBarHandle(parent, horizontal, style, layoutOptions);
	}

	void GUIScrollBarHandle::setHandleSize(CM::UINT32 size)
	{
		mHandleSize = std::min(getMaxSize(), size);
		markContentAsDirty();
	}

	void GUIScrollBarHandle::setHandlePos(float pct)
	{
		UINT32 maxScrollAmount = getMaxSize() - mHandleSize;
		mHandlePos = Math::FloorToInt(pct * maxScrollAmount);

		markContentAsDirty();
	}

	float GUIScrollBarHandle::getHandlePos() const
	{
		UINT32 maxScrollAmount = getMaxSize() - mHandleSize;
		return (float)mHandlePos / maxScrollAmount;
	}

	UINT32 GUIScrollBarHandle::getScrollableSize() const
	{
		return getMaxSize() - mHandleSize;
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

		Rect localClipRect(mClipRect.x + mOffset.x, mClipRect.y + mOffset.y, mClipRect.width, mClipRect.height);
		mBounds.clip(localClipRect);
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
		Int2 offset = mOffset;
		if(mHorizontal)
			offset.x += mHandlePos;
		else
			offset.y += mHandlePos;

		Rect clipRect = mClipRect;
		if(mHorizontal)
			clipRect.x -= mHandlePos;
		else
			clipRect.y -= mHandlePos;

		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
			vertexStride, indexStride, renderElementIdx, offset, clipRect);
	}

	bool GUIScrollBarHandle::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseMove)
		{
			if(mMouseOverHandle)
			{
				if(!isOnHandle(ev.getPosition()))
				{
					mMouseOverHandle = false;

					mCurTexture = mStyle->normal.texture;
					markContentAsDirty();

					return true;
				}
			}
			else
			{
				if(isOnHandle(ev.getPosition()))
				{
					mMouseOverHandle = true;

					mCurTexture = mStyle->hover.texture;
					markContentAsDirty();

					return true;
				}
			}
		}

		if(ev.getType() == GUIMouseEventType::MouseDown && mMouseOverHandle)
		{
			mCurTexture = mStyle->active.texture;
			markContentAsDirty();

			if(mHorizontal)
			{
				INT32 left = (INT32)mOffset.x + mHandlePos;
				mDragStartPos = ev.getPosition().x - left;
			}
			else
			{
				INT32 top = (INT32)mOffset.y + mHandlePos;
				mDragStartPos = ev.getPosition().y - top;
			}

			mHandleDragged = true;
			return true;
		}

		if(ev.getType() == GUIMouseEventType::MouseDrag && mHandleDragged)
		{
			if(mHorizontal)
			{
				mHandlePos = ev.getPosition().x - mDragStartPos - mOffset.x;
			}
			else
			{
				mHandlePos = ev.getPosition().y - mDragStartPos - mOffset.y;
			}

			UINT32 maxScrollAmount = getMaxSize() - mHandleSize;
			mHandlePos = Math::Clamp(mHandlePos, 0, (INT32)maxScrollAmount);

			if(!onHandleMoved.empty())
			{
				float pct = (float)mHandlePos / maxScrollAmount;
				onHandleMoved(pct);
			}

			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUIMouseEventType::MouseOut && !mHandleDragged)
		{
			mCurTexture = mStyle->normal.texture;
			markContentAsDirty();

			return true;
		}

		if(ev.getType() == GUIMouseEventType::MouseUp)
		{
			if(mMouseOverHandle)
				mCurTexture = mStyle->hover.texture;
			else
				mCurTexture = mStyle->normal.texture;

			// If we clicked above or below the scroll handle, scroll by one page
			INT32 handleOffset = 0;
			if(mHorizontal)
			{
				INT32 handleLeft = (INT32)mOffset.x + mHandlePos;
				INT32 handleRight = handleLeft + mHandleSize;

				if(ev.getPosition().x < handleLeft)
					handleOffset -= mHandleSize;
				else if(ev.getPosition().x > handleRight)
					handleOffset += mHandleSize;
			}
			else
			{
				INT32 handleTop = (INT32)mOffset.y + mHandlePos;
				INT32 handleBottom = handleTop + mHandleSize;

				if(ev.getPosition().y < handleTop)
					handleOffset -= mHandleSize;
				else if(ev.getPosition().y > handleBottom)
					handleOffset += mHandleSize;
			}

			mHandlePos += handleOffset;
			UINT32 maxScrollAmount = getMaxSize() - mHandleSize;
			mHandlePos = Math::Clamp(mHandlePos, 0, (INT32)maxScrollAmount);

			if(!onHandleMoved.empty())
			{
				float pct = (float)mHandlePos / maxScrollAmount;
				onHandleMoved(pct);
			}

			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUIMouseEventType::MouseDragEnd)
		{
			mHandleDragged = false;

			if(mMouseOverHandle)
				mCurTexture = mStyle->hover.texture;
			else
				mCurTexture = mStyle->normal.texture;

			markContentAsDirty();
			return true;
		}
		
		return false;
	}

	bool GUIScrollBarHandle::isOnHandle(const CM::Int2& pos) const
	{
		if(mHorizontal)
		{
			INT32 left = (INT32)mOffset.x + mHandlePos;
			INT32 right = left + mHandleSize;

			if(pos.x >= left && pos.x < right)
				return true;
		}
		else
		{
			INT32 top = (INT32)mOffset.y + mHandlePos;
			INT32 bottom = top + mHandleSize;

			if(pos.y >= top && pos.y < bottom)
				return true;
		}
		
		return false;
	}

	UINT32 GUIScrollBarHandle::getMaxSize() const
	{
		UINT32 maxSize = mHeight;
		if(mHorizontal)
			maxSize = mWidth;

		return maxSize;
	}
}
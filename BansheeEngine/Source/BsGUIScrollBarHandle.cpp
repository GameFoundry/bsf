#include "BsGUIScrollBarHandle.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "CmDebug.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIScrollBarHandle::getGUITypeName()
	{
		static String name = "ScrollBarHandle";
		return name;
	}

	GUIScrollBarHandle::GUIScrollBarHandle(bool horizontal, const CM::String& styleName, const GUILayoutOptions& layoutOptions)
		:GUIElement(styleName, layoutOptions), mHorizontal(horizontal), mHandleSize(2), mMouseOverHandle(false), mHandlePos(0), mDragStartPos(0),
		mHandleDragged(false), mState(State::Normal)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();
	}

	GUIScrollBarHandle::~GUIScrollBarHandle()
	{
		cm_delete<PoolAlloc>(mImageSprite);
	}

	GUIScrollBarHandle* GUIScrollBarHandle::create(bool horizontal, const CM::String& styleName)
	{
		return new (cm_alloc<GUIScrollBarHandle, PoolAlloc>()) GUIScrollBarHandle(horizontal, getStyleName<GUIScrollBarHandle>(styleName), GUILayoutOptions::create());
	}

	GUIScrollBarHandle* GUIScrollBarHandle::create(bool horizontal, const GUIOptions& layoutOptions, const CM::String& styleName)
	{
		return new (cm_alloc<GUIScrollBarHandle, PoolAlloc>()) GUIScrollBarHandle(horizontal, getStyleName<GUIScrollBarHandle>(styleName), GUILayoutOptions::create(layoutOptions));
	}

	void GUIScrollBarHandle::setHandleSize(CM::UINT32 size)
	{
		mHandleSize = std::min(getMaxSize(), size);
		markContentAsDirty();
	}

	void GUIScrollBarHandle::setHandlePos(float pct)
	{
		pct = Math::clamp01(pct);

		UINT32 maxScrollAmount = getMaxSize() - mHandleSize;
		mHandlePos = pct * maxScrollAmount;

		markContentAsDirty();
	}

	float GUIScrollBarHandle::getHandlePos() const
	{
		UINT32 maxScrollAmount = getMaxSize() - mHandleSize;

		if(maxScrollAmount > 0.0f)
			return mHandlePos / maxScrollAmount;
		else
			return 0.0f;
	}

	UINT32 GUIScrollBarHandle::getScrollableSize() const
	{
		return getMaxSize() - mHandleSize;
	}

	UINT32 GUIScrollBarHandle::getNumRenderElements() const
	{
		return mImageSprite->getNumRenderElements();
	}

	const GUIMaterialInfo& GUIScrollBarHandle::getMaterial(UINT32 renderElementIdx) const
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

		HSpriteTexture activeTex = getActiveTexture();
		if(SpriteTexture::checkIsLoaded(activeTex))
			desc.texture = activeTex.getInternalPtr();

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
		
		GUIElement::updateRenderElementsInternal();
	}

	void GUIScrollBarHandle::updateClippedBounds()
	{
		mClippedBounds = RectI(mOffset.x, mOffset.y, mWidth, mHeight);

		RectI localClipRect(mClipRect.x + mOffset.x, mClipRect.y + mOffset.y, mClipRect.width, mClipRect.height);
		mClippedBounds.clip(localClipRect);
	}

	Vector2I GUIScrollBarHandle::_getOptimalSize() const
	{
		HSpriteTexture activeTex = getActiveTexture();

		if(SpriteTexture::checkIsLoaded(activeTex))
			return Vector2I(activeTex->getTexture()->getWidth(), activeTex->getTexture()->getHeight());

		return Vector2I();
	}

	void GUIScrollBarHandle::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		Vector2I offset = mOffset;
		if(mHorizontal)
			offset.x += Math::floorToInt(mHandlePos);
		else
			offset.y += Math::floorToInt(mHandlePos);

		RectI clipRect = mClipRect;
		if(mHorizontal)
			clipRect.x -= Math::floorToInt(mHandlePos);
		else
			clipRect.y -= Math::floorToInt(mHandlePos);

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

					mState = State::Normal;
					markContentAsDirty();

					return true;
				}
			}
			else
			{
				if(isOnHandle(ev.getPosition()))
				{
					mMouseOverHandle = true;

					mState = State::Hover;
					markContentAsDirty();

					return true;
				}
			}
		}

		if(ev.getType() == GUIMouseEventType::MouseDown && mMouseOverHandle)
		{
			mState = State::Active;
			markContentAsDirty();

			if(mHorizontal)
			{
				INT32 left = (INT32)mOffset.x + Math::floorToInt(mHandlePos);
				mDragStartPos = ev.getPosition().x - left;
			}
			else
			{
				INT32 top = (INT32)mOffset.y + Math::floorToInt(mHandlePos);
				mDragStartPos = ev.getPosition().y - top;
			}

			mHandleDragged = true;
			return true;
		}

		if(ev.getType() == GUIMouseEventType::MouseDrag && mHandleDragged)
		{
			if(mHorizontal)
			{
				mHandlePos = (float)(ev.getPosition().x - mDragStartPos - mOffset.x);
			}
			else
			{
				mHandlePos = float(ev.getPosition().y - mDragStartPos - mOffset.y);
			}

			float maxScrollAmount = (float)getMaxSize() - mHandleSize;
			mHandlePos = Math::clamp(mHandlePos, 0.0f, maxScrollAmount);

			if(!onHandleMoved.empty())
			{
				float pct = 0.0f;
				if(maxScrollAmount > 0.0f)
					pct = mHandlePos / maxScrollAmount;
				
				onHandleMoved(pct);
			}

			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUIMouseEventType::MouseOut && !mHandleDragged)
		{
			mState = State::Normal;
			mMouseOverHandle = false;
			markContentAsDirty();

			return true;
		}

		if(ev.getType() == GUIMouseEventType::MouseUp)
		{
			if(mMouseOverHandle)
				mState = State::Hover;
			else
				mState = State::Normal;

			// If we clicked above or below the scroll handle, scroll by one page
			INT32 handleOffset = 0;
			if(mHorizontal)
			{
				INT32 handleLeft = (INT32)mOffset.x + Math::floorToInt(mHandlePos);
				INT32 handleRight = handleLeft + mHandleSize;

				if(ev.getPosition().x < handleLeft)
					handleOffset -= mHandleSize;
				else if(ev.getPosition().x > handleRight)
					handleOffset += mHandleSize;
			}
			else
			{
				INT32 handleTop = (INT32)mOffset.y + Math::floorToInt(mHandlePos);
				INT32 handleBottom = handleTop + mHandleSize;

				if(ev.getPosition().y < handleTop)
					handleOffset -= mHandleSize;
				else if(ev.getPosition().y > handleBottom)
					handleOffset += mHandleSize;
			}

			mHandlePos += handleOffset;
			float maxScrollAmount = (float)getMaxSize() - mHandleSize;
			mHandlePos = Math::clamp(mHandlePos, 0.0f, maxScrollAmount);

			if(!onHandleMoved.empty())
			{
				float pct = 0.0f;
				
				if(maxScrollAmount > 0.0f)
					pct = (float)mHandlePos / maxScrollAmount;

				onHandleMoved(pct);
			}

			markContentAsDirty();
			return true;
		}

		if(ev.getType() == GUIMouseEventType::MouseDragEnd)
		{
			mHandleDragged = false;

			if(mMouseOverHandle)
				mState = State::Hover;
			else
				mState = State::Normal;

			markContentAsDirty();
			return true;
		}
		
		return false;
	}

	bool GUIScrollBarHandle::isOnHandle(const CM::Vector2I& pos) const
	{
		if(mHorizontal)
		{
			INT32 left = (INT32)mOffset.x + Math::floorToInt(mHandlePos);
			INT32 right = left + mHandleSize;

			if(pos.x >= left && pos.x < right)
				return true;
		}
		else
		{
			INT32 top = (INT32)mOffset.y + Math::floorToInt(mHandlePos);
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

	const HSpriteTexture& GUIScrollBarHandle::getActiveTexture() const
	{
		switch(mState)
		{
		case State::Active:
			return _getStyle()->active.texture;
		case State::Hover:
			return _getStyle()->hover.texture;
		case State::Normal:
			return _getStyle()->normal.texture;
		}

		return _getStyle()->normal.texture;
	}
}
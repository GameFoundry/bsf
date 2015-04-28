#include "BsGUISliderHandle.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUIDimensions.h"
#include "BsGUIMouseEvent.h"
#include "BsDebug.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	const String& GUISliderHandle::getGUITypeName()
	{
		static String name = "SliderHandle";
		return name;
	}

	GUISliderHandle::GUISliderHandle(bool horizontal, bool jumpOnClick, const String& styleName, const GUIDimensions& layoutOptions)
		:GUIElement(styleName, layoutOptions), mHorizontal(horizontal), mHandleSize(0), mMouseOverHandle(false), mPctHandlePos(0.0f), mDragStartPos(0),
		mHandleDragged(false), mState(State::Normal), mJumpOnClick(jumpOnClick)
	{
		mImageSprite = bs_new<ImageSprite, PoolAlloc>();
	}

	GUISliderHandle::~GUISliderHandle()
	{
		bs_delete<PoolAlloc>(mImageSprite);
	}

	GUISliderHandle* GUISliderHandle::create(bool horizontal, bool jumpOnClick, const String& styleName)
	{
		return new (bs_alloc<GUISliderHandle, PoolAlloc>()) GUISliderHandle(horizontal, jumpOnClick, 
			getStyleName<GUISliderHandle>(styleName), GUIDimensions::create());
	}

	GUISliderHandle* GUISliderHandle::create(bool horizontal, bool jumpOnClick, const GUIOptions& layoutOptions, const String& styleName)
	{
		return new (bs_alloc<GUISliderHandle, PoolAlloc>()) GUISliderHandle(horizontal, jumpOnClick, 
			getStyleName<GUISliderHandle>(styleName), GUIDimensions::create(layoutOptions));
	}

	void GUISliderHandle::setHandleSize(UINT32 size)
	{
		mHandleSize = std::min(getMaxSize(), size);
		markContentAsDirty();
	}

	void GUISliderHandle::setHandlePos(float pct)
	{
		mPctHandlePos = Math::clamp01(pct);

		markContentAsDirty();
	}

	float GUISliderHandle::getHandlePos() const
	{
		return mPctHandlePos;;
	}

	UINT32 GUISliderHandle::getScrollableSize() const
	{
		return getMaxSize() - mHandleSize;
	}

	void GUISliderHandle::setTint(const Color& color)
	{
		mColor = color;

		markContentAsDirty();
	}

	UINT32 GUISliderHandle::_getNumRenderElements() const
	{
		return mImageSprite->getNumRenderElements();
	}

	const GUIMaterialInfo& GUISliderHandle::_getMaterial(UINT32 renderElementIdx) const
	{
		return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUISliderHandle::_getNumQuads(UINT32 renderElementIdx) const
	{
		return mImageSprite->getNumQuads(renderElementIdx);
	}

	void GUISliderHandle::updateRenderElementsInternal()
	{		
		IMAGE_SPRITE_DESC desc;

		HSpriteTexture activeTex = getActiveTexture();
		if(SpriteTexture::checkIsLoaded(activeTex))
			desc.texture = activeTex.getInternalPtr();

		if (mHorizontal)
		{
			if (mHandleSize == 0 && desc.texture != nullptr)
				mHandleSize = desc.texture->getWidth();

			desc.width = mHandleSize;
			desc.height = mHeight;
		}
		else
		{
			if (mHandleSize == 0 && desc.texture != nullptr)
				mHandleSize = desc.texture->getHeight();

			desc.width = mWidth;
			desc.height = mHandleSize;
		}

		desc.color = mColor;
		mImageSprite->update(desc, (UINT64)_getParentWidget());
		
		GUIElement::updateRenderElementsInternal();
	}

	void GUISliderHandle::updateClippedBounds()
	{
		mClippedBounds = Rect2I(mOffset.x, mOffset.y, mWidth, mHeight);

		Rect2I localClipRect(mClipRect.x + mOffset.x, mClipRect.y + mOffset.y, mClipRect.width, mClipRect.height);
		mClippedBounds.clip(localClipRect);
	}

	Vector2I GUISliderHandle::_getOptimalSize() const
	{
		HSpriteTexture activeTex = getActiveTexture();

		if(SpriteTexture::checkIsLoaded(activeTex))
			return Vector2I(activeTex->getWidth(), activeTex->getHeight());

		return Vector2I();
	}

	void GUISliderHandle::_fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		Vector2I offset = mOffset;
		if(mHorizontal)
			offset.x += getHandlePosPx();
		else
			offset.y += getHandlePosPx();

		Rect2I clipRect = mClipRect;
		if(mHorizontal)
			clipRect.x -= getHandlePosPx();
		else
			clipRect.y -= getHandlePosPx();

		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
			vertexStride, indexStride, renderElementIdx, offset, clipRect);
	}

	bool GUISliderHandle::_mouseEvent(const GUIMouseEvent& ev)
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

		if(ev.getType() == GUIMouseEventType::MouseDown && (mMouseOverHandle || mJumpOnClick))
		{
			mState = State::Active;
			markContentAsDirty();

			if (mJumpOnClick)
			{
				float handlePosPx = 0.0f;

				if (mHorizontal)
					handlePosPx = (float)(ev.getPosition().x - (INT32)mOffset.x - mHandleSize * 0.5f);
				else
					handlePosPx = (float)(ev.getPosition().y - (INT32)mOffset.y - mHandleSize * 0.5f);

				float maxScrollAmount = (float)getMaxSize() - mHandleSize;
				mPctHandlePos = Math::clamp01(handlePosPx / maxScrollAmount);
			}

			if(mHorizontal)
			{
				INT32 left = (INT32)mOffset.x + getHandlePosPx();
				mDragStartPos = ev.getPosition().x - left;
			}
			else
			{
				INT32 top = (INT32)mOffset.y + getHandlePosPx();
				mDragStartPos = ev.getPosition().y - top;
			}

			mHandleDragged = true;
			return true;
		}

		if(ev.getType() == GUIMouseEventType::MouseDrag && mHandleDragged)
		{
			float handlePosPx = 0.0f;
			if(mHorizontal)
			{
				handlePosPx = (float)(ev.getPosition().x - mDragStartPos - mOffset.x);
			}
			else
			{
				handlePosPx = (float)(ev.getPosition().y - mDragStartPos - mOffset.y);
			}

			float maxScrollAmount = (float)getMaxSize() - mHandleSize;
			mPctHandlePos = Math::clamp01(handlePosPx / maxScrollAmount);

			onHandleMoved(mPctHandlePos);

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
			INT32 handlePosPx = getHandlePosPx();
			if (!mJumpOnClick)
			{
				INT32 handleOffset = 0;
				if (mHorizontal)
				{
					INT32 handleLeft = (INT32)mOffset.x + handlePosPx;
					INT32 handleRight = handleLeft + mHandleSize;

					if (ev.getPosition().x < handleLeft)
						handleOffset -= mHandleSize;
					else if (ev.getPosition().x > handleRight)
						handleOffset += mHandleSize;
				}
				else
				{
					INT32 handleTop = (INT32)mOffset.y + handlePosPx;
					INT32 handleBottom = handleTop + mHandleSize;

					if (ev.getPosition().y < handleTop)
						handleOffset -= mHandleSize;
					else if (ev.getPosition().y > handleBottom)
						handleOffset += mHandleSize;
				}

				handlePosPx += handleOffset;
			}

			float maxScrollAmount = (float)getMaxSize() - mHandleSize;
			mPctHandlePos = Math::clamp01(handlePosPx / maxScrollAmount);

			onHandleMoved(mPctHandlePos);

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

	bool GUISliderHandle::isOnHandle(const Vector2I& pos) const
	{
		if(mHorizontal)
		{
			INT32 left = (INT32)mOffset.x + getHandlePosPx();
			INT32 right = left + mHandleSize;

			if(pos.x >= left && pos.x < right)
				return true;
		}
		else
		{
			INT32 top = (INT32)mOffset.y + getHandlePosPx();
			INT32 bottom = top + mHandleSize;

			if(pos.y >= top && pos.y < bottom)
				return true;
		}
		
		return false;
	}

	INT32 GUISliderHandle::getHandlePosPx() const
	{
		UINT32 maxScrollAmount = getMaxSize() - mHandleSize;
		return Math::floorToInt(mPctHandlePos * maxScrollAmount);
	}

	UINT32 GUISliderHandle::getMaxSize() const
	{
		UINT32 maxSize = mHeight;
		if(mHorizontal)
			maxSize = mWidth;

		return maxSize;
	}

	const HSpriteTexture& GUISliderHandle::getActiveTexture() const
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
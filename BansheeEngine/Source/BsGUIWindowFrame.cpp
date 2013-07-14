#include "BsGUIWindowFrame.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "CmApplication.h"
#include "CmCursor.h"
#include "CmTexture.h"
#include "CmRenderWindow.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	enum class FrameSubArea
	{
		TopLeft, TopCenter, TopRight,
		MiddleLeft, Middle, MiddleRight,
		BottomLeft, BottomCenter, BottomRight,
		None
	};

	FrameSubArea getFrameSubArea(const Int2& position, const Rect& bounds)
	{
		INT32 x0 = bounds.x;
		INT32 x1 = bounds.x + 3;
		INT32 x2 = bounds.x + bounds.width - 3;
		INT32 x3 = bounds.x + bounds.width;

		INT32 y0 = bounds.y;
		INT32 y1 = bounds.y + 3;
		INT32 y2 = bounds.y + bounds.height - 3;
		INT32 y3 = bounds.y + bounds.height;

		if(position.x >= x0 && position.x < x1 &&
			position.y >= y0 && position.y < y1)
		{
			return FrameSubArea::TopLeft;
		}

		if(position.x >= x1 && position.x < x2 &&
			position.y >= y0 && position.y < y1)
		{
			return FrameSubArea::TopCenter;
		}

		if(position.x >= x2 && position.x < x3 &&
			position.y >= y0 && position.y < y1)
		{
			return FrameSubArea::TopRight;
		}

		if(position.x >= x0 && position.x < x1 &&
			position.y >= y1 && position.y < y2)
		{
			return FrameSubArea::MiddleLeft;
		}

		if(position.x >= x1 && position.x < x2 &&
			position.y >= y1 && position.y < y2)
		{
			return FrameSubArea::Middle;
		}

		if(position.x >= x2 && position.x < x3 &&
			position.y >= y1 && position.y < y2)
		{
			return FrameSubArea::MiddleRight;
		}

		if(position.x >= x0 && position.x < x1 &&
			position.y >= y2 && position.y < y3)
		{
			return FrameSubArea::BottomLeft;
		}

		if(position.x >= x1 && position.x < x2 &&
			position.y >= y2 && position.y < y3)
		{
			return FrameSubArea::BottomCenter;
		}

		if(position.x >= x2 && position.x < x3 &&
			position.y >= y2 && position.y < y3)
		{
			return FrameSubArea::BottomRight;
		}

		return FrameSubArea::None;
	}

	const String& GUIWindowFrame::getGUITypeName()
	{
		static String name = "WindowFrame";
		return name;
	}

	GUIWindowFrame::GUIWindowFrame(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mResizeCursorSet(false)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();

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
		cm_delete<PoolAlloc>(mImageSprite);
	}

	GUIWindowFrame* GUIWindowFrame::create(GUIWidget& parent, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIWindowFrame, PoolAlloc>()) GUIWindowFrame(parent, style, getDefaultLayoutOptions(style));
	}

	GUIWindowFrame* GUIWindowFrame::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIWindowFrame, PoolAlloc>()) GUIWindowFrame(parent, style, layoutOptions);
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
		mDesc.width = mWidth;
		mDesc.height = mHeight;

		mImageSprite->update(mDesc);
		mBounds = mImageSprite->getBounds(mOffset, mClipRect);
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

	bool GUIWindowFrame::_isInBounds(const CM::Int2 position) const
	{
		Rect contentBounds = getVisibleBounds();

		if(!contentBounds.contains(position))
			return false;

		FrameSubArea subArea = getFrameSubArea(position, contentBounds);
		if(subArea != FrameSubArea::None && subArea != FrameSubArea::Middle)
			return true;

		return false;
	}

	void GUIWindowFrame::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx, mOffset, mClipRect);
	}

	void GUIWindowFrame::setFocused(bool focused)
	{
		if(focused)
			mDesc.texture = mStyle->focused.texture;
		else
			mDesc.texture = mStyle->normal.texture;

		markContentAsDirty();
	}

	bool GUIWindowFrame::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseMove || ev.getType() == GUIMouseEventType::MouseDrag)
		{
			Rect contentBounds = getVisibleBounds();

			FrameSubArea subArea = getFrameSubArea(ev.getPosition(), contentBounds);
			if(subArea != FrameSubArea::None && subArea != FrameSubArea::Middle)
			{
				switch (subArea)
				{
				case BansheeEngine::FrameSubArea::TopLeft:
					Cursor::setCursor(CursorType::SizeNWSE);
					break;
				case BansheeEngine::FrameSubArea::TopCenter:
					Cursor::setCursor(CursorType::SizeNS);
					break;
				case BansheeEngine::FrameSubArea::TopRight:
					Cursor::setCursor(CursorType::SizeNESW);
					break;
				case BansheeEngine::FrameSubArea::MiddleLeft:
					Cursor::setCursor(CursorType::SizeWE);
					break;
				case BansheeEngine::FrameSubArea::MiddleRight:
					Cursor::setCursor(CursorType::SizeWE);
					break;
				case BansheeEngine::FrameSubArea::BottomLeft:
					Cursor::setCursor(CursorType::SizeNESW);
					break;
				case BansheeEngine::FrameSubArea::BottomCenter:
					Cursor::setCursor(CursorType::SizeNS);
					break;
				case BansheeEngine::FrameSubArea::BottomRight:
					Cursor::setCursor(CursorType::SizeNWSE);
					break;
				}

				mResizeCursorSet = true;

				return true;
			}
		}

		if(ev.getType() == GUIMouseEventType::MouseDown)
		{
			Rect contentBounds = getVisibleBounds();

			FrameSubArea subArea = getFrameSubArea(ev.getPosition(), contentBounds);
			if(subArea != FrameSubArea::None && subArea != FrameSubArea::Middle)
			{
				RenderWindow* window = _getParentWidget().getOwnerWindow();
				RenderWindowPtr windowPtr = std::static_pointer_cast<RenderWindow>(window->getThisPtr());

				switch (subArea)
				{
				case BansheeEngine::FrameSubArea::TopLeft:
					gMainCA().startResize(windowPtr, WindowResizeDirection::TopLeft);
					break;
				case BansheeEngine::FrameSubArea::TopCenter:
					gMainCA().startResize(windowPtr, WindowResizeDirection::Top);
					break;
				case BansheeEngine::FrameSubArea::TopRight:
					gMainCA().startResize(windowPtr, WindowResizeDirection::TopRight);
					break;
				case BansheeEngine::FrameSubArea::MiddleLeft:
					gMainCA().startResize(windowPtr, WindowResizeDirection::Left);
					break;
				case BansheeEngine::FrameSubArea::MiddleRight:
					gMainCA().startResize(windowPtr, WindowResizeDirection::Right);
					break;
				case BansheeEngine::FrameSubArea::BottomLeft:
					gMainCA().startResize(windowPtr, WindowResizeDirection::BottomLeft);
					break;
				case BansheeEngine::FrameSubArea::BottomCenter:
					gMainCA().startResize(windowPtr, WindowResizeDirection::Bottom);
					break;
				case BansheeEngine::FrameSubArea::BottomRight:
					gMainCA().startResize(windowPtr, WindowResizeDirection::BottomRight);
					break;
				}

				return true;
			}
		}

		if(ev.getType() == GUIMouseEventType::MouseOut || ev.getType() == GUIMouseEventType::MouseDragEnd)
		{
			if(mResizeCursorSet)
			{
				Cursor::setCursor(CursorType::Arrow);
				mResizeCursorSet = false;

				return true;
			}
		}

		return false;
	}
}
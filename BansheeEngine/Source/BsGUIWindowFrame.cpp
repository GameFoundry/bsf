#include "BsGUIWindowFrame.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "CmCursor.h"
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

	bool GUIWindowFrame::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseMove || ev.getType() == GUIMouseEventType::MouseDrag)
		{
			Int2 offset = _getOffset();
			Int2 cursorPos = ev.getPosition();

			INT32 x0 = offset.x;
			INT32 x1 = offset.x + 3;
			INT32 x2 = offset.x + _getWidth() - 3;
			INT32 x3 = offset.x + _getWidth();

			INT32 y0 = offset.y;
			INT32 y1 = offset.y + 3;
			INT32 y2 = offset.y + _getHeight() - 3;
			INT32 y3 = offset.y + _getHeight();

			bool overResizeArea = false;
			// Top left
			if(cursorPos.x >= x0 && cursorPos.x < x1 &&
				cursorPos.y >= y0 && cursorPos.y < y1)
			{
				Cursor::setCursor(CursorType::SizeNWSE);
				overResizeArea = true;
			}

			// Top center
			if(cursorPos.x >= x1 && cursorPos.x < x2 &&
				cursorPos.y >= y0 && cursorPos.y < y1)
			{
				Cursor::setCursor(CursorType::SizeNS);
				overResizeArea = true;
			}

			// Top right
			if(cursorPos.x >= x2 && cursorPos.x < x3 &&
				cursorPos.y >= y0 && cursorPos.y < y1)
			{
				Cursor::setCursor(CursorType::SizeNESW);
				overResizeArea = true;
			}

			// Middle left
			if(cursorPos.x >= x0 && cursorPos.x < x1 &&
				cursorPos.y >= y1 && cursorPos.y < y2)
			{
				Cursor::setCursor(CursorType::SizeWE);
				overResizeArea = true;
			}

			// Middle center
			if(cursorPos.x >= x1 && cursorPos.x < x2 &&
				cursorPos.y >= y1 && cursorPos.y < y2)
			{
				Cursor::setCursor(CursorType::Arrow);
				overResizeArea = false;
			}

			// Middle right
			if(cursorPos.x >= x2 && cursorPos.x < x3 &&
				cursorPos.y >= y1 && cursorPos.y < y2)
			{
				Cursor::setCursor(CursorType::SizeWE);
				overResizeArea = true;
			}

			// Bottom left
			if(cursorPos.x >= x0 && cursorPos.x < x1 &&
				cursorPos.y >= y2 && cursorPos.y < y3)
			{
				Cursor::setCursor(CursorType::SizeNESW);
				overResizeArea = true;
			}

			// Bottom center
			if(cursorPos.x >= x1 && cursorPos.x < x2 &&
				cursorPos.y >= y2 && cursorPos.y < y3)
			{
				Cursor::setCursor(CursorType::SizeNS);
				overResizeArea = true;
			}

			// Bottom right
			if(cursorPos.x >= x2 && cursorPos.x < x3 &&
				cursorPos.y >= y2 && cursorPos.y < y3)
			{
				Cursor::setCursor(CursorType::SizeNWSE);
				overResizeArea = true;
			}

			if(overResizeArea)
			{
				mResizeCursorSet = true;

				return true;
			}
		}

		if(ev.getType() == GUIMouseEventType::MouseOut)
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
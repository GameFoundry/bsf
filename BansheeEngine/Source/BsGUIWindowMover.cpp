#include "BsGUIWindowMover.h"
#include "CmApplication.h"
#include "CmTexture.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIWindowMover::getGUITypeName()
	{
		static String name = "WindowMover";
		return name;
	}

	GUIWindowMover::GUIWindowMover(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions)
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

	GUIWindowMover::~GUIWindowMover()
	{
		cm_delete<PoolAlloc>(mImageSprite);
	}

	GUIWindowMover* GUIWindowMover::create(GUIWidget& parent, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIWindowMover, PoolAlloc>()) GUIWindowMover(parent, style, getDefaultLayoutOptions(style));
	}

	GUIWindowMover* GUIWindowMover::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIWindowMover, PoolAlloc>()) GUIWindowMover(parent, style, layoutOptions);
	}

	UINT32 GUIWindowMover::getNumRenderElements() const
	{
		return mImageSprite->getNumRenderElements();
	}

	const HMaterial& GUIWindowMover::getMaterial(UINT32 renderElementIdx) const
	{
		return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUIWindowMover::getNumQuads(UINT32 renderElementIdx) const
	{
		return mImageSprite->getNumQuads(renderElementIdx);
	}

	void GUIWindowMover::updateRenderElementsInternal()
	{		
		mDesc.offset = mOffset;
		mDesc.width = mWidth;
		mDesc.height = mHeight;
		mDesc.clipRect = mClipRect;

		mImageSprite->update(mDesc);
		mBounds = mImageSprite->getBounds();
	}

	UINT32 GUIWindowMover::_getOptimalWidth() const
	{
		if(mDesc.texture != nullptr)
		{
			return mDesc.texture->getTexture()->getWidth();
		}

		return 0;
	}

	UINT32 GUIWindowMover::_getOptimalHeight() const
	{
		if(mDesc.texture != nullptr)
		{
			return mDesc.texture->getTexture()->getHeight();
		}

		return 0;
	}

	void GUIWindowMover::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx);
	}

	void GUIWindowMover::setFocused(bool focused)
	{
		if(focused)
			mDesc.texture = mStyle->focused.texture;
		else
			mDesc.texture = mStyle->normal.texture;

		markAsDirty();
	}

	bool GUIWindowMover::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseDown)
		{
			RenderWindow* window = _getParentWidget().getOwnerWindow();
			RenderWindowPtr windowPtr = std::static_pointer_cast<RenderWindow>(window->getThisPtr());

			gMainCA().startMove(windowPtr);

			return true;
		}

		return false;
	}
}
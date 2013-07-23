#include "BsGUIScrollBar.h"
#include "BsGUIElementStyle.h"
#include "BsGUISkin.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutOptions.h"
#include "BsGUILayout.h"
#include "BsGUISkin.h"
#include "BsGUIButton.h"
#include "BsGUIScrollBarHandle.h"
#include "BsGUISpace.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIScrollBar::GUIScrollBar(GUIWidget& parent, bool horizontal, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mHorizontal(horizontal)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();

		if(mHorizontal)
		{
			mLayout = &addLayoutXInternal();

			mUpBtn = GUIButton::create(parent, L"", parent.getSkin()->getStyle("ScrollLeftBtn"));
			mDownBtn = GUIButton::create(parent, L"", parent.getSkin()->getStyle("ScrollRightBtn"));

			mHandleBtn = GUIScrollBarHandle::create(parent, mHorizontal, GUILayoutOptions::expandableY(6), parent.getSkin()->getStyle("ScrollBarHorzBtn"));
		}
		else
		{
			mLayout = &addLayoutYInternal();

			mUpBtn = GUIButton::create(parent, L"", parent.getSkin()->getStyle("ScrollUpBtn"));
			mDownBtn = GUIButton::create(parent, L"", parent.getSkin()->getStyle("ScrollDownBtn"));

			mHandleBtn = GUIScrollBarHandle::create(parent, mHorizontal, GUILayoutOptions::expandableY(6), parent.getSkin()->getStyle("ScrollBarVertBtn"));
		}

		mLayout->addSpace(2);
		mLayout->addElement(mUpBtn);
		mLayout->addSpace(2);
		mLayout->addElement(mHandleBtn);
		mLayout->addSpace(2);
		mLayout->addElement(mDownBtn);
		mLayout->addSpace(2);

		mHandleBtn->handleMoved.connect(boost::bind(&GUIScrollBar::handleMoved, this, _1));
	}

	GUIScrollBar::~GUIScrollBar()
	{
		cm_delete<PoolAlloc>(mImageSprite);

		GUIElement::destroy(mUpBtn);
		GUIElement::destroy(mDownBtn);
		GUIElement::destroy(mHandleBtn);
	}

	UINT32 GUIScrollBar::getNumRenderElements() const
	{
		return mImageSprite->getNumRenderElements();
	}

	const HMaterial& GUIScrollBar::getMaterial(UINT32 renderElementIdx) const
	{
		return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUIScrollBar::getNumQuads(UINT32 renderElementIdx) const
	{
		return mImageSprite->getNumQuads(renderElementIdx);
	}

	void GUIScrollBar::updateRenderElementsInternal()
	{
		IMAGE_SPRITE_DESC desc;
		desc.texture = mStyle->normal.texture;
		desc.width = mWidth;
		desc.height = mHeight;

		mImageSprite->update(desc);
	}

	UINT32 GUIScrollBar::_getOptimalWidth() const
	{
		return mLayout->_getOptimalWidth();
	}

	UINT32 GUIScrollBar::_getOptimalHeight() const
	{
		return mLayout->_getOptimalHeight();
	}

	UINT32 GUIScrollBar::_getRenderElementDepth(UINT32 renderElementIdx) const
	{
		return _getDepth() + 2; // + 2 depth because child buttons use +1
	}

	void GUIScrollBar::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx, mOffset, mClipRect);
	}

	void GUIScrollBar::handleMoved(float handlePct)
	{
		if(!scrollPositionChanged.empty())
			scrollPositionChanged(handlePct);
	}

	void GUIScrollBar::setHandleSize(UINT32 size)
	{
		mHandleBtn->setHandleSize(size);
	}

	void GUIScrollBar::setScrollPos(float pct)
	{
		mHandleBtn->setHandlePos(pct);
	}

	CM::UINT32 GUIScrollBar::getMaxHandleSize() const
	{
		return mHandleBtn->getMaxSize();
	}
}
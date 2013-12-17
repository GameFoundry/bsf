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
	const UINT32 GUIScrollBar::ButtonScrollAmount = 10;

	GUIScrollBar::GUIScrollBar(GUIWidget& parent, bool horizontal, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mHorizontal(horizontal)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();

		if(mHorizontal)
		{
			mLayout = &addLayoutXInternal(this);

			mUpBtn = GUIButton::create(parent, HString(L""), parent.getSkin().getStyle("ScrollLeftBtn"));
			mDownBtn = GUIButton::create(parent, HString(L""), parent.getSkin().getStyle("ScrollRightBtn"));

			mHandleBtn = GUIScrollBarHandle::create(parent, mHorizontal, 
				GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleWidth(6)), parent.getSkin().getStyle("ScrollBarHorzBtn"));
		}
		else
		{
			mLayout = &addLayoutYInternal(this);

			mUpBtn = GUIButton::create(parent, HString(L""), parent.getSkin().getStyle("ScrollUpBtn"));
			mDownBtn = GUIButton::create(parent, HString(L""), parent.getSkin().getStyle("ScrollDownBtn"));

			mHandleBtn = GUIScrollBarHandle::create(parent, mHorizontal, 
				GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleWidth(6)), parent.getSkin().getStyle("ScrollBarVertBtn"));
		}

		mLayout->addSpace(2);
		mLayout->addElement(mUpBtn);
		mLayout->addSpace(2);
		mLayout->addElement(mHandleBtn);
		mLayout->addSpace(2);
		mLayout->addElement(mDownBtn);
		mLayout->addSpace(2);

		mHandleBtn->onHandleMoved.connect(boost::bind(&GUIScrollBar::handleMoved, this, _1));

		mUpBtn->onClick.connect(boost::bind(&GUIScrollBar::upButtonClicked, this));
		mDownBtn->onClick.connect(boost::bind(&GUIScrollBar::downButtonClicked, this));

		mHandleBtn->_setAcceptsKeyboardFocus(false);
		mUpBtn->_setAcceptsKeyboardFocus(false);
		mDownBtn->_setAcceptsKeyboardFocus(false);
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

	const GUIMaterialInfo& GUIScrollBar::getMaterial(UINT32 renderElementIdx) const
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

		GUIElement::updateRenderElementsInternal();
	}

	void GUIScrollBar::updateClippedBounds()
	{
		mClippedBounds = RectI(0, 0, 0, 0); // We don't want any mouse input for this element. This is just a container.
	}

	Vector2I GUIScrollBar::_getOptimalSize() const
	{
		return mLayout->_getOptimalSize();
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
		if(!onScrollPositionChanged.empty())
			onScrollPositionChanged(handlePct);
	}

	void GUIScrollBar::upButtonClicked()
	{
		float handleOffset = 0.0f;
		float scrollableSize = (float)mHandleBtn->getScrollableSize();
		
		if(scrollableSize > 0.0f)
			handleOffset = ButtonScrollAmount / scrollableSize;

		scroll(handleOffset);
	}

	void GUIScrollBar::downButtonClicked()
	{
		float handleOffset = 0.0f;
		float scrollableSize = (float)mHandleBtn->getScrollableSize();

		if(scrollableSize > 0.0f)
			handleOffset = ButtonScrollAmount / scrollableSize;

		scroll(-handleOffset);
	}

	void GUIScrollBar::scroll(float amount)
	{
		float newHandlePos = Math::clamp01(mHandleBtn->getHandlePos() - amount);

		mHandleBtn->setHandlePos(newHandlePos);

		if(!onScrollPositionChanged.empty())
			onScrollPositionChanged(newHandlePos);
	}

	void GUIScrollBar::setHandleSize(UINT32 size)
	{
		mHandleBtn->setHandleSize(size);
	}

	void GUIScrollBar::setScrollPos(float pct)
	{
		mHandleBtn->setHandlePos(pct);
	}

	UINT32 GUIScrollBar::getMaxHandleSize() const
	{
		return mHandleBtn->getMaxSize();
	}

	UINT32 GUIScrollBar::getScrollableSize() const
	{
		return mHandleBtn->getScrollableSize();
	}
}
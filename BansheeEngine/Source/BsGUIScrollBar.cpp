#include "BsGUIScrollBar.h"
#include "BsSpriteTexture.h"
#include "BsGUIElementStyle.h"
#include "BsGUIDimensions.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUIButton.h"
#include "BsGUISliderHandle.h"
#include "BsGUISpace.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 GUIScrollBar::ButtonScrollAmount = 10;

	GUIScrollBar::GUIScrollBar(bool horizontal, const String& styleName, const GUIDimensions& dimensions)
		:GUIElement(styleName, dimensions), mHorizontal(horizontal)
	{
		mImageSprite = bs_new<ImageSprite>();

		if(mHorizontal)
		{
			mLayout = GUILayoutX::create();
			_registerChildElement(mLayout);

			mUpBtn = GUIButton::create(HString(L""), "ScrollLeftBtn");
			mDownBtn = GUIButton::create(HString(L""), "ScrollRightBtn");

			mHandleBtn = GUISliderHandle::create(mHorizontal, false, "ScrollBarHorzBtn");
		}
		else
		{
			mLayout = GUILayoutY::create();
			_registerChildElement(mLayout);

			mUpBtn = GUIButton::create(HString(L""), "ScrollUpBtn");
			mDownBtn = GUIButton::create(HString(L""), "ScrollDownBtn");

			mHandleBtn = GUISliderHandle::create(mHorizontal, false, "ScrollBarVertBtn");
		}

		mLayout->addNewElement<GUIFixedSpace>(2);
		mLayout->addElement(mUpBtn);
		mLayout->addElement(mHandleBtn);
		mLayout->addElement(mDownBtn);
		mLayout->addNewElement<GUIFixedSpace>(2);

		mHandleBtn->onHandleMoved.connect(std::bind(&GUIScrollBar::handleMoved, this, _1));

		mUpBtn->onClick.connect(std::bind(&GUIScrollBar::upButtonClicked, this));
		mDownBtn->onClick.connect(std::bind(&GUIScrollBar::downButtonClicked, this));
	}

	GUIScrollBar::~GUIScrollBar()
	{
		bs_delete(mImageSprite);

		GUIElement::destroy(mUpBtn);
		GUIElement::destroy(mDownBtn);
		GUIElement::destroy(mHandleBtn);
	}

	UINT32 GUIScrollBar::_getNumRenderElements() const
	{
		return mImageSprite->getNumRenderElements();
	}

	const SpriteMaterialInfo& GUIScrollBar::_getMaterial(UINT32 renderElementIdx) const
	{
		return mImageSprite->getMaterialInfo(renderElementIdx);
	}

	UINT32 GUIScrollBar::_getNumQuads(UINT32 renderElementIdx) const
	{
		return mImageSprite->getNumQuads(renderElementIdx);
	}

	void GUIScrollBar::updateRenderElementsInternal()
	{
		IMAGE_SPRITE_DESC desc;

		if(_getStyle()->normal.texture != nullptr && _getStyle()->normal.texture.isLoaded())
			desc.texture = _getStyle()->normal.texture.getInternalPtr();

		desc.width = mLayoutData.area.width;
		desc.height = mLayoutData.area.height;
		desc.color = getTint();

		mImageSprite->update(desc, (UINT64)_getParentWidget());

		GUIElement::updateRenderElementsInternal();
	}

	void GUIScrollBar::updateClippedBounds()
	{
		mClippedBounds = Rect2I(0, 0, 0, 0); // We don't want any mouse input for this element. This is just a container.
	}

	Vector2I GUIScrollBar::_getOptimalSize() const
	{
		return mLayout->_getOptimalSize();
	}

	UINT32 GUIScrollBar::_getRenderElementDepth(UINT32 renderElementIdx) const
	{
		return _getDepth() + 2; // + 2 depth because child buttons use +1
	}

	UINT32 GUIScrollBar::_getRenderElementDepthRange() const
	{
		return 3;
	}

	void GUIScrollBar::_fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		Vector2I offset(mLayoutData.area.x, mLayoutData.area.y);
		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
			vertexStride, indexStride, renderElementIdx, offset, mLayoutData.getLocalClipRect());
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

		float oldHandlePos = mHandleBtn->getHandlePos();
		mHandleBtn->_setHandlePos(newHandlePos);

		if (oldHandlePos != mHandleBtn->getHandlePos())
		{
			mHandleBtn->_markLayoutAsDirty();

			if (!onScrollPositionChanged.empty())
				onScrollPositionChanged(newHandlePos);
		}
	}

	void GUIScrollBar::_setHandleSize(UINT32 size)
	{
		mHandleBtn->_setHandleSize(size);
	}

	void GUIScrollBar::_setScrollPos(float pct)
	{
		mHandleBtn->_setHandlePos(pct);
	}

	float GUIScrollBar::getScrollPos() const
	{
		return mHandleBtn->getHandlePos();
	}

	UINT32 GUIScrollBar::getMaxHandleSize() const
	{
		return mHandleBtn->getMaxSize();
	}

	UINT32 GUIScrollBar::getScrollableSize() const
	{
		return mHandleBtn->getScrollableSize();
	}

	void GUIScrollBar::setTint(const Color& color)
	{
		mUpBtn->setTint(color);
		mDownBtn->setTint(color);
		mHandleBtn->setTint(color);

		GUIElement::setTint(color);
	}
}
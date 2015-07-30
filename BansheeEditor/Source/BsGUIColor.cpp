#include "BsGUIColor.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsGUIDimensions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIHelper.h"
#include "BsBuiltinResources.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	const float GUIColor::ALPHA_SPLIT_POSITION = 0.75f;

	GUIColor::GUIColor(const String& styleName, const GUIDimensions& dimensions)
		:GUIElement(styleName, dimensions), mColorSprite(nullptr), mAlphaSprite(nullptr)
	{
		mColorSprite = bs_new<ImageSprite, PoolAlloc>();
		mAlphaSprite = bs_new<ImageSprite, PoolAlloc>();

		mColorImageDesc.texture = BuiltinResources::instance().getWhiteSpriteTexture().getInternalPtr();
		mAlphaImageDesc.texture = BuiltinResources::instance().getWhiteSpriteTexture().getInternalPtr();
	}

	GUIColor::~GUIColor()
	{
		bs_delete<PoolAlloc>(mColorSprite);
		bs_delete<PoolAlloc>(mAlphaSprite);
	}

	const String& GUIColor::getGUITypeName()
	{
		static String name = "Color";
		return name;
	}

	GUIColor* GUIColor::create(const String& styleName)
	{
		return new (bs_alloc<GUIColor, PoolAlloc>()) GUIColor(getStyleName<GUIColor>(styleName), GUIDimensions::create());
	}

	GUIColor* GUIColor::create(const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUIColor, PoolAlloc>()) GUIColor(getStyleName<GUIColor>(styleName), GUIDimensions::create(options));
	}

	void GUIColor::setColor(const Color& color)
	{
		mColor = color;

		_markLayoutAsDirty();
	}

	void GUIColor::setTint(const Color& color)
	{
		mTint = color;

		_markLayoutAsDirty();
	}

	UINT32 GUIColor::_getNumRenderElements() const
	{
		UINT32 numElements = mColorSprite->getNumRenderElements();
		numElements += mAlphaSprite->getNumRenderElements();

		return numElements;
	}

	const GUIMaterialInfo& GUIColor::_getMaterial(UINT32 renderElementIdx) const
	{
		UINT32 alphaSpriteIdx = mColorSprite->getNumRenderElements();

		if(renderElementIdx >= alphaSpriteIdx)
			return mAlphaSprite->getMaterial(alphaSpriteIdx - renderElementIdx);
		else
			return mColorSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUIColor::_getNumQuads(UINT32 renderElementIdx) const
	{
		UINT32 alphaSpriteIdx = mColorSprite->getNumRenderElements();

		UINT32 numQuads = 0;
		if(renderElementIdx >= alphaSpriteIdx)
			numQuads = mAlphaSprite->getNumQuads(alphaSpriteIdx - renderElementIdx);
		else
			numQuads = mColorSprite->getNumQuads(renderElementIdx);

		return numQuads;
	}

	void GUIColor::updateRenderElementsInternal()
	{		
		Color color = mColor * mTint;

		mColorImageDesc.color = color;
		mColorImageDesc.color.a = 1.0f;

		mAlphaImageDesc.color = Color::White * color.a;
		mAlphaImageDesc.color.a = 1.0f;

		mColorImageDesc.width = (UINT32)(mLayoutData.area.width * ALPHA_SPLIT_POSITION);
		mColorImageDesc.height = mLayoutData.area.height;

		mAlphaImageDesc.width = mLayoutData.area.width - mColorImageDesc.width;
		mAlphaImageDesc.height = mLayoutData.area.height;

		mColorSprite->update(mColorImageDesc, (UINT64)_getParentWidget());
		mAlphaSprite->update(mAlphaImageDesc, (UINT64)_getParentWidget());

		GUIElement::updateRenderElementsInternal();
	}

	void GUIColor::updateClippedBounds()
	{
		mClippedBounds = mLayoutData.area;

		if (mLayoutData.clipRect.width > 0 && mLayoutData.clipRect.height > 0)
			mClippedBounds.clip(mLayoutData.clipRect);
	}

	Vector2I GUIColor::_getOptimalSize() const
	{
		return GUIHelper::calcOptimalContentsSize(Vector2I(80, 10), *_getStyle(), _getDimensions()); // Arbitrary size
	}

	void GUIColor::_fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		UINT32 alphaSpriteIdx = mColorSprite->getNumRenderElements();

		Vector2I offset(mLayoutData.area.x, mLayoutData.area.y);
		if(renderElementIdx < alphaSpriteIdx)
		{
			mColorSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
				vertexStride, indexStride, renderElementIdx, offset, mLayoutData.getLocalClipRect());

			return;
		}
		else if(renderElementIdx >= alphaSpriteIdx)
		{
			Vector2I alphaOffset = offset;
			UINT32 xOffset = (UINT32)(mLayoutData.area.width * ALPHA_SPLIT_POSITION);
			alphaOffset.x += xOffset;

			Rect2I alphaClipRect = mLayoutData.getLocalClipRect();
			alphaClipRect.x -= xOffset;

			mAlphaSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
				vertexStride, indexStride, alphaSpriteIdx - renderElementIdx, alphaOffset, alphaClipRect);
		}
	}

	bool GUIColor::_mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseUp)
		{
			// TODO - Open color picker window
			//      - Trigger onValueChanged when done

			return true;
		}

		return false;
	}
}
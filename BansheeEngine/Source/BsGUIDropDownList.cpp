#include "BsGUIDropDownList.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "BsTextSprite.h"
#include "BsGUILayoutOptions.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIManager.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIDropDownList::getGUITypeName()
	{
		static String name = "DropDownList";
		return name;
	}

	GUIDropDownList::GUIDropDownList(GUIWidget& parent, const GUIElementStyle* style, const Vector<WString>::type& elements, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mElements(elements), mNumImageRenderElements(0), mSelectedIdx(0)
	{
		mImageSprite = cm_new<ImageSprite, PoolAlloc>();
		mTextSprite = cm_new<TextSprite, PoolAlloc>();

		mImageDesc.texture = mStyle->normal.texture;

		if(mImageDesc.texture != nullptr)
		{
			mImageDesc.width = mImageDesc.texture->getTexture()->getWidth();
			mImageDesc.height = mImageDesc.texture->getTexture()->getHeight();
		}

		mImageDesc.borderLeft = mStyle->border.left;
		mImageDesc.borderRight = mStyle->border.right;
		mImageDesc.borderTop = mStyle->border.top;
		mImageDesc.borderBottom = mStyle->border.bottom;
	}

	GUIDropDownList::~GUIDropDownList()
	{
		cm_delete<PoolAlloc>(mTextSprite);
		cm_delete<PoolAlloc>(mImageSprite);
	}

	GUIDropDownList* GUIDropDownList::create(GUIWidget& parent, const Vector<WString>::type& elements, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIDropDownList, PoolAlloc>()) GUIDropDownList(parent, style, elements, getDefaultLayoutOptions(style));
	}

	GUIDropDownList* GUIDropDownList::create(GUIWidget& parent, const Vector<WString>::type& elements, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIDropDownList, PoolAlloc>()) GUIDropDownList(parent, style, elements, layoutOptions);
	}

	UINT32 GUIDropDownList::getNumRenderElements() const
	{
		UINT32 numElements = mImageSprite->getNumRenderElements();
		numElements += mTextSprite->getNumRenderElements();

		return numElements;
	}

	const HMaterial& GUIDropDownList::getMaterial(UINT32 renderElementIdx) const
	{
		if(renderElementIdx >= mNumImageRenderElements)
			return mTextSprite->getMaterial(mNumImageRenderElements - renderElementIdx);
		else
			return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUIDropDownList::getNumQuads(UINT32 renderElementIdx) const
	{
		UINT32 numQuads = 0;
		if(renderElementIdx >= mNumImageRenderElements)
			numQuads = mTextSprite->getNumQuads(mNumImageRenderElements - renderElementIdx);
		else
			numQuads = mImageSprite->getNumQuads(renderElementIdx);

		return numQuads;
	}

	void GUIDropDownList::updateRenderElementsInternal()
	{		
		mImageDesc.width = mWidth;
		mImageDesc.height = mHeight;

		mImageSprite->update(mImageDesc);
		mNumImageRenderElements = mImageSprite->getNumRenderElements();

		TEXT_SPRITE_DESC textDesc;
		textDesc.text = mElements[mSelectedIdx];
		textDesc.font = mStyle->font;
		textDesc.fontSize = mStyle->fontSize;

		Rect textBounds = getContentBounds();

		textDesc.width = textBounds.width;
		textDesc.height = textBounds.height;
		textDesc.horzAlign = mStyle->textHorzAlign;
		textDesc.vertAlign = mStyle->textVertAlign;

		mTextSprite->update(textDesc);

		GUIElement::updateRenderElementsInternal();
	}

	void GUIDropDownList::updateClippedBounds()
	{
		mClippedBounds = mImageSprite->getBounds(mOffset, mClipRect);
	}

	UINT32 GUIDropDownList::_getOptimalWidth() const
	{
		if(mImageDesc.texture != nullptr)
		{
			return mImageDesc.texture->getTexture()->getWidth();
		}

		return 0;
	}

	UINT32 GUIDropDownList::_getOptimalHeight() const
	{
		if(mImageDesc.texture != nullptr)
		{
			return mImageDesc.texture->getTexture()->getHeight();
		}

		return 0;
	}

	UINT32 GUIDropDownList::_getRenderElementDepth(UINT32 renderElementIdx) const
	{
		if(renderElementIdx >= mNumImageRenderElements)
			return _getDepth();
		else
			return _getDepth() + 1;
	}

	void GUIDropDownList::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		if(renderElementIdx >= mNumImageRenderElements)
		{
			Rect textBounds = getContentBounds();
			Int2 offset(textBounds.x, textBounds.y);
			Rect textClipRect = getContentClipRect();

			mTextSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
				vertexStride, indexStride, mNumImageRenderElements - renderElementIdx, offset, textClipRect);
		}
		else
		{
			mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, 
				vertexStride, indexStride, renderElementIdx, mOffset, mClipRect);
		}
	}

	bool GUIDropDownList::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseOver)
		{
			mImageDesc.texture = mStyle->hover.texture;
			markContentAsDirty();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseOut)
		{
			mImageDesc.texture = mStyle->normal.texture;
			markContentAsDirty();

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseDown)
		{
			mImageDesc.texture = mStyle->active.texture;
			markContentAsDirty();

			GUIManager::instance().openDropDownBox(this, mElements, boost::bind(&GUIDropDownList::elementSelected, this, _1), _getParentWidget().getSkin());

			return true;
		}
		else if(ev.getType() == GUIMouseEventType::MouseUp)
		{
			mImageDesc.texture = mStyle->hover.texture;
			markContentAsDirty();

			return true;
		}

		return false;
	}

	void GUIDropDownList::elementSelected(CM::UINT32 idx)
	{
		if(!onSelectionChanged.empty())
			onSelectionChanged(idx);

		mSelectedIdx = idx;

		markContentAsDirty();
	}
}
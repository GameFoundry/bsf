#include "BsGUIElement.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUILayout.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIElement::GUIElement(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions, bool acceptsKeyboardFocus)
		:mParent(parent), mIsDirty(true), mParentLayout(nullptr), mLayoutOptions(layoutOptions), mWidth(0), mHeight(0), mDepth(0), mStyle(style),
		mAcceptsKeyboardFocus(acceptsKeyboardFocus)
	{
		mParent.registerElement(this);
	}

	GUIElement::~GUIElement()
	{
		if(mParentLayout != nullptr)
			mParentLayout->removeElement(this);
	}

	void GUIElement::updateRenderElements()
	{
		updateRenderElementsInternal();
		markAsClean();
	}

	void GUIElement::setLayoutOptions(const GUILayoutOptions& layoutOptions) 
	{
		if(layoutOptions.maxWidth < layoutOptions.minWidth)
		{
			CM_EXCEPT(InvalidParametersException, "Maximum width is less than minimum width! Max width: " + 
			toString(layoutOptions.maxWidth) + ". Min width: " + toString(layoutOptions.minWidth));
		}

		if(layoutOptions.maxHeight < layoutOptions.minHeight)
		{
			CM_EXCEPT(InvalidParametersException, "Maximum height is less than minimum height! Max height: " + 
			toString(layoutOptions.maxHeight) + ". Min height: " + toString(layoutOptions.minHeight));
		}

		mLayoutOptions = layoutOptions; 
	}


	bool GUIElement::mouseEvent(const GUIMouseEvent& ev)
	{
		return false;
	}

	bool GUIElement::keyEvent(const GUIKeyEvent& ev)
	{
		return false;
	}

	bool GUIElement::commandEvent(const GUICommandEvent& ev)
	{
		return false;
	}

	void GUIElement::_setWidgetDepth(UINT8 depth) 
	{ 
		mDepth |= depth << 24; 
		markAsDirty();
	}

	void GUIElement::_setAreaDepth(UINT16 depth) 
	{ 
		mDepth |= depth << 8; 
		markAsDirty();
	}

	void GUIElement::_setOffset(const CM::Int2& offset) 
	{ 
		mOffset = offset; 
		markAsDirty();
	}

	void GUIElement::_setWidth(UINT32 width) 
	{ 
		mWidth = width; 
		markAsDirty();
	}

	void GUIElement::_setHeight(UINT32 height) 
	{ 
		mHeight = height; 
		markAsDirty();
	}

	void GUIElement::_setClipRect(const CM::Rect& clipRect) 
	{ 
		mClipRect = clipRect; 
		markAsDirty();
	}

	Rect GUIElement::getContentBounds() const
	{
		Rect bounds = _getBounds();
		
		bounds.x += mStyle->margins.left;
		bounds.y += mStyle->margins.top;
		bounds.width = (UINT32)std::max(0, (INT32)bounds.width - (INT32)(mStyle->margins.left + mStyle->margins.right));
		bounds.height = (UINT32)std::max(0, (INT32)bounds.height - (INT32)(mStyle->margins.top + mStyle->margins.bottom));

		return bounds;
	}

	bool GUIElement::_isInBounds(const CM::Int2 position) const
	{
		Rect contentBounds = getContentBounds();

		return contentBounds.contains(position);
	}

	void GUIElement::markAsDirty() 
	{ 
		if(!mIsDirty)
		{
			if(mParentLayout != nullptr)
				mParentLayout->_markAsDirty();

			mIsDirty = true; 
		}
	}

	GUILayoutOptions GUIElement::getDefaultLayoutOptions(const GUIElementStyle* style)
	{
		GUILayoutOptions layoutOptions;

		layoutOptions.fixedWidth = style->fixedWidth;
		layoutOptions.fixedHeight = style->fixedHeight;
		layoutOptions.width = style->width;
		layoutOptions.height = style->height;
		layoutOptions.minWidth = style->minWidth;
		layoutOptions.maxWidth = style->maxWidth;
		layoutOptions.minHeight = style->minHeight;
		layoutOptions.maxHeight = style->maxHeight;

		return layoutOptions;
	}

	void GUIElement::_destroyInternal(GUIElement* element)
	{
		cm_delete<PoolAlloc>(element);
	}

	void GUIElement::destroy(GUIElement* element)
	{
		element->mParent.unregisterElement(element);

		cm_delete<PoolAlloc>(element);
	}
}
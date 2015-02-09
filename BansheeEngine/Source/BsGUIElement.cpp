#include "BsGUIElement.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUILayout.h"
#include "BsGUIManager.h"
#include "BsException.h"

namespace BansheeEngine
{
	GUIElement::GUIElement(const String& styleName, const GUILayoutOptions& layoutOptions)
		:mLayoutOptions(layoutOptions), mWidth(0), mHeight(0), mDepth(0), mStyle(nullptr),
		mIsDestroyed(false), mStyleName(styleName)
	{
		_refreshStyle();
	}

	GUIElement::~GUIElement()
	{ }

	void GUIElement::updateRenderElements()
	{
		updateRenderElementsInternal();
		_markAsClean();
	}

	void GUIElement::updateRenderElementsInternal()
	{
		updateClippedBounds();
	}

	void GUIElement::setLayoutOptions(const GUILayoutOptions& layoutOptions) 
	{
		if(layoutOptions.maxWidth < layoutOptions.minWidth)
		{
			BS_EXCEPT(InvalidParametersException, "Maximum width is less than minimum width! Max width: " + 
			toString(layoutOptions.maxWidth) + ". Min width: " + toString(layoutOptions.minWidth));
		}

		if(layoutOptions.maxHeight < layoutOptions.minHeight)
		{
			BS_EXCEPT(InvalidParametersException, "Maximum height is less than minimum height! Max height: " + 
			toString(layoutOptions.maxHeight) + ". Min height: " + toString(layoutOptions.minHeight));
		}

		mLayoutOptions = layoutOptions; 
	}

	void GUIElement::setStyle(const String& styleName)
	{
		mStyleName = styleName;
		_refreshStyle();
	}

	bool GUIElement::mouseEvent(const GUIMouseEvent& ev)
	{
		return false;
	}

	bool GUIElement::textInputEvent(const GUITextInputEvent& ev)
	{
		return false;
	}

	bool GUIElement::commandEvent(const GUICommandEvent& ev)
	{
		return false;
	}

	bool GUIElement::virtualButtonEvent(const GUIVirtualButtonEvent& ev)
	{
		return false;
	}

	void GUIElement::_setWidgetDepth(UINT8 depth) 
	{ 
		mDepth |= depth << 24; 
		markMeshAsDirty();
	}

	void GUIElement::_setAreaDepth(UINT16 depth) 
	{ 
		mDepth |= depth << 8; 
		markMeshAsDirty();
	}

	void GUIElement::_setElementDepth(UINT8 depth)
	{
		mDepth |= depth;
		markMeshAsDirty();
	}

	void GUIElement::_setOffset(const Vector2I& offset) 
	{ 
		if(mOffset != offset)
		{
			markMeshAsDirty();

			mOffset = offset;
			updateClippedBounds();
		}
	}

	void GUIElement::_setWidth(UINT32 width) 
	{ 
		if(mWidth != width)
			markContentAsDirty();

		mWidth = width; 
	}

	void GUIElement::_setHeight(UINT32 height) 
	{ 
		if(mHeight != height)
			markContentAsDirty();

		mHeight = height;
	}

	void GUIElement::_setClipRect(const Rect2I& clipRect) 
	{ 
		if(mClipRect != clipRect)
		{
			markMeshAsDirty();

			mClipRect = clipRect; 
			updateClippedBounds();
		}
	}

	void GUIElement::_changeParentWidget(GUIWidget* widget)
	{
		bool doRefreshStyle = false;
		if(mParentWidget != widget)
		{
			if(mParentWidget != nullptr)
				mParentWidget->unregisterElement(this);

			if(widget != nullptr)
				widget->registerElement(this);

			doRefreshStyle = true;
		}

		GUIElementBase::_changeParentWidget(widget);

		if(doRefreshStyle)
			_refreshStyle();
	}

	const RectOffset& GUIElement::_getPadding() const
	{
		if(mStyle != nullptr)
			return mStyle->padding;
		else
		{
			static RectOffset padding;

			return padding;
		}
	}

	Vector2I GUIElement::_calculateOptimalLayoutSize() const
	{
		if (mIsDisabled)
			return Vector2I(0, 0);

		const GUILayoutOptions& layoutOptions = _getLayoutOptions();

		UINT32 optimalWidth = 0;
		UINT32 optimalHeight = 0;

		Vector2I optimalSize;
		if (!layoutOptions.fixedWidth || !layoutOptions.fixedHeight)
			optimalSize = _getOptimalSize();

		if (layoutOptions.fixedHeight)
		{
			optimalHeight = layoutOptions.height;
		}
		else
		{
			optimalHeight = optimalSize.y;

			if (layoutOptions.minHeight > 0)
				optimalHeight = std::max(layoutOptions.minHeight, optimalHeight);

			if (layoutOptions.maxHeight > 0)
				optimalHeight = std::min(layoutOptions.maxHeight, optimalHeight);
		}

		if (layoutOptions.fixedWidth)
			optimalWidth = layoutOptions.width;
		else
		{
			optimalWidth = optimalSize.x;

			if (layoutOptions.minWidth > 0)
				optimalWidth = std::max(layoutOptions.minWidth, optimalWidth);

			if (layoutOptions.maxWidth > 0)
				optimalWidth = std::min(layoutOptions.maxWidth, optimalWidth);
		}

		return Vector2I(optimalWidth, optimalHeight);
	}

	Rect2I GUIElement::_getCachedBounds() const
	{
		return Rect2I(mOffset.x, mOffset.y, mWidth, mHeight);
	}

	void GUIElement::setFocus(bool enabled)
	{
		GUIManager::instance().setFocus(this, enabled);
	}

	void GUIElement::setLayoutOptions(const GUIOptions& layoutOptions)
	{
		mLayoutOptions = GUILayoutOptions::create(layoutOptions);
		mLayoutOptions.updateWithStyle(mStyle);

		markContentAsDirty();
	}

	Rect2I GUIElement::getVisibleBounds() const
	{
		Rect2I bounds = _getClippedBounds();
		
		bounds.x += mStyle->margins.left;
		bounds.y += mStyle->margins.top;
		bounds.width = (UINT32)std::max(0, (INT32)bounds.width - (INT32)(mStyle->margins.left + mStyle->margins.right));
		bounds.height = (UINT32)std::max(0, (INT32)bounds.height - (INT32)(mStyle->margins.top + mStyle->margins.bottom));

		return bounds;
	}

	Rect2I GUIElement::getContentBounds() const
	{
		Rect2I bounds;

		bounds.x = mOffset.x + mStyle->margins.left + mStyle->contentOffset.left;
		bounds.y = mOffset.y + mStyle->margins.top + mStyle->contentOffset.top;
		bounds.width = (UINT32)std::max(0, (INT32)mWidth - 
			(INT32)(mStyle->margins.left + mStyle->margins.right + mStyle->contentOffset.left + mStyle->contentOffset.right));
		bounds.height = (UINT32)std::max(0, (INT32)mHeight - 
			(INT32)(mStyle->margins.top + mStyle->margins.bottom + mStyle->contentOffset.top + mStyle->contentOffset.bottom));

		return bounds;
	}

	Rect2I GUIElement::getContentClipRect() const
	{
		Rect2I contentBounds = getContentBounds();
		
		// Transform into element space so we can clip it using the element clip rectangle
		Vector2I offsetDiff = Vector2I(contentBounds.x - mOffset.x, contentBounds.y - mOffset.y);
		Rect2I contentClipRect(offsetDiff.x, offsetDiff.y, contentBounds.width, contentBounds.height);
		contentClipRect.clip(mClipRect);

		// Transform into content sprite space
		contentClipRect.x -= offsetDiff.x;
		contentClipRect.y -= offsetDiff.y;

		return contentClipRect;
	}

	bool GUIElement::_isInBounds(const Vector2I position) const
	{
		Rect2I contentBounds = getVisibleBounds();

		return contentBounds.contains(position);
	}

	void GUIElement::_refreshStyle()
	{
		const GUIElementStyle* newStyle = nullptr;
		if(_getParentWidget() != nullptr)
			newStyle = _getParentWidget()->getSkin().getStyle(mStyleName);
		else
			newStyle = &GUISkin::DefaultStyle;

		if(newStyle != mStyle)
		{
			mStyle = newStyle;
			mLayoutOptions.updateWithStyle(mStyle);
			styleUpdated();

			markContentAsDirty();
		}
	}

	const String& GUIElement::getSubStyleName(const String& subStyleTypeName) const
	{
		auto iterFind = mStyle->subStyles.find(subStyleTypeName);

		if (iterFind != mStyle->subStyles.end())
			return iterFind->second;
		else
			return StringUtil::BLANK;
	}

	void GUIElement::destroy(GUIElement* element)
	{
		if(element->mIsDestroyed)
			return;

		if(element->mParentWidget != nullptr)
			element->mParentWidget->unregisterElement(element);

		if (element->mParentElement != nullptr)
			element->mParentElement->_unregisterChildElement(element);

		element->mIsDestroyed = true;

		GUIManager::instance().queueForDestroy(element);
	}
}
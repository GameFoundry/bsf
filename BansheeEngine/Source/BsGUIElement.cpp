#include "BsGUIElement.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUILayout.h"
#include "BsGUIManager.h"
#include "BsException.h"
#include "BsGUILayoutUtility.h"

namespace BansheeEngine
{
	GUIElement::GUIElement(const String& styleName, const GUIDimensions& dimensions)
		:GUIElementBase(dimensions), mStyle(&GUISkin::DefaultStyle),
		mIsDestroyed(false), mStyleName(styleName)
	{
		// Style is set to default here, and the proper one is assigned once GUI element
		// is assigned to a parent (that's when the active GUI skin becomes known)
	}

	GUIElement::~GUIElement()
	{ }

	void GUIElement::_updateRenderElements()
	{
		updateRenderElementsInternal();
	}

	void GUIElement::updateRenderElementsInternal()
	{
		updateClippedBounds();
	}

	void GUIElement::setStyle(const String& styleName)
	{
		mStyleName = styleName;
		_refreshStyle();
	}

	bool GUIElement::_mouseEvent(const GUIMouseEvent& ev)
	{
		return false;
	}

	bool GUIElement::_textInputEvent(const GUITextInputEvent& ev)
	{
		return false;
	}

	bool GUIElement::_commandEvent(const GUICommandEvent& ev)
	{
		return false;
	}

	bool GUIElement::_virtualButtonEvent(const GUIVirtualButtonEvent& ev)
	{
		return false;
	}

	void GUIElement::_setElementDepth(UINT8 depth)
	{
		mLayoutData.depth = depth | (mLayoutData.depth & 0xFFFFFF00);
		_markMeshAsDirty();
	}

	UINT8 GUIElement::_getElementDepth() const
	{
		return mLayoutData.depth & 0xFF;
	}

	void GUIElement::_setLayoutData(const GUILayoutData& data)
	{
		// Preserve element depth as that is not controlled by layout but is stored
		// there only for convenience
		UINT8 elemDepth = _getElementDepth();
		GUIElementBase::_setLayoutData(data);
		_setElementDepth(elemDepth);

		updateClippedBounds();
	}

	void GUIElement::_changeParentWidget(GUIWidget* widget)
	{
		bool doRefreshStyle = false;
		if(mParentWidget != widget)
			doRefreshStyle = true;

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

	void GUIElement::setFocus(bool enabled)
	{
		GUIManager::instance().setFocus(this, enabled);
	}

	void GUIElement::resetDimensions()
	{
		mDimensions = GUIDimensions::create();
		mDimensions.updateWithStyle(mStyle);

		_markContentAsDirty();
	}

	Rect2I GUIElement::getCachedVisibleBounds() const
	{
		Rect2I bounds = _getClippedBounds();
		
		bounds.x += mStyle->margins.left;
		bounds.y += mStyle->margins.top;
		bounds.width = (UINT32)std::max(0, (INT32)bounds.width - (INT32)(mStyle->margins.left + mStyle->margins.right));
		bounds.height = (UINT32)std::max(0, (INT32)bounds.height - (INT32)(mStyle->margins.top + mStyle->margins.bottom));

		return bounds;
	}

	Rect2I GUIElement::getCachedContentBounds() const
	{
		Rect2I bounds;

		bounds.x = mLayoutData.area.x + mStyle->margins.left + mStyle->contentOffset.left;
		bounds.y = mLayoutData.area.y + mStyle->margins.top + mStyle->contentOffset.top;
		bounds.width = (UINT32)std::max(0, (INT32)mLayoutData.area.width -
			(INT32)(mStyle->margins.left + mStyle->margins.right + mStyle->contentOffset.left + mStyle->contentOffset.right));
		bounds.height = (UINT32)std::max(0, (INT32)mLayoutData.area.height -
			(INT32)(mStyle->margins.top + mStyle->margins.bottom + mStyle->contentOffset.top + mStyle->contentOffset.bottom));

		return bounds;
	}

	Rect2I GUIElement::getCachedContentClipRect() const
	{
		Rect2I contentBounds = getCachedContentBounds();
		
		// Transform into element space so we can clip it using the element clip rectangle
		Vector2I offsetDiff = Vector2I(contentBounds.x - mLayoutData.area.x, contentBounds.y - mLayoutData.area.y);
		Rect2I contentClipRect(offsetDiff.x, offsetDiff.y, contentBounds.width, contentBounds.height);
		contentClipRect.clip(mLayoutData.getLocalClipRect());

		// Transform into content sprite space
		contentClipRect.x -= offsetDiff.x;
		contentClipRect.y -= offsetDiff.y;

		return contentClipRect;
	}

	bool GUIElement::_isInBounds(const Vector2I position) const
	{
		Rect2I contentBounds = getCachedVisibleBounds();

		return contentBounds.contains(position);
	}

	void GUIElement::_refreshStyle()
	{
		const GUIElementStyle* newStyle = nullptr;
		if(_getParentWidget() != nullptr && !mStyleName.empty())
			newStyle = _getParentWidget()->getSkin().getStyle(mStyleName);
		else
			newStyle = &GUISkin::DefaultStyle;

		if(newStyle != mStyle)
		{
			mStyle = newStyle;
			mDimensions.updateWithStyle(mStyle);
			styleUpdated();

			_markContentAsDirty();
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

		if (element->mParentElement != nullptr)
			element->mParentElement->_unregisterChildElement(element);

		element->mIsDestroyed = true;

		GUIManager::instance().queueForDestroy(element);
	}

	Rect2I GUIElement::getVisibleBounds()
	{
		Rect2I bounds = getBounds();

		bounds.x += mStyle->margins.left;
		bounds.y += mStyle->margins.top;
		bounds.width = (UINT32)std::max(0, (INT32)bounds.width - (INT32)(mStyle->margins.left + mStyle->margins.right));
		bounds.height = (UINT32)std::max(0, (INT32)bounds.height - (INT32)(mStyle->margins.top + mStyle->margins.bottom));

		return bounds;
	}
}
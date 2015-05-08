#include "BsGUIElementBase.h"
#include "BsGUILayout.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUIPanel.h"
#include "BsGUISpace.h"
#include "BsGUIElement.h"
#include "BsException.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutUtility.h"

namespace BansheeEngine
{
	GUIElementBase::GUIElementBase()
		:mIsDirty(true), mParentElement(nullptr), mIsDisabled(false), 
		mParentWidget(nullptr), mWidth(0), mHeight(0), mAnchorParent(nullptr), 
		mUpdateParent(nullptr), mPanelDepthRange(-1), mDepth(0)
	{
		_setAreaDepth(0);
	}

	GUIElementBase::GUIElementBase(const GUIDimensions& dimensions)
		:mIsDirty(true), mParentElement(nullptr), mIsDisabled(false),
		mParentWidget(nullptr), mDimensions(dimensions), mWidth(0), 
		mHeight(0), mAnchorParent(nullptr), mUpdateParent(nullptr), 
		mPanelDepthRange(-1), mDepth(0)
	{
		_setAreaDepth(0);
	}

	GUIElementBase::~GUIElementBase()
	{
		Vector<GUIElementBase*> childCopy = mChildren;
		for (auto& child : childCopy)
		{
			if (child->_getType() == Type::Element)
			{
				GUIElement* element = static_cast<GUIElement*>(child);
				GUIElement::destroy(element);
			}
			else if (child->_getType() == Type::Layout || child->_getType() == GUIElementBase::Type::Panel)
			{
				GUILayout* layout = static_cast<GUILayout*>(child);
				GUILayout::destroy(layout);
			}
			else if (child->_getType() == Type::FixedSpace)
			{
				GUIFixedSpace* space = static_cast<GUIFixedSpace*>(child);
				GUIFixedSpace::destroy(space);
			}
			else if (child->_getType() == Type::FlexibleSpace)
			{
				GUIFlexibleSpace* space = static_cast<GUIFlexibleSpace*>(child);
				GUIFlexibleSpace::destroy(space);
			}
		}
	}

	void GUIElementBase::setPosition(INT32 x, INT32 y)
	{
		mDimensions.x = x;
		mDimensions.y = y;

		markMeshAsDirty();
	}

	void GUIElementBase::setWidth(UINT32 width)
	{
		bool isFixedBefore = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		mDimensions.flags |= GUIDF_FixedWidth | GUIDF_OverWidth;
		mDimensions.minWidth = mDimensions.maxWidth = width;

		bool isFixedAfter = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		if (isFixedBefore != isFixedAfter)
			refreshChildUpdateParents();
			
		markContentAsDirty();
	}

	void GUIElementBase::setFlexibleWidth(UINT32 minWidth, UINT32 maxWidth)
	{
		if (maxWidth < minWidth)
			std::swap(minWidth, maxWidth);

		bool isFixedBefore = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		mDimensions.flags |= GUIDF_OverWidth;
		mDimensions.flags &= ~GUIDF_FixedWidth;
		mDimensions.minWidth = minWidth;
		mDimensions.maxWidth = maxWidth;

		bool isFixedAfter = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		if (isFixedBefore != isFixedAfter)
			refreshChildUpdateParents();

		markContentAsDirty();
	}

	void GUIElementBase::setHeight(UINT32 height)
	{
		bool isFixedBefore = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		mDimensions.flags |= GUIDF_FixedHeight | GUIDF_OverHeight;
		mDimensions.minHeight = mDimensions.maxHeight = height;

		bool isFixedAfter = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		if (isFixedBefore != isFixedAfter)
			refreshChildUpdateParents();

		markContentAsDirty();
	}

	void GUIElementBase::setFlexibleHeight(UINT32 minHeight, UINT32 maxHeight)
	{
		if (maxHeight < minHeight)
			std::swap(minHeight, maxHeight);

		bool isFixedBefore = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		mDimensions.flags |= GUIDF_OverHeight;
		mDimensions.flags &= ~GUIDF_FixedHeight;
		mDimensions.minHeight = minHeight;
		mDimensions.maxHeight = maxHeight;

		bool isFixedAfter = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		if (isFixedBefore != isFixedAfter)
			refreshChildUpdateParents();

		markContentAsDirty();
	}

	void GUIElementBase::resetDimensions()
	{
		bool isFixedBefore = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		mDimensions = GUIDimensions::create();

		bool isFixedAfter = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		if (isFixedBefore != isFixedAfter)
			refreshChildUpdateParents();

		markContentAsDirty();
	}

	Rect2I GUIElementBase::getBounds() const
	{
		return GUILayoutUtility::calcBounds(this);
	}

	Rect2I GUIElementBase::getVisibleBounds() const
	{
		return getBounds();
	}

	void GUIElementBase::_setPosition(const Vector2I& offset)
	{
		mOffset = offset;
	}

	void GUIElementBase::_setWidth(UINT32 width)
	{
		mWidth = width;
	}

	void GUIElementBase::_setHeight(UINT32 height)
	{
		mHeight = height;
	}

	void GUIElementBase::_setWidgetDepth(UINT8 depth)
	{
		UINT32 shiftedDepth = depth << 24;

		mDepth = shiftedDepth | (mDepth & 0x00FFFFFF);
	}

	void GUIElementBase::_setAreaDepth(INT16 depth)
	{
		UINT32 signedDepth = ((INT32)depth + 32768) << 8;

		mDepth = signedDepth | (mDepth & 0xFF0000FF);;
	}

	void GUIElementBase::_setClipRect(const Rect2I& clipRect)
	{
		mClipRect = clipRect;
	}

	void GUIElementBase::_setPanelDepthRange(UINT16 min, UINT16 max)
	{
		mPanelDepthRange = (min << 16) | max;
	}

	void GUIElementBase::_getPanelDepthRange(UINT16& min, UINT16& max)
	{
		min = mPanelDepthRange >> 16;
		max = mPanelDepthRange & 0xFFFF;
	}

	UINT8 GUIElementBase::_getWidgetDepth() const
	{
		return (mDepth >> 24) & 0xFF;
	}

	INT16 GUIElementBase::_getAreaDepth() const
	{
		return (((INT32)mDepth >> 8) & 0xFFFF) - 32768;
	}

	void GUIElementBase::_markAsClean()
	{
		mIsDirty = false;
	}

	void GUIElementBase::markContentAsDirty() 
	{ 
		if(_isDisabled())
			return;

		mIsDirty = true;
	}

	void GUIElementBase::markMeshAsDirty()
	{
		if(_isDisabled())
			return;

		if (mParentWidget != nullptr)
			mParentWidget->_markMeshDirty(this);
	}

	void GUIElementBase::enableRecursively()
	{
		if (mParentElement != nullptr && mParentElement->mIsDisabled)
			return; // Cannot enable if parent is disabled

		// Make sure to mark everything as dirty, as we didn't track any dirty flags while the element was disabled
		mIsDisabled = false;
		markContentAsDirty();

		for(auto& elem : mChildren)
		{
			elem->enableRecursively();
		}
	}

	void GUIElementBase::disableRecursively()
	{
		markMeshAsDirty(); // Just need to hide the mesh
		mIsDisabled = true;

		for(auto& elem : mChildren)
		{
			elem->disableRecursively();
		}
	}

	void GUIElementBase::_updateLayout(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I clipRect, 
		UINT8 widgetDepth, INT16 panelDepth, UINT16 panelDepthRangeMin, UINT16 panelDepthRangeMax)
	{
		_updateOptimalLayoutSizes(); // We calculate optimal sizes of all layouts as a pre-processing step, as they are requested often during update
		_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, panelDepth, panelDepthRangeMin, panelDepthRangeMax);
	}

	void GUIElementBase::_updateOptimalLayoutSizes()
	{
		for(auto& child : mChildren)
		{
			child->_updateOptimalLayoutSizes();
		}
	}

	void GUIElementBase::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I clipRect, 
		UINT8 widgetDepth, INT16 panelDepth, UINT16 panelDepthRangeMin, UINT16 panelDepthRangeMax)
	{
		for(auto& child : mChildren)
		{
			child->_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, panelDepth, panelDepthRangeMin, panelDepthRangeMax);
		}
	}

	LayoutSizeRange GUIElementBase::_calculateLayoutSizeRange() const
	{
		if (mIsDisabled)
			return LayoutSizeRange();

		const GUIDimensions& dimensions = _getDimensions();
		return dimensions.calculateSizeRange(_getOptimalSize());
	}

	void GUIElementBase::_getElementAreas(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I* elementAreas, UINT32 numElements,
		const Vector<LayoutSizeRange>& sizeRanges, const LayoutSizeRange& mySizeRange) const
	{
		assert(mChildren.size() == 0);
	}

	void GUIElementBase::_setParent(GUIElementBase* parent) 
	{ 
		if(mParentElement != parent)
		{
			mParentElement = parent; 
			_updateAUParents();

			if (parent != nullptr)
			{
				if (_getParentWidget() != parent->_getParentWidget())
					_changeParentWidget(parent->_getParentWidget());
			}
			else
				_changeParentWidget(nullptr);
		}
	}

	void GUIElementBase::_registerChildElement(GUIElementBase* element)
	{
		GUIElementBase* parentElement = element->_getParent();
		if(parentElement != nullptr)
		{
			parentElement->_unregisterChildElement(element);
		}

		element->_setParent(this);
		mChildren.push_back(element);

		if (mIsDisabled)
			element->disableRecursively();

		markContentAsDirty();
	}

	void GUIElementBase::_unregisterChildElement(GUIElementBase* element)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUIElementBase* child = *iter;

			if (child == element)
			{
				mChildren.erase(iter);
				element->_setParent(nullptr);
				foundElem = true;

				markContentAsDirty();
				break;
			}
		}

		if(!foundElem)
			BS_EXCEPT(InvalidParametersException, "Provided element is not a part of this element.");
	}

	void GUIElementBase::_changeParentWidget(GUIWidget* widget)
	{
		if (mParentWidget != widget)
		{
			if (mParentWidget != nullptr)
				mParentWidget->_unregisterElement(this);

			if (widget != nullptr)
				widget->_registerElement(this);
		}

		mParentWidget = widget;

		for(auto& child : mChildren)
		{
			child->_changeParentWidget(widget);
		}

		markContentAsDirty();
	}

	void GUIElementBase::_updateAUParents()
	{
		GUIElementBase* updateParent = nullptr;
		if (mParentElement != nullptr)
			updateParent = mParentElement->findUpdateParent();

		GUIPanel* anchorParent = nullptr;
		GUIElementBase* currentParent = mParentElement;
		while (currentParent != nullptr)
		{
			if (currentParent->_getType() == Type::Panel)
			{
				anchorParent = static_cast<GUIPanel*>(currentParent);
				break;
			}

			currentParent = currentParent->mParentElement;
		}

		setAnchorParent(anchorParent);
		setUpdateParent(updateParent);
	}

	GUIElementBase* GUIElementBase::findUpdateParent()
	{
		GUIElementBase* currentElement = this;
		while (currentElement != nullptr)
		{
			const GUIDimensions& parentDimensions = currentElement->_getDimensions();
			bool boundsDependOnChildren = !parentDimensions.fixedHeight() || !parentDimensions.fixedWidth();

			if (!boundsDependOnChildren)
				return currentElement;

			currentElement = currentElement->mParentElement;
		}

		return nullptr;
	}

	void GUIElementBase::refreshChildUpdateParents()
	{
		GUIElementBase* updateParent = findUpdateParent();

		for (auto& child : mChildren)
			child->setUpdateParent(updateParent);
	}

	void GUIElementBase::setAnchorParent(GUIPanel* anchorParent)
	{
		mAnchorParent = anchorParent;

		if (_getType() == Type::Panel)
			return;

		for (auto& child : mChildren)
			child->setAnchorParent(anchorParent);
	}

	void GUIElementBase::setUpdateParent(GUIElementBase* updateParent)
	{
		mUpdateParent = updateParent;

		const GUIDimensions& dimensions = _getDimensions();
		bool boundsDependOnChildren = !dimensions.fixedHeight() || !dimensions.fixedWidth();

		if (!boundsDependOnChildren)
			return;

		for (auto& child : mChildren)
			child->setUpdateParent(updateParent);
	}
}
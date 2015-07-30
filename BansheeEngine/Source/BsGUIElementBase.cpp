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
#include "BsProfilerCPU.h"

namespace BansheeEngine
{
	GUIElementBase::GUIElementBase()
		:mIsDirty(true), mParentElement(nullptr), mIsDisabled(false), 
		mParentWidget(nullptr), mAnchorParent(nullptr), mUpdateParent(nullptr)
	{

	}

	GUIElementBase::GUIElementBase(const GUIDimensions& dimensions)
		:mIsDirty(true), mParentElement(nullptr), mIsDisabled(false),
		mParentWidget(nullptr), mDimensions(dimensions), 
		mAnchorParent(nullptr), mUpdateParent(nullptr)
	{

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

		_markMeshAsDirty();
	}

	void GUIElementBase::setWidth(UINT32 width)
	{
		bool isFixedBefore = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		mDimensions.flags |= GUIDF_FixedWidth | GUIDF_OverWidth;
		mDimensions.minWidth = mDimensions.maxWidth = width;

		bool isFixedAfter = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		if (isFixedBefore != isFixedAfter)
			refreshChildUpdateParents();
			
		_markLayoutAsDirty();
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

		_markLayoutAsDirty();
	}

	void GUIElementBase::setHeight(UINT32 height)
	{
		bool isFixedBefore = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		mDimensions.flags |= GUIDF_FixedHeight | GUIDF_OverHeight;
		mDimensions.minHeight = mDimensions.maxHeight = height;

		bool isFixedAfter = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		if (isFixedBefore != isFixedAfter)
			refreshChildUpdateParents();

		_markLayoutAsDirty();
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

		_markLayoutAsDirty();
	}

	void GUIElementBase::resetDimensions()
	{
		bool isFixedBefore = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		mDimensions = GUIDimensions::create();

		bool isFixedAfter = (mDimensions.flags & GUIDF_FixedWidth) != 0 && (mDimensions.flags & GUIDF_FixedHeight) != 0;

		if (isFixedBefore != isFixedAfter)
			refreshChildUpdateParents();

		_markLayoutAsDirty();
	}

	Rect2I GUIElementBase::getBounds(GUIPanel* relativeTo)
	{
		if (relativeTo == nullptr)
			relativeTo = mAnchorParent;

		Rect2I anchorBounds;
		if (relativeTo != nullptr)
			anchorBounds = relativeTo->getGlobalBounds();

		if (mUpdateParent != nullptr && mUpdateParent->_isDirty() && mParentWidget != nullptr)
			mParentWidget->_updateLayout(mUpdateParent);

		Rect2I bounds = mLayoutData.area;
		bounds.x -= anchorBounds.x;
		bounds.y -= anchorBounds.y;
		
		return bounds;
	}

	Rect2I GUIElementBase::getGlobalBounds()
	{
		if (mUpdateParent != nullptr && mUpdateParent->_isDirty() && mParentWidget != nullptr)
			mParentWidget->_updateLayout(mUpdateParent);

		return mLayoutData.area;
	}

	Rect2I GUIElementBase::getVisibleBounds()
	{
		return getBounds();
	}
	
	void GUIElementBase::_markAsClean()
	{
		mIsDirty = false;
	}

	void GUIElementBase::_markLayoutAsDirty() 
	{ 
		if(_isDisabled())
			return;

		if (mUpdateParent != nullptr)
			mUpdateParent->mIsDirty = true;
		else
			mIsDirty = true;
	}

	void GUIElementBase::_markMeshAsDirty()
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
		_markLayoutAsDirty();

		for(auto& elem : mChildren)
		{
			elem->enableRecursively();
		}
	}

	void GUIElementBase::disableRecursively()
	{
		_markMeshAsDirty(); // Just need to hide the mesh
		mIsDisabled = true;

		for(auto& elem : mChildren)
		{
			elem->disableRecursively();
		}
	}

	void GUIElementBase::_updateLayout(const GUILayoutData& data)
	{
		_updateOptimalLayoutSizes(); // We calculate optimal sizes of all layouts as a pre-processing step, as they are requested often during update
		_updateLayoutInternal(data);
	}

	void GUIElementBase::_updateOptimalLayoutSizes()
	{
		for(auto& child : mChildren)
		{
			child->_updateOptimalLayoutSizes();
		}
	}

	void GUIElementBase::_updateLayoutInternal(const GUILayoutData& data)
	{
		for(auto& child : mChildren)
		{
			child->_updateLayoutInternal(data);
		}
	}

	LayoutSizeRange GUIElementBase::_calculateLayoutSizeRange() const
	{
		if (mIsDisabled)
			return LayoutSizeRange();

		const GUIDimensions& dimensions = _getDimensions();
		return dimensions.calculateSizeRange(_getOptimalSize());
	}

	LayoutSizeRange GUIElementBase::_getLayoutSizeRange() const
	{
		return _calculateLayoutSizeRange();
	}

	void GUIElementBase::_getElementAreas(const Rect2I& layoutArea, Rect2I* elementAreas, UINT32 numElements,
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
		assert(!element->_isDestroyed());

		GUIElementBase* parentElement = element->_getParent();
		if(parentElement != nullptr)
		{
			parentElement->_unregisterChildElement(element);
		}

		element->_setParent(this);
		mChildren.push_back(element);

		if (mIsDisabled)
			element->disableRecursively();

		_markLayoutAsDirty();
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

				_markLayoutAsDirty();
				break;
			}
		}

		if(!foundElem)
			BS_EXCEPT(InvalidParametersException, "Provided element is not a part of this element.");
	}

	void GUIElementBase::_changeParentWidget(GUIWidget* widget)
	{
		assert(!_isDestroyed());

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

		_markLayoutAsDirty();
	}

	void GUIElementBase::_updateAUParents()
	{
		GUIElementBase* updateParent = nullptr;
		if (mParentElement != nullptr)
		{
			updateParent = mParentElement->findUpdateParent();

			// If parent is a panel then we can do an optimization and only update
			// one child instead of all of them, so change parent to that child.
			if (updateParent != nullptr && updateParent->_getType() == GUIElementBase::Type::Panel)
			{
				GUIElementBase* optimizedUpdateParent = this;
				while (optimizedUpdateParent->_getParent() != updateParent)
					optimizedUpdateParent = optimizedUpdateParent->_getParent();

				updateParent = optimizedUpdateParent;
			}
		}

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
		{
			GUIElementBase* childUpdateParent = updateParent;

			// If parent is a panel then we can do an optimization and only update
			// one child instead of all of them, so change parent to that child.
			if (childUpdateParent != nullptr && childUpdateParent->_getType() == GUIElementBase::Type::Panel)
			{
				GUIElementBase* optimizedUpdateParent = child;
				while (optimizedUpdateParent->_getParent() != childUpdateParent)
					optimizedUpdateParent = optimizedUpdateParent->_getParent();

				childUpdateParent = optimizedUpdateParent;
			}

			child->setUpdateParent(childUpdateParent);
		}
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
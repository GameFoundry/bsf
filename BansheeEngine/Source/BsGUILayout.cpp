#include "BsGUILayout.h"
#include "BsGUIElement.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUISpace.h"
#include "CmException.h"

namespace BansheeEngine
{
	GUILayout::GUILayout()
		:mOptimalWidth(0), mOptimalHeight(0), mActualWidth(0), mActualHeight(0)
	{

	}

	GUILayout::~GUILayout() 
	{

	}

	void GUILayout::addElement(GUIElement* element)
	{
		_registerChildElement(element);
	}

	void GUILayout::removeElement(GUIElement* element)
	{
		_unregisterChildElement(element);
	}

	void GUILayout::insertElement(UINT32 idx, GUIElement* element)
	{
		if(idx < 0 || idx > (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUIElementBase* parentElement = element->_getParent();
		if(parentElement != nullptr)
		{
			parentElement->_unregisterChildElement(element);
		}

		element->_setParent(this);
		mChildren.insert(mChildren.begin() + idx, element);
		
		markContentAsDirty();
	}

	void GUILayout::removeChildAt(UINT32 idx)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUIElementBase* child = mChildren[idx];
		mChildren.erase(mChildren.begin() + idx);

		if(child->_getType() == GUIElementBase::Type::Element)
			child->_setParent(nullptr);
		else
			cm_delete<PoolAlloc>(child);

		markContentAsDirty();
	}

	GUIFixedSpace& GUILayout::addSpace(UINT32 size)
	{
		GUIFixedSpace* entry = cm_new<GUIFixedSpace, PoolAlloc>(size);

		mChildren.push_back(entry);
		markContentAsDirty();

		return *entry;
	}

	void GUILayout::removeSpace(GUIFixedSpace& space)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUIElementBase* child = *iter;

			if(child->_getType() == GUIElementBase::Type::FixedSpace && child == &space)
			{
				cm_delete<PoolAlloc>(child);

				mChildren.erase(iter);
				foundElem = true;
				markContentAsDirty();
				break;
			}
		}

		if(!foundElem)
			CM_EXCEPT(InvalidParametersException, "Provided element is not a part of this layout.");
	}

	GUIFixedSpace& GUILayout::insertSpace(UINT32 idx, UINT32 size)
	{
		if(idx < 0 || idx > (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUIFixedSpace* entry = cm_new<GUIFixedSpace, PoolAlloc>(size);

		mChildren.insert(mChildren.begin() + idx, entry);
		markContentAsDirty();

		return *entry;
	}

	GUIFlexibleSpace& GUILayout::addFlexibleSpace()
	{
		GUIFlexibleSpace* entry = cm_new<GUIFlexibleSpace, PoolAlloc>();

		mChildren.push_back(entry);
		markContentAsDirty();

		return *entry;
	}

	void GUILayout::removeFlexibleSpace(GUIFlexibleSpace& space)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUIElementBase* child = *iter;

			if(child->_getType() == GUIElementBase::Type::FlexibleSpace && child == &space)
			{
				cm_delete<PoolAlloc>(child);

				mChildren.erase(iter);
				foundElem = true;
				markContentAsDirty();
				break;
			}
		}

		if(!foundElem)
			CM_EXCEPT(InvalidParametersException, "Provided element is not a part of this layout.");
	}

	GUIFlexibleSpace& GUILayout::insertFlexibleSpace(UINT32 idx)
	{
		if(idx < 0 || idx > (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUIFlexibleSpace* entry = cm_new<GUIFlexibleSpace, PoolAlloc>();

		mChildren.insert(mChildren.begin() + idx, entry);
		markContentAsDirty();

		return *entry;
	}

	const RectOffset& GUILayout::_getPadding() const
	{
		static RectOffset padding;

		return padding;
	}
}
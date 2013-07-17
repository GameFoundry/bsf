#include "BsGUILayout.h"
#include "BsGUIElement.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUISpace.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUILayout::GUILayout()
		:mOptimalWidth(0), mOptimalHeight(0)
	{

	}

	GUILayout::~GUILayout() 
	{
		for(auto& child : mChildren)
		{
			// Non-GUIElement are owned by us
			if(child->_getType() != GUIElementBase::Type::Element)
				cm_delete<PoolAlloc>(child);
			else
				child->_setParentLayout(nullptr);
		}
	}

	void GUILayout::addElement(GUIElement* element)
	{
		if(element->_getParentLayout() != nullptr)
			element->_getParentLayout()->removeElement(element);

		element->_setParentLayout(this);
		mChildren.push_back(element);

		markContentAsDirty();
	}

	void GUILayout::removeElement(GUIElement* element)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUIElementBase* child = *iter;

			if(child->_getType() == GUIElementBase::Type::Element && child == element)
			{
				mChildren.erase(iter);
				foundElem = true;
				
				markContentAsDirty();
				break;
			}
		}

		if(!foundElem)
			CM_EXCEPT(InvalidParametersException, "Provided element is not a part of this layout.");
	}

	void GUILayout::insertElement(UINT32 idx, GUIElement* element)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		if(element->_getParentLayout() != nullptr)
			element->_getParentLayout()->removeElement(element);

		element->_setParentLayout(this);
		mChildren.insert(mChildren.begin() + idx, element);
		
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
		if(idx < 0 || idx >= (UINT32)mChildren.size())
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
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUIFlexibleSpace* entry = cm_new<GUIFlexibleSpace, PoolAlloc>();

		mChildren.insert(mChildren.begin() + idx, entry);
		markContentAsDirty();

		return *entry;
	}

	UINT32 GUILayout::getNumChildren() const
	{
		return (UINT32)mChildren.size();
	}
}
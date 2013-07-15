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
			if(child.isLayout())
			{
				// Child layouts are directly owned by us
				cm_delete<PoolAlloc>(child.layout);
			}
			else if(child.isFixedSpace())
			{
				cm_delete<PoolAlloc>(child.space);
			}
			else if(child.isFlexibleSpace())
			{
				cm_delete<PoolAlloc>(child.flexibleSpace);
			}
			else
			{
				child.element->_setParentLayout(nullptr);
			}
		}
	}

	void GUILayout::addElement(GUIElement* element)
	{
		if(element->_getParentLayout() != nullptr)
			element->_getParentLayout()->removeElement(element);

		GUILayoutEntry entry;
		entry.setElement(element);

		element->_setParentLayout(this);
		mChildren.push_back(entry);

		markContentAsDirty();
	}

	void GUILayout::removeElement(GUIElement* element)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUILayoutEntry& child = *iter;

			if(child.isElement() && child.element == element)
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

		GUILayoutEntry entry;
		entry.setElement(element);

		element->_setParentLayout(this);
		mChildren.insert(mChildren.begin() + idx, entry);
		
		markContentAsDirty();
	}

	GUILayout& GUILayout::addLayoutX()
	{
		GUILayoutEntry entry;
		entry.setLayout(cm_new<GUILayoutX, PoolAlloc>());

		mChildren.push_back(entry);
		markContentAsDirty();

		return *entry.layout;
	}

	GUILayout& GUILayout::addLayoutY()
	{
		GUILayoutEntry entry;
		entry.setLayout(cm_new<GUILayoutY, PoolAlloc>());

		mChildren.push_back(entry);
		markContentAsDirty();

		return *entry.layout;
	}

	void GUILayout::removeLayout(GUILayout& layout)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUILayoutEntry& child = *iter;

			if(child.isLayout() && child.layout == &layout)
			{
				cm_delete<PoolAlloc>(child.layout);

				mChildren.erase(iter);
				foundElem = true;
				markContentAsDirty();
				break;
			}
		}

		if(!foundElem)
			CM_EXCEPT(InvalidParametersException, "Provided element is not a part of this layout.");
	}

	GUILayout& GUILayout::insertLayoutX(UINT32 idx)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUILayoutEntry entry;
		entry.setLayout(cm_new<GUILayoutX, PoolAlloc>());

		mChildren.insert(mChildren.begin() + idx, entry);
		markContentAsDirty();

		return *entry.layout;
	}

	GUILayout& GUILayout::insertLayoutY(UINT32 idx)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUILayoutEntry entry;
		entry.setLayout(cm_new<GUILayoutY, PoolAlloc>());;

		mChildren.insert(mChildren.begin() + idx, entry);
		markContentAsDirty();

		return *entry.layout;
	}

	GUIFixedSpace& GUILayout::addSpace(UINT32 size)
	{
		GUILayoutEntry entry;
		entry.setSpace(cm_new<GUIFixedSpace, PoolAlloc>(size));

		mChildren.push_back(entry);
		markContentAsDirty();

		return *entry.space;
	}

	void GUILayout::removeSpace(GUIFixedSpace& space)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUILayoutEntry& child = *iter;

			if(child.isFixedSpace() && child.space == &space)
			{
				cm_delete<PoolAlloc>(child.space);

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

		GUILayoutEntry entry;
		entry.setSpace(cm_new<GUIFixedSpace, PoolAlloc>(size));

		mChildren.insert(mChildren.begin() + idx, entry);
		markContentAsDirty();

		return *entry.space;
	}

	GUIFlexibleSpace& GUILayout::addFlexibleSpace()
	{
		GUILayoutEntry entry;
		entry.setFlexibleSpace(cm_new<GUIFlexibleSpace, PoolAlloc>());

		mChildren.push_back(entry);
		markContentAsDirty();

		return *entry.flexibleSpace;
	}

	void GUILayout::removeFlexibleSpace(GUIFlexibleSpace& space)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUILayoutEntry& child = *iter;

			if(child.isFlexibleSpace() && child.flexibleSpace == &space)
			{
				cm_delete<PoolAlloc>(child.flexibleSpace);

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

		GUILayoutEntry entry;
		entry.setFlexibleSpace(cm_new<GUIFlexibleSpace, PoolAlloc>());

		mChildren.insert(mChildren.begin() + idx, entry);
		markContentAsDirty();

		return *entry.flexibleSpace;
	}

	UINT32 GUILayout::getNumChildren() const
	{
		return (UINT32)mChildren.size();
	}
}
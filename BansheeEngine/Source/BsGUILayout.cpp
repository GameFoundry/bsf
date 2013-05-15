#include "BsGUILayout.h"
#include "BsGUIElement.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUILayout::GUILayout()
	{

	}

	GUILayout::~GUILayout() 
	{
		for(auto& child : mChildren)
		{
			if(child.isLayout)
			{
				// Child layouts are directly owned by us
				CM_DELETE(child.layout, GUILayout, PoolAlloc);
			}
			else
			{
				child.element->setParentLayout(nullptr);
			}
		}
	}

	void GUILayout::addElement(GUIElement* element)
	{
		if(element->getParentLayout() != nullptr)
			element->getParentLayout()->removeElement(element);

		GUILayoutEntry entry;
		entry.element = element;
		entry.isLayout = false;

		element->setParentLayout(this);
		mChildren.push_back(entry);
	}

	void GUILayout::removeElement(GUIElement* element)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUILayoutEntry& child = *iter;

			if(!child.isLayout && child.element == element)
			{
				mChildren.erase(iter);
				foundElem = true;
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

		if(element->getParentLayout() != nullptr)
			element->getParentLayout()->removeElement(element);

		GUILayoutEntry entry;
		entry.element = element;
		entry.isLayout = false;

		element->setParentLayout(this);
		mChildren.insert(mChildren.begin() + idx, entry);
	}

	GUILayout& GUILayout::addLayoutX()
	{
		GUILayoutEntry entry;
		entry.layout = CM_NEW(GUILayoutX, PoolAlloc) GUILayoutX();
		entry.isLayout = true;

		mChildren.push_back(entry);

		return *entry.layout;
	}

	GUILayout& GUILayout::addLayoutY()
	{
		GUILayoutEntry entry;
		entry.layout = CM_NEW(GUILayoutY, PoolAlloc) GUILayoutY();
		entry.isLayout = true;

		mChildren.push_back(entry);

		return *entry.layout;
	}

	void GUILayout::removeLayout(GUILayout& layout)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUILayoutEntry& child = *iter;

			if(child.isLayout && child.layout == &layout)
			{
				CM_DELETE(child.layout, GUILayout, PoolAlloc);

				mChildren.erase(iter);
				foundElem = true;
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
		entry.layout = CM_NEW(GUILayoutX, PoolAlloc) GUILayoutX();
		entry.isLayout = true;

		mChildren.insert(mChildren.begin() + idx, entry);

		return *entry.layout;
	}

	GUILayout& GUILayout::insertLayoutY(UINT32 idx)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUILayoutEntry entry;
		entry.layout = CM_NEW(GUILayoutY, PoolAlloc) GUILayoutY();
		entry.isLayout = true;

		mChildren.insert(mChildren.begin() + idx, entry);

		return *entry.layout;
	}

	UINT32 GUILayout::getNumChildren() const
	{
		return (UINT32)mChildren.size();
	}
}
#include "BsGUILayout.h"
#include "BsGUIElement.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUILayout::GUILayout()
		:mParentLayout(nullptr)
	{

	}

	GUILayout::~GUILayout() 
	{

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

	void GUILayout::addLayout(GUILayout* layout)
	{
		if(layout->getParentLayout() != nullptr)
			layout->getParentLayout()->removeLayout(layout);

		GUILayoutEntry entry;
		entry.layout = layout;
		entry.isLayout = true;

		layout->setParentLayout(this);
		mChildren.push_back(entry);
	}

	void GUILayout::removeLayout(GUILayout* layout)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUILayoutEntry& child = *iter;

			if(child.isLayout && child.layout == layout)
			{
				mChildren.erase(iter);
				foundElem = true;
				break;
			}
		}

		if(!foundElem)
			CM_EXCEPT(InvalidParametersException, "Provided element is not a part of this layout.");
	}

	void GUILayout::insertLayout(UINT32 idx, GUILayout* layout)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		if(layout->getParentLayout() != nullptr)
			layout->getParentLayout()->removeLayout(layout);

		GUILayoutEntry entry;
		entry.layout = layout;
		entry.isLayout = false;

		layout->setParentLayout(this);
		mChildren.insert(mChildren.begin() + idx, entry);
	}

	UINT32 GUILayout::getNumChildren() const
	{
		return (UINT32)mChildren.size();
	}
}
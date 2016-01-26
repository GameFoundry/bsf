#include "BsDockManagerLayout.h"
#include "BsDockManagerLayoutRTTI.h"

namespace BansheeEngine
{
	DockManagerLayout::Entry::Entry()
		:isLeaf(true), splitPosition(0), horizontalSplit(false),
		parent(nullptr)
	{
		children[0] = nullptr;
		children[1] = nullptr;
	}

	DockManagerLayout::Entry::~Entry()
	{ }

	DockManagerLayout::Entry* DockManagerLayout::Entry::createLeaf(Entry* parent, UINT32 childIdx, 
		const Vector<String>& widgetNames)
	{
		Entry* newEntry = bs_new<Entry>();
		newEntry->isLeaf = true;
		newEntry->parent = parent;

		if(parent != nullptr)
			parent->children[childIdx] = newEntry;

		newEntry->widgetNames = widgetNames;

		return newEntry;
	}

	DockManagerLayout::Entry* DockManagerLayout::Entry::createContainer(Entry* parent, UINT32 childIdx, 
		float splitPosition, bool horizontalSplit)
	{
		Entry* newEntry = bs_new<Entry>();
		newEntry->isLeaf = false;
		newEntry->parent = parent;

		if(parent != nullptr)
			parent->children[childIdx] = newEntry;

		newEntry->horizontalSplit = horizontalSplit;
		newEntry->splitPosition = splitPosition;

		return newEntry;
	}

	DockManagerLayout::~DockManagerLayout()
	{
		Stack<Entry*> todo;
		if(!mRootEntry.isLeaf)
		{
			todo.push(mRootEntry.children[0]);
			todo.push(mRootEntry.children[1]);
		}

		while(!todo.empty())
		{
			Entry* current = todo.top();
			todo.pop();

			if(!current->isLeaf)
			{
				todo.push(current->children[0]);
				todo.push(current->children[1]);
			}

			bs_delete(current);
		}
	}

	/************************************************************************/
	/* 								RTTI		                     		*/
	/************************************************************************/

	RTTITypeBase* DockManagerLayout::getRTTIStatic()
	{
		return DockManagerLayoutRTTI::instance();
	}

	RTTITypeBase* DockManagerLayout::getRTTI() const
	{
		return DockManagerLayout::getRTTIStatic();
	}
}
#include "BsDockManagerLayout.h"
#include "BsDockManagerLayoutRTTI.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	DockManagerLayout::Entry::Entry()
		:isLeaf(true), firstChildSize(0), horizontalSplit(false),
		parent(nullptr)
	{
		children[0] = nullptr;
		children[1] = nullptr;
	}

	DockManagerLayout::Entry::~Entry()
	{
		if(!isLeaf)
		{
			if(children[0] != nullptr)
				cm_delete(children[0]);

			if(children[1] != nullptr)
				cm_delete(children[1]);
		}
	}

	DockManagerLayout::Entry* DockManagerLayout::Entry::createLeaf(Entry* parent, CM::UINT32 childIdx, const CM::Vector<CM::String>::type& widgetNames)
	{
		Entry* newEntry = cm_new<Entry>();
		newEntry->isLeaf = true;
		newEntry->parent = parent;

		if(parent != nullptr)
			parent->children[childIdx] = newEntry;

		newEntry->widgetNames = widgetNames;

		return newEntry;
	}

	DockManagerLayout::Entry* DockManagerLayout::Entry::createContainer(Entry* parent, CM::UINT32 childIdx, CM::UINT32 firstChildSize, bool horizontalSplit)
	{
		Entry* newEntry = cm_new<Entry>();
		newEntry->isLeaf = false;
		newEntry->parent = parent;

		if(parent != nullptr)
			parent->children[childIdx] = newEntry;

		newEntry->horizontalSplit = horizontalSplit;
		newEntry->firstChildSize = firstChildSize;

		return newEntry;
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
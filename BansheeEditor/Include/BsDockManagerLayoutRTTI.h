#pragma once

#include "BsEditorPrerequisites.h"
#include "BsDockManagerLayout.h"
#include "CmRTTIType.h"

namespace BansheeEditor
{
	class DockManagerLayoutRTTI : public CM::RTTIType<DockManagerLayout, CM::IReflectable, DockManagerLayoutRTTI>
	{
	private:
		DockManagerLayout::Entry& getRootEntry(DockManagerLayout* obj) { return obj->mRootEntry; }
		void setRootEntry(DockManagerLayout* obj, DockManagerLayout::Entry& val) { obj->mRootEntry = val; } 

	public:
		DockManagerLayoutRTTI()
		{
			addPlainField("mRootEntry", 0, &DockManagerLayoutRTTI::getRootEntry, &DockManagerLayoutRTTI::setRootEntry);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "DockManagerLayout";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_DockManagerLayout;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<DockManagerLayout>();
		}
	};
}

namespace CamelotFramework
{
	template<> struct CM::RTTIPlainType<BansheeEditor::DockManagerLayout::Entry>
	{	
		enum { id = BansheeEditor::TID_DockManagerLayoutEntry }; enum { hasDynamicSize = 1 };

		static void toMemory(const BansheeEditor::DockManagerLayout::Entry& data, char* memory)
		{ 
			UINT32 size = 0;
			char* memoryStart = memory;
			memory += sizeof(UINT32);
			size += sizeof(UINT32);

			memory = rttiWriteElem(data.isLeaf, memory, size);
			memory = rttiWriteElem(data.horizontalSplit, memory, size);
			memory = rttiWriteElem(data.splitPosition, memory, size);
			memory = rttiWriteElem(data.widgetNames, memory, size);

			if(!data.isLeaf)
			{
				memory = rttiWriteElem(*data.children[0], memory, size);
				memory = rttiWriteElem(*data.children[1], memory, size);
			}

			memcpy(memoryStart, &size, sizeof(UINT32));
		}

		static UINT32 fromMemory(BansheeEditor::DockManagerLayout::Entry& data, char* memory)
		{ 
			UINT32 size = 0;
			memcpy(&size, memory, sizeof(UINT32));
			memory += sizeof(UINT32);

			memory = rttiReadElem(data.isLeaf, memory);
			memory = rttiReadElem(data.horizontalSplit, memory);
			memory = rttiReadElem(data.splitPosition, memory);
			memory = rttiReadElem(data.widgetNames, memory);

			if(!data.isLeaf)
			{
				data.children[0] = cm_new<BansheeEditor::DockManagerLayout::Entry>();
				data.children[1] = cm_new<BansheeEditor::DockManagerLayout::Entry>();

				memory = rttiReadElem(*data.children[0], memory);
				memory = rttiReadElem(*data.children[1], memory);
				
				data.children[0]->parent = &data;
				data.children[1]->parent = &data;
			}

			return size;
		}

		static UINT32 getDynamicSize(const BansheeEditor::DockManagerLayout::Entry& data)	
		{ 
			UINT64 dataSize = sizeof(UINT32) + rttiGetElemSize(data.isLeaf) + rttiGetElemSize(data.horizontalSplit) + 
				rttiGetElemSize(data.splitPosition) + rttiGetElemSize(data.widgetNames);

			if(!data.isLeaf)
			{
				dataSize += rttiGetElemSize(*data.children[0]);
				dataSize += rttiGetElemSize(*data.children[1]);
			}

#if CM_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				__string_throwDataOverflowException();
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 
}
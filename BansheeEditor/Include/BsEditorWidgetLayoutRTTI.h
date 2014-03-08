#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidgetLayout.h"
#include "BsDockManagerLayout.h"
#include "CmRTTIType.h"

namespace BansheeEditor
{
	class EditorWidgetLayoutRTTI : public CM::RTTIType<EditorWidgetLayout, CM::IReflectable, EditorWidgetLayoutRTTI>
	{
	private:
		EditorWidgetLayout::Entry& getEntry(EditorWidgetLayout* obj, CM::UINT32 idx) { return obj->mEntries[idx]; }
		void setEntry(EditorWidgetLayout* obj, CM::UINT32 idx, EditorWidgetLayout::Entry& val) { obj->mEntries[idx] = val; } 
		CM::UINT32 getEntriesArraySize(EditorWidgetLayout* obj) { return (CM::UINT32)obj->mEntries.size(); }
		void setEntriesArraySize(EditorWidgetLayout* obj, CM::UINT32 size) { obj->mEntries.resize(size); }

		DockManagerLayoutPtr getDockLayout(EditorWidgetLayout* obj) { return obj->mDockLayout; }
		void setDockLayout(EditorWidgetLayout* obj, DockManagerLayoutPtr val) { obj->mDockLayout = val; }

	public:
		EditorWidgetLayoutRTTI()
		{
			addPlainArrayField("mRootEntry", 0, &EditorWidgetLayoutRTTI::getEntry, &EditorWidgetLayoutRTTI::getEntriesArraySize, 
				&EditorWidgetLayoutRTTI::setEntry, &EditorWidgetLayoutRTTI::setEntriesArraySize);

			addReflectablePtrField("mDockLayout", 1, &EditorWidgetLayoutRTTI::getDockLayout, &EditorWidgetLayoutRTTI::setDockLayout);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "EditorWidgetLayout";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_EditorWidgetLayout;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<EditorWidgetLayout>(EditorWidgetLayout::PrivatelyConstruct());
		}
	};
}

namespace CamelotFramework
{
	template<> struct CM::RTTIPlainType<BansheeEditor::EditorWidgetLayout::Entry>
	{	
		enum { id = BansheeEditor::TID_EditorWidgetLayoutEntry }; enum { hasDynamicSize = 1 };

		static void toMemory(const BansheeEditor::EditorWidgetLayout::Entry& data, char* memory)
		{ 
			UINT32 size = 0;
			char* memoryStart = memory;
			memory += sizeof(UINT32);
			size += sizeof(UINT32);

			memory = rttiWriteElem(data.widgetNames, memory, size);
			memory = rttiWriteElem(data.isDocked, memory, size);
			memory = rttiWriteElem(data.x, memory, size);
			memory = rttiWriteElem(data.y, memory, size);
			memory = rttiWriteElem(data.width, memory, size);
			memory = rttiWriteElem(data.height, memory, size);

			memcpy(memoryStart, &size, sizeof(UINT32));
		}

		static UINT32 fromMemory(BansheeEditor::EditorWidgetLayout::Entry& data, char* memory)
		{ 
			UINT32 size = 0;
			memcpy(&size, memory, sizeof(UINT32));
			memory += sizeof(UINT32);

			memory = rttiReadElem(data.widgetNames, memory);
			memory = rttiReadElem(data.isDocked, memory);
			memory = rttiReadElem(data.x, memory);
			memory = rttiReadElem(data.y, memory);
			memory = rttiReadElem(data.width, memory);
			memory = rttiReadElem(data.height, memory);

			return size;
		}

		static UINT32 getDynamicSize(const BansheeEditor::EditorWidgetLayout::Entry& data)	
		{ 
			UINT64 dataSize = sizeof(UINT32) + rttiGetElemSize(data.widgetNames) + rttiGetElemSize(data.isDocked) + 
				rttiGetElemSize(data.x) + rttiGetElemSize(data.y) + rttiGetElemSize(data.width) + rttiGetElemSize(data.height);

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
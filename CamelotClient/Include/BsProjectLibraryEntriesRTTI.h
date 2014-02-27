#pragma once

#include "BsEditorPrerequisites.h"
#include "CmRTTIType.h"
#include "BsProjectLibraryEntries.h"

namespace BansheeEditor
{
	class ProjectLibraryEntriesRTTI : public CM::RTTIType<ProjectLibraryEntries, CM::IReflectable, ProjectLibraryEntriesRTTI>
	{
	private:
		ProjectLibrary::DirectoryEntry& getRootElement(ProjectLibraryEntries* obj) { return obj->mRootEntry; }
		void setRootElement(ProjectLibraryEntries* obj, ProjectLibrary::DirectoryEntry& val) { obj->mRootEntry = val; } 

	public:
		ProjectLibraryEntriesRTTI()
		{
			addPlainField("mRootElement", 0, &ProjectLibraryEntriesRTTI::getRootElement, &ProjectLibraryEntriesRTTI::setRootElement);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ProjectLibraryEntries";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_ProjectLibraryEntries;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return ProjectLibraryEntries::createEmpty();
		}
	};
}

namespace CamelotFramework
{
	template<> struct CM::RTTIPlainType<BansheeEditor::ProjectLibrary::ResourceEntry>
	{	
		enum { id = BansheeEditor::TID_ProjectLibraryResEntry }; enum { hasDynamicSize = 1 };

		static void toMemory(const BansheeEditor::ProjectLibrary::ResourceEntry& data, char* memory)
		{ 
			UINT32 size = 0;
			char* memoryStart = memory;
			memory += sizeof(UINT32);
			size += sizeof(UINT32);

			UINT32 type = (UINT32)data.type;
			memory = rttiWriteElem(type, memory, size);
			memory = rttiWriteElem(data.path, memory, size);
			memory = rttiWriteElem(data.elementName, memory, size);
			memory = rttiWriteElem(data.lastUpdateTime, memory, size);

			memcpy(memoryStart, &size, sizeof(UINT32));
		}

		static UINT32 fromMemory(BansheeEditor::ProjectLibrary::ResourceEntry& data, char* memory)
		{ 
			UINT32 size = 0;
			memcpy(&size, memory, sizeof(UINT32));
			memory += sizeof(UINT32);

			UINT32 type;
			memory = rttiReadElem(type, memory);
			data.type = (BansheeEditor::ProjectLibrary::LibraryEntryType)type;

			memory = rttiReadElem(data.path, memory);
			memory = rttiReadElem(data.elementName, memory);
			memory = rttiReadElem(data.lastUpdateTime, memory);

			return size;
		}

		static UINT32 getDynamicSize(const BansheeEditor::ProjectLibrary::ResourceEntry& data)	
		{ 
			UINT64 dataSize = sizeof(UINT32) + rttiGetElemSize(data.type) + rttiGetElemSize(data.path) + rttiGetElemSize(data.elementName) +
				rttiGetElemSize(data.lastUpdateTime);

#if CM_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				__string_throwDataOverflowException();
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 

	template<> struct CM::RTTIPlainType<BansheeEditor::ProjectLibrary::DirectoryEntry>
	{	
		enum { id = BansheeEditor::TID_ProjectLibraryDirEntry }; enum { hasDynamicSize = 1 };

		static void toMemory(const BansheeEditor::ProjectLibrary::DirectoryEntry& data, char* memory)
		{ 
			UINT32 size = 0;
			char* memoryStart = memory;
			memory += sizeof(UINT32);
			size += sizeof(UINT32);

			memory = rttiWriteElem(data.type, memory, size);
			memory = rttiWriteElem(data.path, memory, size);
			memory = rttiWriteElem(data.elementName, memory, size);

			UINT32 numChildren = (UINT32)data.mChildren.size();
			memory = rttiWriteElem(numChildren, memory, size);

			for(auto& child : data.mChildren)
			{
				if(child->type == BansheeEditor::ProjectLibrary::LibraryEntryType::File)
				{
					BansheeEditor::ProjectLibrary::ResourceEntry* childResEntry = static_cast<BansheeEditor::ProjectLibrary::ResourceEntry*>(child);
					memory = rttiWriteElem(*childResEntry, memory, size);
				}
				else if(child->type == BansheeEditor::ProjectLibrary::LibraryEntryType::Directory)
				{
					BansheeEditor::ProjectLibrary::DirectoryEntry* childDirEntry = static_cast<BansheeEditor::ProjectLibrary::DirectoryEntry*>(child);
					memory = rttiWriteElem(*childDirEntry, memory, size);
				}
			}

			memcpy(memoryStart, &size, sizeof(UINT32));
		}

		static UINT32 fromMemory(BansheeEditor::ProjectLibrary::DirectoryEntry& data, char* memory)
		{ 
			UINT32 size = 0;
			memcpy(&size, memory, sizeof(UINT32));
			memory += sizeof(UINT32);

			memory = rttiReadElem(data.type, memory);
			memory = rttiReadElem(data.path, memory);
			memory = rttiReadElem(data.elementName, memory);

			UINT32 numChildren = 0;
			memory = rttiReadElem(numChildren, memory);

			for(UINT32 i = 0; i < numChildren; i++)
			{
				BansheeEditor::ProjectLibrary::LibraryEntryType childType = BansheeEditor::ProjectLibrary::LibraryEntryType::File;
				rttiReadElem(childType, memory + sizeof(UINT32)); // Skip ahead to get type

				if(childType == BansheeEditor::ProjectLibrary::LibraryEntryType::File)
				{
					BansheeEditor::ProjectLibrary::ResourceEntry* childResEntry = cm_new<BansheeEditor::ProjectLibrary::ResourceEntry>(); // Note: Assumes that ProjectLibrary takes care of the cleanup
					memory = rttiReadElem(*childResEntry, memory);

					data.mChildren.push_back(childResEntry);
				}
				else if(childType == BansheeEditor::ProjectLibrary::LibraryEntryType::Directory)
				{
					BansheeEditor::ProjectLibrary::DirectoryEntry* childDirEntry = cm_new<BansheeEditor::ProjectLibrary::DirectoryEntry>(); // Note: Assumes that ProjectLibrary takes care of the cleanup
					memory = rttiReadElem(*childDirEntry, memory);

					data.mChildren.push_back(childDirEntry);
				}
			}

			return size;
		}

		static UINT32 getDynamicSize(const BansheeEditor::ProjectLibrary::DirectoryEntry& data)	
		{ 
			UINT64 dataSize = sizeof(UINT32) + rttiGetElemSize(data.type) + rttiGetElemSize(data.path) + rttiGetElemSize(data.elementName);

			dataSize += rttiGetElemSize((UINT32)data.mChildren.size());

			for(auto& child : data.mChildren)
			{
				if(child->type == BansheeEditor::ProjectLibrary::LibraryEntryType::File)
				{
					BansheeEditor::ProjectLibrary::ResourceEntry* childResEntry = static_cast<BansheeEditor::ProjectLibrary::ResourceEntry*>(child);
					dataSize += rttiGetElemSize(*childResEntry);
				}
				else if(child->type == BansheeEditor::ProjectLibrary::LibraryEntryType::Directory)
				{
					BansheeEditor::ProjectLibrary::DirectoryEntry* childDirEntry = static_cast<BansheeEditor::ProjectLibrary::DirectoryEntry*>(child);
					dataSize += rttiGetElemSize(*childDirEntry);
				}
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
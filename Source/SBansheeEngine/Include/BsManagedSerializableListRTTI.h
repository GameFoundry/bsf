//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsManagedSerializableList.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableListRTTI : public RTTIType<ManagedSerializableList, IReflectable, ManagedSerializableListRTTI>
	{
	private:
		ManagedSerializableTypeInfoListPtr getTypeInfo(ManagedSerializableList* obj)
		{
			return obj->mListTypeInfo;
		}

		void setTypeInfo(ManagedSerializableList* obj, ManagedSerializableTypeInfoListPtr val)
		{
			obj->mListTypeInfo = val;
		}

		UINT32& getNumElements(ManagedSerializableList* obj)
		{
			return (UINT32&)obj->mNumElements;
		}

		void setNumElements(ManagedSerializableList* obj, UINT32& numElements)
		{
			obj->mNumElements = numElements;
		}

		ManagedSerializableFieldDataPtr getListEntry(ManagedSerializableList* obj, UINT32 arrayIdx)
		{
			return obj->getFieldData(arrayIdx);
		}

		void setListEntry(ManagedSerializableList* obj, UINT32 arrayIdx, ManagedSerializableFieldDataPtr val)
		{
			obj->setFieldData(arrayIdx, val);
		}

		UINT32 getNumListEntries(ManagedSerializableList* obj)
		{
			return (UINT32)obj->mNumElements;
		}

		void setNumListEntries(ManagedSerializableList* obj, UINT32 numEntries)
		{
			obj->mCachedEntries = Vector<ManagedSerializableFieldDataPtr>(numEntries);
		}

	public:
		ManagedSerializableListRTTI()
		{
			addReflectablePtrField("mListTypeInfo", 0, &ManagedSerializableListRTTI::getTypeInfo, &ManagedSerializableListRTTI::setTypeInfo);
			addPlainField("mNumElements", 1, &ManagedSerializableListRTTI::getNumElements, &ManagedSerializableListRTTI::setNumElements);
			addReflectablePtrArrayField("mListEntries", 2, &ManagedSerializableListRTTI::getListEntry, &ManagedSerializableListRTTI::getNumListEntries, 
				&ManagedSerializableListRTTI::setListEntry, &ManagedSerializableListRTTI::setNumListEntries);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableList";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ScriptSerializableList;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ManagedSerializableList::createEmpty();
		}
	};
}
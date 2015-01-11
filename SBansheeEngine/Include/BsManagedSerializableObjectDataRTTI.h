#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsManagedSerializableObjectData.h"
#include "BsManagedSerializableField.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableObjectDataRTTI : public RTTIType <ManagedSerializableObjectData, IReflectable, ManagedSerializableObjectDataRTTI>
	{
	private:
		ManagedSerializableFieldDataEntryPtr getFieldEntry(ManagedSerializableObjectData* obj, UINT32 arrayIdx)
		{
			return obj->mFieldData[arrayIdx];
		}

		void setFieldsEntry(ManagedSerializableObjectData* obj, UINT32 arrayIdx, ManagedSerializableFieldDataEntryPtr val)
		{
			obj->mFieldData[arrayIdx] = val;
		}

		UINT32 getNumFieldEntries(ManagedSerializableObjectData* obj)
		{
			return (UINT32)obj->mFieldData.size();
		}

		void setNumFieldEntries(ManagedSerializableObjectData* obj, UINT32 numEntries)
		{
			obj->mFieldData = Vector<ManagedSerializableFieldDataEntryPtr>(numEntries);
		}

	public:
		ManagedSerializableObjectDataRTTI()
		{
			addReflectablePtrArrayField("mFieldEntries", 0, &ManagedSerializableObjectDataRTTI::getFieldEntry, &ManagedSerializableObjectDataRTTI::getNumFieldEntries,
				&ManagedSerializableObjectDataRTTI::setFieldsEntry, &ManagedSerializableObjectDataRTTI::setNumFieldEntries);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableObjectData";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ScriptSerializableObjectData;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<ManagedSerializableObjectData>();;
		}
	};
}
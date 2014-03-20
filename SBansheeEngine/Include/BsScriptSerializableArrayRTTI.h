#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "BsScriptSerializableArray.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableArrayRTTI : public CM::RTTIType<ScriptSerializableArray, CM::IReflectable, ScriptSerializableArrayRTTI>
	{
	private:
		ScriptSerializableTypeInfoArrayPtr getTypeInfo(ScriptSerializableArray* obj)
		{
			return obj->mArrayTypeInfo;
		}

		void setTypeInfo(ScriptSerializableArray* obj, ScriptSerializableTypeInfoArrayPtr val)
		{
			obj->mArrayTypeInfo = val;
		}

		ScriptSerializableFieldDataPtr getArrayEntry(ScriptSerializableArray* obj, CM::UINT32 arrayIdx)
		{
			return obj->mArrayEntries[arrayIdx];
		}

		void setArrayEntry(ScriptSerializableArray* obj, CM::UINT32 arrayIdx, ScriptSerializableFieldDataPtr val)
		{
			obj->mArrayEntries[arrayIdx] = val;
		}

		CM::UINT32 getNumArrayEntries(ScriptSerializableArray* obj)
		{
			return (CM::UINT32)obj->mArrayEntries.size();
		}

		void setNumArrayEntries(ScriptSerializableArray* obj, CM::UINT32 numEntries)
		{
			obj->mArrayEntries.resize(numEntries);
		}

	public:
		ScriptSerializableArrayRTTI()
		{
			addReflectablePtrField("mArrayTypeInfo", 0, &ScriptSerializableArrayRTTI::getTypeInfo, &ScriptSerializableArrayRTTI::setTypeInfo);
			addReflectablePtrArrayField("mArrayEntries", 1, &ScriptSerializableArrayRTTI::getArrayEntry, &ScriptSerializableArrayRTTI::getNumArrayEntries, 
				&ScriptSerializableArrayRTTI::setArrayEntry, &ScriptSerializableArrayRTTI::setNumArrayEntries);
		}

		virtual void onDeserializationEnded(CM::IReflectable* obj)
		{
			ScriptSerializableArray* serializableObject = static_cast<ScriptSerializableArray*>(obj);

			// TODO
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableArray";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_ScriptSerializableArray;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return ScriptSerializableArray::createEmpty();
		}
	};
}
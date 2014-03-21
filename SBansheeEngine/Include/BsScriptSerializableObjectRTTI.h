#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptSerializableObject.h"
#include "BsScriptSerializableField.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableObjectRTTI : public CM::RTTIType<ScriptSerializableObject, CM::IReflectable, ScriptSerializableObjectRTTI>
	{
	private:
		ScriptSerializableObjectInfoPtr getInfo(ScriptSerializableObject* obj)
		{
			return obj->mObjInfo;
		}

		void setInfo(ScriptSerializableObject* obj, ScriptSerializableObjectInfoPtr val)
		{
			obj->mObjInfo = val;
		}

		ScriptSerializableFieldDataEntryPtr getFieldEntry(ScriptSerializableObject* obj, CM::UINT32 arrayIdx)
		{
			return obj->mFieldEntries[arrayIdx];
		}

		void setFieldsEntry(ScriptSerializableObject* obj, CM::UINT32 arrayIdx, ScriptSerializableFieldDataEntryPtr val)
		{
			obj->mFieldEntries[arrayIdx] = val;
		}

		CM::UINT32 getNumFieldEntries(ScriptSerializableObject* obj)
		{
			return (CM::UINT32)obj->mFieldEntries.size();
		}

		void setNumFieldEntries(ScriptSerializableObject* obj, CM::UINT32 numEntries)
		{
			obj->mFieldEntries.resize(numEntries);
		}

	public:
		ScriptSerializableObjectRTTI()
		{
			addReflectablePtrField("mObjInfo", 0, &ScriptSerializableObjectRTTI::getInfo, &ScriptSerializableObjectRTTI::setInfo);
			addReflectablePtrArrayField("mFieldEntries", 1, &ScriptSerializableObjectRTTI::getFieldEntry, &ScriptSerializableObjectRTTI::getNumFieldEntries, 
				&ScriptSerializableObjectRTTI::setFieldsEntry, &ScriptSerializableObjectRTTI::setNumFieldEntries);
		}

		virtual void onDeserializationEnded(CM::IReflectable* obj)
		{
			ScriptSerializableObject* serializableObject = static_cast<ScriptSerializableObject*>(obj);
			
			ScriptSerializableObjectInfoPtr storedObjInfo = serializableObject->getObjectInfo();
			ScriptSerializableObjectInfoPtr currentObjInfo = nullptr;

			// See if this type even still exists
			if(!RuntimeScriptObjects::instance().getSerializableObjectInfo(storedObjInfo->mTypeInfo->mTypeNamespace, storedObjInfo->mTypeInfo->mTypeName, currentObjInfo))
				return;

			serializableObject->mManagedInstance = currentObjInfo->mMonoClass->createInstance();

			auto findFieldInfoFromKey = [&] (CM::UINT16 typeId, CM::UINT16 fieldId, ScriptSerializableObjectInfoPtr objInfo, 
				ScriptSerializableFieldInfoPtr& outFieldInfo, ScriptSerializableObjectInfoPtr &outObjInfo) -> bool
			{
				while(objInfo != nullptr)
				{
					if(objInfo->mTypeId == typeId)
					{
						auto iterFind = objInfo->mFields.find(fieldId);
						if(iterFind != objInfo->mFields.end())
						{
							outFieldInfo = iterFind->second;
							outObjInfo = objInfo;

							return true;
						}

						return false;
					}

					objInfo = objInfo->mBaseClass;
				}

				return false;
			};

			auto findTypeNameMatchingFieldInfo = [&] (const ScriptSerializableFieldInfoPtr& fieldInfo, const ScriptSerializableObjectInfoPtr& fieldObjInfo, 
				ScriptSerializableObjectInfoPtr objInfo) -> ScriptSerializableFieldInfoPtr
			{
				while(objInfo != nullptr)
				{
					if(objInfo->mTypeInfo->matches(fieldObjInfo->mTypeInfo))
					{
						auto iterFind = objInfo->mFieldNameToId.find(fieldInfo->mName);
						if(iterFind != objInfo->mFieldNameToId.end())
						{
							auto iterFind2 = objInfo->mFields.find(iterFind->second);
							if(iterFind2 != objInfo->mFields.end())
							{
								ScriptSerializableFieldInfoPtr foundField = iterFind2->second;
								if(foundField->isSerializable())
								{
									if(fieldInfo->mTypeInfo->matches(foundField->mTypeInfo))
										return foundField;
								}
							}
						}

						return nullptr;
					}

					objInfo = objInfo->mBaseClass;
				}

				return nullptr;
			};

			// Scan all fields and ensure the fields still exist
			for(auto& fieldEntry : serializableObject->mFieldEntries)
			{
				ScriptSerializableFieldInfoPtr storedFieldEntry;
				ScriptSerializableObjectInfoPtr storedFieldObjEntry;
				
				if(!findFieldInfoFromKey(fieldEntry->mKey->mTypeId, fieldEntry->mKey->mFieldId, storedObjInfo, storedFieldEntry, storedFieldObjEntry))
					continue;

				ScriptSerializableFieldInfoPtr matchingFieldInfo = findTypeNameMatchingFieldInfo(storedFieldEntry, storedFieldObjEntry, currentObjInfo);
				if(matchingFieldInfo != nullptr)
					serializableObject->setFieldData(matchingFieldInfo, fieldEntry->mValue);
			}
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableObject";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_ScriptSerializableObject;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return ScriptSerializableObject::createEmpty();
		}
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "BsScriptSerializableObjectInfo.h"
#include "CmException.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableAssemblyInfoRTTI : public CM::RTTIType<ScriptSerializableAssemblyInfo, CM::IReflectable, ScriptSerializableAssemblyInfoRTTI>
	{
	private:
		CM::String& getName(ScriptSerializableAssemblyInfo* obj)
		{
			return obj->mName;
		}

		void setName(ScriptSerializableAssemblyInfo* obj, CM::String& val)
		{
			obj->mName = val;
		}

		ScriptSerializableObjectInfoPtr getSerializableObjectInfo(ScriptSerializableAssemblyInfo* obj, CM::UINT32 idx) 
		{ 
			auto iter = obj->mObjectInfos.begin();
			for(CM::UINT32 i = 0; i < idx; i++)
				iter++;

			return iter->second;
		}

		void setSerializableObjectInfo(ScriptSerializableAssemblyInfo* obj, UINT32 idx, ScriptSerializableObjectInfoPtr val) 
		{ 
			obj->mTypeNameToId[val->getFullTypeName()] = val->mTypeId;
			obj->mObjectInfos[val->mTypeId] = val;
		}
		
		UINT32 getSerializableObjectInfoArraySize(ScriptSerializableAssemblyInfo* obj) { return (UINT32)obj->mObjectInfos.size(); }
		void setSerializableObjectInfoArraySize(ScriptSerializableAssemblyInfo* obj, UINT32 size) {  }

	public:
		ScriptSerializableAssemblyInfoRTTI()
		{
			addPlainField("mName", 0, &ScriptSerializableAssemblyInfoRTTI::getName, &ScriptSerializableAssemblyInfoRTTI::setName);
			addReflectablePtrArrayField("mObjectInfos", 1, &ScriptSerializableAssemblyInfoRTTI::getSerializableObjectInfo, 
				&ScriptSerializableAssemblyInfoRTTI::getSerializableObjectInfoArraySize, &ScriptSerializableAssemblyInfoRTTI::setSerializableObjectInfo, 
				&ScriptSerializableAssemblyInfoRTTI::setSerializableObjectInfoArraySize);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableAssemblyInfo";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableAssemblyInfo;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableAssemblyInfo>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableObjectInfoRTTI : public CM::RTTIType<ScriptSerializableObjectInfo, CM::IReflectable, ScriptSerializableObjectInfoRTTI>
	{
	private:
		CM::String& getNamespace(ScriptSerializableObjectInfo* obj)
		{
			return obj->mNamespace;
		}

		void setNamespace(ScriptSerializableObjectInfo* obj, CM::String& val)
		{
			obj->mNamespace = val;
		}

		CM::String& getTypename(ScriptSerializableObjectInfo* obj)
		{
			return obj->mTypeName;
		}

		void setTypename(ScriptSerializableObjectInfo* obj, CM::String& val)
		{
			obj->mTypeName = val;
		}

		CM::UINT32& getTypeId(ScriptSerializableObjectInfo* obj)
		{
			return obj->mTypeId;
		}

		void setTypeId(ScriptSerializableObjectInfo* obj, CM::UINT32& val)
		{
			obj->mTypeId = val;
		}

		ScriptSerializableObjectInfoPtr getBaseClass(ScriptSerializableObjectInfo* obj)
		{
			return obj->mBaseClass.lock();
		}

		void setBaseClass(ScriptSerializableObjectInfo* obj, ScriptSerializableObjectInfoPtr val)
		{
			obj->mBaseClass = val;
		}

		ScriptSerializableFieldInfoPtr getSerializableFieldInfo(ScriptSerializableObjectInfo* obj, CM::UINT32 idx) 
		{ 
			auto iter = obj->mFields.begin();
			for(CM::UINT32 i = 0; i < idx; i++)
				iter++;

			return iter->second;
		}

		void setSerializableFieldInfo(ScriptSerializableObjectInfo* obj, UINT32 idx, ScriptSerializableFieldInfoPtr val) 
		{ 
			obj->mFieldNameToId[val->mName] = val->mFieldId;
			obj->mFields[val->mFieldId] = val;
		}

		UINT32 getSerializableFieldInfoArraySize(ScriptSerializableObjectInfo* obj) { return (UINT32)obj->mFields.size(); }
		void setSerializableFieldInfoArraySize(ScriptSerializableObjectInfo* obj, UINT32 size) {  }

	public:
		ScriptSerializableObjectInfoRTTI()
		{
			addPlainField("mNamespace", 0, &ScriptSerializableObjectInfoRTTI::getNamespace, &ScriptSerializableObjectInfoRTTI::setNamespace);
			addPlainField("mTypename", 1, &ScriptSerializableObjectInfoRTTI::getTypename, &ScriptSerializableObjectInfoRTTI::setTypename);
			addPlainField("mTypeId", 2, &ScriptSerializableObjectInfoRTTI::getTypeId, &ScriptSerializableObjectInfoRTTI::setTypeId);
			addReflectablePtrField("mBaseClass", 3, &ScriptSerializableObjectInfoRTTI::getBaseClass, &ScriptSerializableObjectInfoRTTI::setBaseClass);

			addReflectablePtrArrayField("mFields", 4, &ScriptSerializableObjectInfoRTTI::getSerializableFieldInfo, 
				&ScriptSerializableObjectInfoRTTI::getSerializableFieldInfoArraySize, &ScriptSerializableObjectInfoRTTI::setSerializableFieldInfo, 
				&ScriptSerializableObjectInfoRTTI::setSerializableFieldInfoArraySize);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableObjectInfo";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableObjectInfo;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableObjectInfo>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldInfoRTTI : public CM::RTTIType<ScriptSerializableFieldInfo, CM::IReflectable, ScriptSerializableFieldInfoRTTI>
	{
	private:
		CM::String& getTypeNamespace(ScriptSerializableFieldInfo* obj)
		{
			return obj->mTypeNamespace;
		}

		void setTypeNamespace(ScriptSerializableFieldInfo* obj, CM::String& val)
		{
			obj->mTypeNamespace = val;
		}

		CM::String& getTypeName(ScriptSerializableFieldInfo* obj)
		{
			return obj->mTypeName;
		}

		void setTypeName(ScriptSerializableFieldInfo* obj, CM::String& val)
		{
			obj->mTypeName = val;
		}

		CM::String& getName(ScriptSerializableFieldInfo* obj)
		{
			return obj->mName;
		}

		void setName(ScriptSerializableFieldInfo* obj, CM::String& val)
		{
			obj->mName = val;
		}

		CM::UINT32& getFieldId(ScriptSerializableFieldInfo* obj)
		{
			return obj->mFieldId;
		}

		void setFieldId(ScriptSerializableFieldInfo* obj, CM::UINT32& val)
		{
			obj->mFieldId = val;
		}

		CM::UINT32& getFlags(ScriptSerializableFieldInfo* obj)
		{
			CM::UINT32 flags = (CM::UINT32)obj->mFlags;

			return flags;
		}

		void setFlags(ScriptSerializableFieldInfo* obj, CM::UINT32& val)
		{
			obj->mFlags = (ScriptFieldFlags)val;
		}

		CM::UINT32& getType(ScriptSerializableFieldInfo* obj)
		{
			CM::UINT32 flags = (CM::UINT32)obj->mType;

			return flags;
		}

		void setType(ScriptSerializableFieldInfo* obj, CM::UINT32& val)
		{
			obj->mType = (ScriptFieldType)val;
		}

	public:
		ScriptSerializableFieldInfoRTTI()
		{
			addPlainField("mName", 0, &ScriptSerializableFieldInfoRTTI::getName, &ScriptSerializableFieldInfoRTTI::setName);
			addPlainField("mTypeNamespace", 1, &ScriptSerializableFieldInfoRTTI::getTypeNamespace, &ScriptSerializableFieldInfoRTTI::setTypeNamespace);
			addPlainField("mTypeName", 2, &ScriptSerializableFieldInfoRTTI::getTypeName, &ScriptSerializableFieldInfoRTTI::setTypeName);
			addPlainField("mFieldId", 3, &ScriptSerializableFieldInfoRTTI::getFieldId, &ScriptSerializableFieldInfoRTTI::setFieldId);
			addPlainField("mFlags", 4, &ScriptSerializableFieldInfoRTTI::getFlags, &ScriptSerializableFieldInfoRTTI::setFlags);
			addPlainField("mType", 5, &ScriptSerializableFieldInfoRTTI::getType, &ScriptSerializableFieldInfoRTTI::setType);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableFieldInfo";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldInfo;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			CM_EXCEPT(CM::InvalidStateException, "Cannot instantiate an abstract class.");
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldInfoPlainRTTI : public CM::RTTIType<ScriptSerializableFieldInfoPlain, ScriptSerializableFieldInfo, ScriptSerializableFieldInfoPlainRTTI>
	{
	private:

	public:
		ScriptSerializableFieldInfoPlainRTTI()
		{

		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableFieldInfoPlain";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldInfoPlain;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldInfoPlain>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldInfoArrayRTTI : public CM::RTTIType<ScriptSerializableFieldInfoArray, ScriptSerializableFieldInfo, ScriptSerializableFieldInfoArrayRTTI>
	{
	private:

	public:
		ScriptSerializableFieldInfoArrayRTTI()
		{

		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableFieldInfoArray";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldInfoArray;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldInfoArray>();
		}
	};
}
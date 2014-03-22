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

		void setSerializableObjectInfo(ScriptSerializableAssemblyInfo* obj, CM::UINT32 idx, ScriptSerializableObjectInfoPtr val) 
		{ 
			obj->mTypeNameToId[val->getFullTypeName()] = val->mTypeId;
			obj->mObjectInfos[val->mTypeId] = val;
		}
		
		CM::UINT32 getSerializableObjectInfoArraySize(ScriptSerializableAssemblyInfo* obj) { return (CM::UINT32)obj->mObjectInfos.size(); }
		void setSerializableObjectInfoArraySize(ScriptSerializableAssemblyInfo* obj, CM::UINT32 size) {  }

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
			return CM::cm_shared_ptr<ScriptSerializableAssemblyInfo>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableObjectInfoRTTI : public CM::RTTIType<ScriptSerializableObjectInfo, CM::IReflectable, ScriptSerializableObjectInfoRTTI>
	{
	private:
		ScriptSerializableTypeInfoObjectPtr getTypeInfo(ScriptSerializableObjectInfo* obj)
		{
			return obj->mTypeInfo;
		}

		void setTypeInfo(ScriptSerializableObjectInfo* obj, ScriptSerializableTypeInfoObjectPtr val)
		{
			obj->mTypeInfo = val;
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
			return obj->mBaseClass;
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

		void setSerializableFieldInfo(ScriptSerializableObjectInfo* obj, CM::UINT32 idx, ScriptSerializableFieldInfoPtr val) 
		{ 
			obj->mFieldNameToId[val->mName] = val->mFieldId;
			obj->mFields[val->mFieldId] = val;
		}

		CM::UINT32 getSerializableFieldInfoArraySize(ScriptSerializableObjectInfo* obj) { return (CM::UINT32)obj->mFields.size(); }
		void setSerializableFieldInfoArraySize(ScriptSerializableObjectInfo* obj, CM::UINT32 size) {  }

	public:
		ScriptSerializableObjectInfoRTTI()
		{
			addReflectablePtrField("mTypeInfo", 0, &ScriptSerializableObjectInfoRTTI::getTypeInfo, &ScriptSerializableObjectInfoRTTI::setTypeInfo);
			addPlainField("mTypeId", 1, &ScriptSerializableObjectInfoRTTI::getTypeId, &ScriptSerializableObjectInfoRTTI::setTypeId);
			addReflectablePtrField("mBaseClass", 2, &ScriptSerializableObjectInfoRTTI::getBaseClass, &ScriptSerializableObjectInfoRTTI::setBaseClass);

			addReflectablePtrArrayField("mFields", 3, &ScriptSerializableObjectInfoRTTI::getSerializableFieldInfo, 
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
			return CM::cm_shared_ptr<ScriptSerializableObjectInfo>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldInfoRTTI : public CM::RTTIType<ScriptSerializableFieldInfo, CM::IReflectable, ScriptSerializableFieldInfoRTTI>
	{
	private:
		ScriptSerializableTypeInfoPtr getTypeInfo(ScriptSerializableFieldInfo* obj)
		{
			return obj->mTypeInfo;
		}

		void setTypeInfo(ScriptSerializableFieldInfo* obj, ScriptSerializableTypeInfoPtr val)
		{
			obj->mTypeInfo = val;
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
			return (CM::UINT32&)obj->mFlags;
		}

		void setFlags(ScriptSerializableFieldInfo* obj, CM::UINT32& val)
		{
			obj->mFlags = (ScriptFieldFlags)val;
		}

	public:
		ScriptSerializableFieldInfoRTTI()
		{
			addPlainField("mName", 0, &ScriptSerializableFieldInfoRTTI::getName, &ScriptSerializableFieldInfoRTTI::setName);
			addReflectablePtrField("mTypeInfo", 1, &ScriptSerializableFieldInfoRTTI::getTypeInfo, &ScriptSerializableFieldInfoRTTI::setTypeInfo);
			addPlainField("mFieldId", 2, &ScriptSerializableFieldInfoRTTI::getFieldId, &ScriptSerializableFieldInfoRTTI::setFieldId);
			addPlainField("mFlags", 3, &ScriptSerializableFieldInfoRTTI::getFlags, &ScriptSerializableFieldInfoRTTI::setFlags);
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
		{return CM::cm_shared_ptr<ScriptSerializableFieldInfo>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoRTTI : public CM::RTTIType<ScriptSerializableTypeInfo, CM::IReflectable, ScriptSerializableTypeInfoRTTI>
	{
	private:

	public:
		ScriptSerializableTypeInfoRTTI()
		{

		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableTypeInfo";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableTypeInfo;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			CM_EXCEPT(CM::InvalidStateException, "Cannot instantiate an abstract class");
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoPrimitiveRTTI : public CM::RTTIType<ScriptSerializableTypeInfoPrimitive, ScriptSerializableTypeInfo, ScriptSerializableTypeInfoPrimitiveRTTI>
	{
	private:
		ScriptPrimitiveType& getType(ScriptSerializableTypeInfoPrimitive* obj)
		{
			return obj->mType;
		}

		void setType(ScriptSerializableTypeInfoPrimitive* obj, ScriptPrimitiveType& val)
		{
			obj->mType = val;
		}

	public:
		ScriptSerializableTypeInfoPrimitiveRTTI()
		{
			addPlainField("mType", 0, &ScriptSerializableTypeInfoPrimitiveRTTI::getType, &ScriptSerializableTypeInfoPrimitiveRTTI::setType);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableTypeInfoPrimitive";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableTypeInfoPrimitive;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoObjectRTTI : public CM::RTTIType<ScriptSerializableTypeInfoObject, ScriptSerializableTypeInfo, ScriptSerializableTypeInfoObjectRTTI>
	{
	private:
		CM::String& getTypeNamespace(ScriptSerializableTypeInfoObject* obj)
		{
			return obj->mTypeNamespace;
		}

		void setTypeNamespace(ScriptSerializableTypeInfoObject* obj, CM::String& val)
		{
			obj->mTypeNamespace = val;
		}

		CM::String& getTypeName(ScriptSerializableTypeInfoObject* obj)
		{
			return obj->mTypeName;
		}

		void setTypeName(ScriptSerializableTypeInfoObject* obj, CM::String& val)
		{
			obj->mTypeName = val;
		}

		bool& getIsValueType(ScriptSerializableTypeInfoObject* obj)
		{
			return obj->mValueType;
		}

		void setIsValueType(ScriptSerializableTypeInfoObject* obj, bool& val)
		{
			obj->mValueType = val;
		}

	public:
		ScriptSerializableTypeInfoObjectRTTI()
		{
			addPlainField("mTypeName", 0, &ScriptSerializableTypeInfoObjectRTTI::getTypeName, &ScriptSerializableTypeInfoObjectRTTI::setTypeName);
			addPlainField("mTypeNamespace", 1, &ScriptSerializableTypeInfoObjectRTTI::getTypeNamespace, &ScriptSerializableTypeInfoObjectRTTI::setTypeNamespace);
			addPlainField("mValueType", 2, &ScriptSerializableTypeInfoObjectRTTI::getIsValueType, &ScriptSerializableTypeInfoObjectRTTI::setIsValueType);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableTypeInfoObject";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableTypeInfoObject;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableTypeInfoObject>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoArrayRTTI : public CM::RTTIType<ScriptSerializableTypeInfoArray, ScriptSerializableTypeInfo, ScriptSerializableTypeInfoArrayRTTI>
	{
	private:
		ScriptSerializableTypeInfoPtr getElementType(ScriptSerializableTypeInfoArray* obj)
		{
			return obj->mElementType;
		}

		void setElementType(ScriptSerializableTypeInfoArray* obj, ScriptSerializableTypeInfoPtr val)
		{
			obj->mElementType = val;
		}

		CM::UINT32& getRank(ScriptSerializableTypeInfoArray* obj)
		{
			return obj->mRank;
		}

		void setRank(ScriptSerializableTypeInfoArray* obj, CM::UINT32& val)
		{
			obj->mRank = val;
		}

	public:
		ScriptSerializableTypeInfoArrayRTTI()
		{
			addReflectablePtrField("mElementType", 0, &ScriptSerializableTypeInfoArrayRTTI::getElementType, &ScriptSerializableTypeInfoArrayRTTI::setElementType);
			addPlainField("mRank", 1, &ScriptSerializableTypeInfoArrayRTTI::getRank, &ScriptSerializableTypeInfoArrayRTTI::setRank);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableTypeInfoArray";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableTypeInfoArray;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableTypeInfoArray>();
		}
	};
}
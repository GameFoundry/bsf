#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "BsScriptSerializableObjectInfo.h"
#include "CmException.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableAssemblyInfoRTTI : public RTTIType<ScriptSerializableAssemblyInfo, IReflectable, ScriptSerializableAssemblyInfoRTTI>
	{
	private:
		String& getName(ScriptSerializableAssemblyInfo* obj)
		{
			return obj->mName;
		}

		void setName(ScriptSerializableAssemblyInfo* obj, String& val)
		{
			obj->mName = val;
		}

		ScriptSerializableObjectInfoPtr getSerializableObjectInfo(ScriptSerializableAssemblyInfo* obj, UINT32 idx) 
		{ 
			auto iter = obj->mObjectInfos.begin();
			for(UINT32 i = 0; i < idx; i++)
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

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableAssemblyInfo";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableAssemblyInfo;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableAssemblyInfo>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableObjectInfoRTTI : public RTTIType<ScriptSerializableObjectInfo, IReflectable, ScriptSerializableObjectInfoRTTI>
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

		UINT32& getTypeId(ScriptSerializableObjectInfo* obj)
		{
			return obj->mTypeId;
		}

		void setTypeId(ScriptSerializableObjectInfo* obj, UINT32& val)
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

		ScriptSerializableFieldInfoPtr getSerializableFieldInfo(ScriptSerializableObjectInfo* obj, UINT32 idx) 
		{ 
			auto iter = obj->mFields.begin();
			for(UINT32 i = 0; i < idx; i++)
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
			addReflectablePtrField("mTypeInfo", 0, &ScriptSerializableObjectInfoRTTI::getTypeInfo, &ScriptSerializableObjectInfoRTTI::setTypeInfo);
			addPlainField("mTypeId", 1, &ScriptSerializableObjectInfoRTTI::getTypeId, &ScriptSerializableObjectInfoRTTI::setTypeId);
			addReflectablePtrField("mBaseClass", 2, &ScriptSerializableObjectInfoRTTI::getBaseClass, &ScriptSerializableObjectInfoRTTI::setBaseClass);

			addReflectablePtrArrayField("mFields", 3, &ScriptSerializableObjectInfoRTTI::getSerializableFieldInfo, 
				&ScriptSerializableObjectInfoRTTI::getSerializableFieldInfoArraySize, &ScriptSerializableObjectInfoRTTI::setSerializableFieldInfo, 
				&ScriptSerializableObjectInfoRTTI::setSerializableFieldInfoArraySize);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableObjectInfo";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableObjectInfo;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableObjectInfo>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldInfoRTTI : public RTTIType<ScriptSerializableFieldInfo, IReflectable, ScriptSerializableFieldInfoRTTI>
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

		String& getName(ScriptSerializableFieldInfo* obj)
		{
			return obj->mName;
		}

		void setName(ScriptSerializableFieldInfo* obj, String& val)
		{
			obj->mName = val;
		}

		UINT32& getFieldId(ScriptSerializableFieldInfo* obj)
		{
			return obj->mFieldId;
		}

		void setFieldId(ScriptSerializableFieldInfo* obj, UINT32& val)
		{
			obj->mFieldId = val;
		}

		UINT32& getFlags(ScriptSerializableFieldInfo* obj)
		{
			return (UINT32&)obj->mFlags;
		}

		void setFlags(ScriptSerializableFieldInfo* obj, UINT32& val)
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

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableFieldInfo";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldInfo;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{return cm_shared_ptr<ScriptSerializableFieldInfo>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoRTTI : public RTTIType<ScriptSerializableTypeInfo, IReflectable, ScriptSerializableTypeInfoRTTI>
	{
	private:

	public:
		ScriptSerializableTypeInfoRTTI()
		{

		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableTypeInfo";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableTypeInfo;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			CM_EXCEPT(InvalidStateException, "Cannot instantiate an abstract class");
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoPrimitiveRTTI : public RTTIType<ScriptSerializableTypeInfoPrimitive, ScriptSerializableTypeInfo, ScriptSerializableTypeInfoPrimitiveRTTI>
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

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableTypeInfoPrimitive";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableTypeInfoPrimitive;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoObjectRTTI : public RTTIType<ScriptSerializableTypeInfoObject, ScriptSerializableTypeInfo, ScriptSerializableTypeInfoObjectRTTI>
	{
	private:
		String& getTypeNamespace(ScriptSerializableTypeInfoObject* obj)
		{
			return obj->mTypeNamespace;
		}

		void setTypeNamespace(ScriptSerializableTypeInfoObject* obj, String& val)
		{
			obj->mTypeNamespace = val;
		}

		String& getTypeName(ScriptSerializableTypeInfoObject* obj)
		{
			return obj->mTypeName;
		}

		void setTypeName(ScriptSerializableTypeInfoObject* obj, String& val)
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

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableTypeInfoObject";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableTypeInfoObject;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableTypeInfoObject>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoArrayRTTI : public RTTIType<ScriptSerializableTypeInfoArray, ScriptSerializableTypeInfo, ScriptSerializableTypeInfoArrayRTTI>
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

		UINT32& getRank(ScriptSerializableTypeInfoArray* obj)
		{
			return obj->mRank;
		}

		void setRank(ScriptSerializableTypeInfoArray* obj, UINT32& val)
		{
			obj->mRank = val;
		}

	public:
		ScriptSerializableTypeInfoArrayRTTI()
		{
			addReflectablePtrField("mElementType", 0, &ScriptSerializableTypeInfoArrayRTTI::getElementType, &ScriptSerializableTypeInfoArrayRTTI::setElementType);
			addPlainField("mRank", 1, &ScriptSerializableTypeInfoArrayRTTI::getRank, &ScriptSerializableTypeInfoArrayRTTI::setRank);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableTypeInfoArray";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableTypeInfoArray;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableTypeInfoArray>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoListRTTI : public RTTIType<ScriptSerializableTypeInfoList, ScriptSerializableTypeInfo, ScriptSerializableTypeInfoListRTTI>
	{
	private:
		ScriptSerializableTypeInfoPtr getElementType(ScriptSerializableTypeInfoList* obj)
		{
			return obj->mElementType;
		}

		void setElementType(ScriptSerializableTypeInfoList* obj, ScriptSerializableTypeInfoPtr val)
		{
			obj->mElementType = val;
		}

	public:
		ScriptSerializableTypeInfoListRTTI()
		{
			addReflectablePtrField("mElementType", 0, &ScriptSerializableTypeInfoListRTTI::getElementType, &ScriptSerializableTypeInfoListRTTI::setElementType);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableTypeInfoList";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableTypeInfoList;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableTypeInfoList>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoDictionaryRTTI : public RTTIType<ScriptSerializableTypeInfoDictionary, ScriptSerializableTypeInfo, ScriptSerializableTypeInfoDictionaryRTTI>
	{
	private:
		ScriptSerializableTypeInfoPtr getKeyType(ScriptSerializableTypeInfoDictionary* obj)
		{
			return obj->mKeyType;
		}

		void setKeyType(ScriptSerializableTypeInfoDictionary* obj, ScriptSerializableTypeInfoPtr val)
		{
			obj->mKeyType = val;
		}

		ScriptSerializableTypeInfoPtr getValueType(ScriptSerializableTypeInfoDictionary* obj)
		{
			return obj->mValueType;
		}

		void setValueType(ScriptSerializableTypeInfoDictionary* obj, ScriptSerializableTypeInfoPtr val)
		{
			obj->mValueType = val;
		}

	public:
		ScriptSerializableTypeInfoDictionaryRTTI()
		{
			addReflectablePtrField("mKeyType", 0, &ScriptSerializableTypeInfoDictionaryRTTI::getKeyType, &ScriptSerializableTypeInfoDictionaryRTTI::setKeyType);
			addReflectablePtrField("mValueType", 1, &ScriptSerializableTypeInfoDictionaryRTTI::getValueType, &ScriptSerializableTypeInfoDictionaryRTTI::setValueType);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableTypeInfoDictionary";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableTypeInfoDictionary;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableTypeInfoDictionary>();
		}
	};
}
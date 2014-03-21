#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableArray : public CM::IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
		ScriptSerializableArray(const ConstructPrivately& dummy, const ScriptSerializableTypeInfoArrayPtr& typeInfo, MonoObject* managedInstance);
		ScriptSerializableArray(const ConstructPrivately& dummy);

		MonoObject* getManagedInstance() const { return mManagedInstance; }

		static ScriptSerializableArrayPtr create(MonoObject* managedInstance, const ScriptSerializableTypeInfoArrayPtr& typeInfo);

	protected:
		ScriptSerializableTypeInfoArrayPtr mArrayTypeInfo;
		CM::Vector<ScriptSerializableFieldDataPtr>::type mArrayEntries;
		MonoObject* mManagedInstance;
		::MonoClass* mClass;
		CM::UINT32 mNumElements;

		void setFieldData(CM::UINT32 arrayIdx, const ScriptSerializableFieldDataPtr& val);
		ScriptSerializableFieldDataPtr getFieldData(CM::UINT32 arrayIdx);

		void setValue(CM::UINT32 arrayIdx, void* val);
		void* getValue(CM::UINT32 arrayIdx);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		static ScriptSerializableArrayPtr createEmpty();

	public:
		friend class ScriptSerializableArrayRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};
}
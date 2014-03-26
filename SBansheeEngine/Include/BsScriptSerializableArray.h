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
		MonoObject* mManagedInstance;

		ScriptSerializableTypeInfoArrayPtr mArrayTypeInfo;
		CM::Vector<ScriptSerializableFieldDataPtr>::type mArrayEntries;
		
		CM::Vector<CM::UINT32>::type mNumElements;
		CM::UINT32 mElemSize;

		/**
		 * @brief	Populates internal field data based on currently active managed instance.
		 */
		void serializeManagedInstance();

		/**
		 * @brief	Creates a new managed instance and populates it with stored field data.
		 */
		void deserializeManagedInstance();

		void setFieldData(CM::UINT32 arrayIdx, const ScriptSerializableFieldDataPtr& val);
		ScriptSerializableFieldDataPtr getFieldData(CM::UINT32 arrayIdx);

		void setValue(CM::UINT32 arrayIdx, void* val);
		void* getValue(CM::UINT32 arrayIdx);

		CM::UINT32 toSequentialIdx(const CM::Vector<CM::UINT32>::type& idx) const;
		CM::UINT32 getLength(CM::UINT32 dimension) const;

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
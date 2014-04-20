#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableArray : public IReflectable
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
		::MonoClass* mElementMonoClass;

		ScriptSerializableTypeInfoArrayPtr mArrayTypeInfo;
		Vector<ScriptSerializableFieldDataPtr>::type mArrayEntries;
		
		Vector<UINT32>::type mNumElements;
		UINT32 mElemSize;

		void initMonoObjects();

		/**
		 * @brief	Populates internal field data based on currently active managed instance.
		 */
		void serializeManagedInstance();

		/**
		 * @brief	Creates a new managed instance and populates it with stored field data.
		 */
		void deserializeManagedInstance();

		void setFieldData(UINT32 arrayIdx, const ScriptSerializableFieldDataPtr& val);
		ScriptSerializableFieldDataPtr getFieldData(UINT32 arrayIdx);

		void setValue(UINT32 arrayIdx, void* val);

		UINT32 toSequentialIdx(const Vector<UINT32>::type& idx) const;
		UINT32 getLength(UINT32 dimension) const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		static ScriptSerializableArrayPtr createEmpty();

	public:
		friend class ScriptSerializableArrayRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}
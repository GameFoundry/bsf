#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableArray : public IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
		ManagedSerializableArray(const ConstructPrivately& dummy, const ManagedSerializableTypeInfoArrayPtr& typeInfo, MonoObject* managedInstance);
		ManagedSerializableArray(const ConstructPrivately& dummy);

		MonoObject* getManagedInstance() const { return mManagedInstance; }

		static ManagedSerializableArrayPtr create(MonoObject* managedInstance, const ManagedSerializableTypeInfoArrayPtr& typeInfo);

	protected:
		MonoObject* mManagedInstance;
		::MonoClass* mElementMonoClass;

		ManagedSerializableTypeInfoArrayPtr mArrayTypeInfo;
		Vector<ManagedSerializableFieldDataPtr> mArrayEntries;
		
		Vector<UINT32> mNumElements;
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

		void setFieldData(UINT32 arrayIdx, const ManagedSerializableFieldDataPtr& val);
		ManagedSerializableFieldDataPtr getFieldData(UINT32 arrayIdx);

		void setValue(UINT32 arrayIdx, void* val);

		UINT32 toSequentialIdx(const Vector<UINT32>& idx) const;
		UINT32 getLength(UINT32 dimension) const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		static ManagedSerializableArrayPtr createEmpty();

	public:
		friend class ManagedSerializableArrayRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}
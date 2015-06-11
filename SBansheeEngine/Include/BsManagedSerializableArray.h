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

		void resize(const Vector<UINT32>& newSizes);
		UINT32 getLength(UINT32 dimension) const { return mNumElements[dimension]; }
		Vector<UINT32> getLengths() const { return mNumElements; }
		UINT32 getTotalLength() const;

		void setFieldData(UINT32 arrayIdx, const ManagedSerializableFieldDataPtr& val);
		ManagedSerializableFieldDataPtr getFieldData(UINT32 arrayIdx);

		ManagedSerializableTypeInfoArrayPtr getTypeInfo() const { return mArrayTypeInfo; }

		static ManagedSerializableArrayPtr createFromExisting(MonoObject* managedInstance, const ManagedSerializableTypeInfoArrayPtr& typeInfo);
		static ManagedSerializableArrayPtr createNew(const ManagedSerializableTypeInfoArrayPtr& typeInfo, const Vector<UINT32>& sizes);
		static MonoObject* createManagedInstance(const ManagedSerializableTypeInfoArrayPtr& typeInfo, const Vector<UINT32>& sizes);

	protected:
		MonoObject* mManagedInstance;
		::MonoClass* mElementMonoClass;
		MonoMethod* mCopyMethod;

		ManagedSerializableTypeInfoArrayPtr mArrayTypeInfo;

		Vector<UINT32> mNumElements;
		UINT32 mElemSize;

		void initMonoObjects();
		UINT32 getLengthInternal(UINT32 dimension) const;

		/**
		 * @brief	Creates a new managed instance and populates it with provided entries.
		 */
		void deserializeManagedInstance(const Vector<ManagedSerializableFieldDataPtr>& entries);

		void setValue(UINT32 arrayIdx, void* val);
		UINT32 toSequentialIdx(const Vector<UINT32>& idx) const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		static ManagedSerializableArrayPtr createNew();

	public:
		friend class ManagedSerializableArrayRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}
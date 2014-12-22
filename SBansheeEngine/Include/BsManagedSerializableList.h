#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableList : public IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
		ManagedSerializableList(const ConstructPrivately& dummy, const ManagedSerializableTypeInfoListPtr& typeInfo, MonoObject* managedInstance);
		ManagedSerializableList(const ConstructPrivately& dummy);

		MonoObject* getManagedInstance() const { return mManagedInstance; }

		static ManagedSerializableListPtr createFromExisting(MonoObject* managedInstance, const ManagedSerializableTypeInfoListPtr& typeInfo);
		static ManagedSerializableListPtr createFromNew(const ManagedSerializableTypeInfoListPtr& typeInfo, UINT32 size);
		static MonoObject* createManagedInstance(const ManagedSerializableTypeInfoListPtr& typeInfo, UINT32 size);

	protected:
		MonoObject* mManagedInstance;

		MonoMethod* mAddMethod;
		MonoProperty* mItemProp;
		MonoProperty* mCountProp;

		ManagedSerializableTypeInfoListPtr mListTypeInfo;
		UINT32 mNumElements;

		void initMonoObjects(MonoClass* listClass);

		/**
		 * @brief	Creates a new managed instance and populates it with stored field data.
		 */
		void deserializeManagedInstance(const Vector<ManagedSerializableFieldDataPtr>& entries);

		void setFieldData(UINT32 arrayIdx, const ManagedSerializableFieldDataPtr& val);
		void addFieldData(const ManagedSerializableFieldDataPtr& val);
		ManagedSerializableFieldDataPtr getFieldData(UINT32 arrayIdx);

		UINT32 getLength() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		static ManagedSerializableListPtr createEmpty();

	public:
		friend class ManagedSerializableListRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}
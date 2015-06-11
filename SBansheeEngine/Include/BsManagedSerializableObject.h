#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableObject : public IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
		ManagedSerializableObject(const ConstructPrivately& dummy, ManagedSerializableObjectInfoPtr objInfo, MonoObject* managedInstance);
		ManagedSerializableObject(const ConstructPrivately& dummy);

		MonoObject* getManagedInstance() const { return mManagedInstance; }
		ManagedSerializableObjectInfoPtr getObjectInfo() const { return mObjInfo; }

		void setFieldData(const ManagedSerializableFieldInfoPtr& fieldInfo, const ManagedSerializableFieldDataPtr& val);
		ManagedSerializableFieldDataPtr getFieldData(const ManagedSerializableFieldInfoPtr& fieldInfo) const;

		void setObjectData(const ManagedSerializableObjectDataPtr& objectData, const ManagedSerializableObjectInfoPtr& originalEntriesType);
		ManagedSerializableObjectDataPtr getObjectData() const;

		static ManagedSerializableObjectPtr createFromExisting(MonoObject* managedInstance);
		static ManagedSerializableObjectPtr createNew(const ManagedSerializableTypeInfoObjectPtr& type);
		static MonoObject* createManagedInstance(const ManagedSerializableTypeInfoObjectPtr& type);
	protected:
		ManagedSerializableObjectInfoPtr mObjInfo;
		MonoObject* mManagedInstance;

		/**
		 * @brief	Creates a new managed instance and populates it with provided field data.
		 */
		void deserializeManagedInstance(const Vector<ManagedSerializableFieldDataEntryPtr>& data);

		void setFieldEntries(const Vector<ManagedSerializableFieldDataEntryPtr>& data, const ManagedSerializableObjectInfoPtr& originalEntriesType);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		static ManagedSerializableObjectPtr createEmpty();

	public:
		friend class ManagedSerializableObjectRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}
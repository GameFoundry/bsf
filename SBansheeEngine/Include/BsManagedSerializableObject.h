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

		static ManagedSerializableObjectPtr createFromExisting(MonoObject* managedInstance);
		static ManagedSerializableObjectPtr createFromNew(const ManagedSerializableTypeInfoObjectPtr& type);
		static MonoObject* createManagedInstance(const ManagedSerializableTypeInfoObjectPtr& type);

	protected:
		ManagedSerializableObjectInfoPtr mObjInfo;
		MonoObject* mManagedInstance;
		Vector<ManagedSerializableFieldDataEntryPtr> mFieldEntries;

		/**
		 * @brief	Populates internal field data based on currently active managed instance.
		 */
		void serializeManagedInstance();

		/**
		 * @brief	Creates a new managed instance and populates it with stored field data.
		 */
		void deserializeManagedInstance();

		void setFieldData(const ManagedSerializableFieldInfoPtr& fieldInfo, const ManagedSerializableFieldDataPtr& val);
		ManagedSerializableFieldDataPtr getFieldData(const ManagedSerializableFieldInfoPtr& fieldInfo);

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
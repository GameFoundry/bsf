#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableObject : public CM::IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
		ScriptSerializableObject(const ConstructPrivately& dummy, ScriptSerializableObjectInfoPtr objInfo, MonoObject* managedInstance);
		ScriptSerializableObject(const ConstructPrivately& dummy);

		MonoObject* getManagedInstance() const { return mManagedInstance; }

		static ScriptSerializableObjectPtr create(MonoObject* managedInstance);

	protected:
		ScriptSerializableObjectInfoPtr mObjInfo;
		MonoObject* mManagedInstance;
		CM::Vector<ScriptSerializableFieldDataEntryPtr>::type mFieldEntries;

		/**
		 * @brief	Populates internal field data based on currently active managed instance.
		 */
		void serializeManagedInstance();

		/**
		 * @brief	Creates a new managed instance and populates it with stored field data.
		 */
		void deserializeManagedInstance();

		void setFieldData(const ScriptSerializableFieldInfoPtr& fieldInfo, const ScriptSerializableFieldDataPtr& val);
		ScriptSerializableFieldDataPtr getFieldData(const ScriptSerializableFieldInfoPtr& fieldInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		static ScriptSerializableObjectPtr createEmpty();

	public:
		friend class ScriptSerializableObjectRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};
}
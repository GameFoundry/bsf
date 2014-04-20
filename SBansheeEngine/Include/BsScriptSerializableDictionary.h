#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableDictionary : public IReflectable
	{
	private:
		class Enumerator
		{
		public:
			Enumerator(MonoObject* instance, const ScriptSerializableDictionary* parent);

			ScriptSerializableFieldDataPtr getKey() const;
			ScriptSerializableFieldDataPtr getValue() const;

			bool moveNext();

		private:
			MonoObject* mInstance;
			MonoObject* mCurrent;
			const ScriptSerializableDictionary* mParent;
		};

		struct ConstructPrivately {};

	public:
		ScriptSerializableDictionary(const ConstructPrivately& dummy, const ScriptSerializableTypeInfoDictionaryPtr& typeInfo, MonoObject* managedInstance);
		ScriptSerializableDictionary(const ConstructPrivately& dummy);

		MonoObject* getManagedInstance() const { return mManagedInstance; }

		static ScriptSerializableDictionaryPtr create(MonoObject* managedInstance, const ScriptSerializableTypeInfoDictionaryPtr& typeInfo);

	protected:
		MonoObject* mManagedInstance;

		MonoMethod* mAddMethod;
		MonoMethod* mGetEnumerator;
		MonoMethod* mEnumMoveNext;
		MonoProperty* mEnumCurrentProp;
		MonoProperty* mKeyProp;
		MonoProperty* mValueProp;

		ScriptSerializableTypeInfoDictionaryPtr mDictionaryTypeInfo;
		Vector<ScriptSerializableFieldDataPtr>::type mKeyEntries;
		Vector<ScriptSerializableFieldDataPtr>::type mValueEntries;

		void initMonoObjects(MonoClass* dictionaryClass);

		/**
		 * @brief	Populates internal field data based on currently active managed instance.
		 */
		void serializeManagedInstance();

		/**
		 * @brief	Creates a new managed instance and populates it with stored field data.
		 */
		void deserializeManagedInstance();

		void setFieldData(const ScriptSerializableFieldDataPtr& key, const ScriptSerializableFieldDataPtr& val);
		Enumerator getEnumerator() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		static ScriptSerializableDictionaryPtr createEmpty();

	public:
		friend class ScriptSerializableDictionaryRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}
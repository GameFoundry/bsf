#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableDictionary : public IReflectable
	{
	private:
		class Enumerator
		{
		public:
			Enumerator(MonoObject* instance, const ManagedSerializableDictionary* parent);

			ManagedSerializableFieldDataPtr getKey() const;
			ManagedSerializableFieldDataPtr getValue() const;

			bool moveNext();

		private:
			MonoObject* mInstance;
			MonoObject* mCurrent;
			const ManagedSerializableDictionary* mParent;
		};

		struct ConstructPrivately {};

	public:
		ManagedSerializableDictionary(const ConstructPrivately& dummy, const ManagedSerializableTypeInfoDictionaryPtr& typeInfo, MonoObject* managedInstance);
		ManagedSerializableDictionary(const ConstructPrivately& dummy);

		MonoObject* getManagedInstance() const { return mManagedInstance; }

		static ManagedSerializableDictionaryPtr createFromExisting(MonoObject* managedInstance, const ManagedSerializableTypeInfoDictionaryPtr& typeInfo);
		static ManagedSerializableDictionaryPtr createFromNew(const ManagedSerializableTypeInfoDictionaryPtr& typeInfo);
		static MonoObject* createManagedInstance(const ManagedSerializableTypeInfoDictionaryPtr& typeInfo);

	protected:
		MonoObject* mManagedInstance;

		MonoMethod* mAddMethod;
		MonoMethod* mGetEnumerator;
		MonoMethod* mEnumMoveNext;
		MonoProperty* mEnumCurrentProp;
		MonoProperty* mKeyProp;
		MonoProperty* mValueProp;

		ManagedSerializableTypeInfoDictionaryPtr mDictionaryTypeInfo;

		void initMonoObjects(MonoClass* dictionaryClass);

		/**
		 * @brief	Populates provided field data arrays based on currently active managed instance.
		 */
		void serializeManagedInstance(Vector<ManagedSerializableFieldDataPtr>& keyEntries, 
			Vector<ManagedSerializableFieldDataPtr>& valueEntries);

		/**
		 * @brief	Creates a new managed instance and populates it with provided field data.
		 */
		void deserializeManagedInstance(const Vector<ManagedSerializableFieldDataPtr>& keyEntries, 
			const Vector<ManagedSerializableFieldDataPtr>& valueEntries);

		void setFieldData(const ManagedSerializableFieldDataPtr& key, const ManagedSerializableFieldDataPtr& val);
		Enumerator getEnumerator() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		static ManagedSerializableDictionaryPtr createEmpty();

	public:
		friend class ManagedSerializableDictionaryRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}
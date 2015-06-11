#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableDictionary : public IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
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

	public:
		ManagedSerializableDictionary(const ConstructPrivately& dummy, const ManagedSerializableTypeInfoDictionaryPtr& typeInfo, MonoObject* managedInstance);
		ManagedSerializableDictionary(const ConstructPrivately& dummy);

		MonoObject* getManagedInstance() const { return mManagedInstance; }

		ManagedSerializableTypeInfoDictionaryPtr getTypeInfo() const { return mDictionaryTypeInfo; }

		ManagedSerializableFieldDataPtr getFieldData(const ManagedSerializableFieldDataPtr& key);
		void setFieldData(const ManagedSerializableFieldDataPtr& key, const ManagedSerializableFieldDataPtr& val);
		void removeFieldData(const ManagedSerializableFieldDataPtr& key);
		bool contains(const ManagedSerializableFieldDataPtr& key) const;
		Enumerator getEnumerator() const;

		static ManagedSerializableDictionaryPtr createFromExisting(MonoObject* managedInstance, const ManagedSerializableTypeInfoDictionaryPtr& typeInfo);
		static ManagedSerializableDictionaryPtr createNew(const ManagedSerializableTypeInfoDictionaryPtr& typeInfo);
		static MonoObject* createManagedInstance(const ManagedSerializableTypeInfoDictionaryPtr& typeInfo);

	protected:
		MonoObject* mManagedInstance;

		MonoMethod* mAddMethod;
		MonoMethod* mRemoveMethod;
		MonoMethod* mTryGetValueMethod;
		MonoMethod* mContainsKeyMethod;
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
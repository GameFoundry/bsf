#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	/**
	 * @brief	TODO
	 *
	 * @note	This class can be in two states:
	 *			 - Linked - When the object has a link to a managed object. This is the default 
	 *                      state when a new instance of ManagedSerializableObject is created.
	 *						Any operations during this state will operate directly on the linked
	 *						managed object.
	 *			 - Serialized - When the object has no link to the managed object but instead just
	 *							contains cached object and field data that may be used for initializing
	 *							a managed object. Any operations during this state will operate
	 *							only on the cached internal data.
	 *			You can transfer between these states by calling serialize(linked->serialized) &
	 *	
	 */
	struct BS_SCR_BE_EXPORT ManagedSerializableDictionaryKeyValue : public IReflectable
	{
		ManagedSerializableDictionaryKeyValue() {}
		ManagedSerializableDictionaryKeyValue(const ManagedSerializableFieldDataPtr& key,
			const ManagedSerializableFieldDataPtr& value);

		ManagedSerializableFieldDataPtr key;
		ManagedSerializableFieldDataPtr value;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableDictionaryKeyValueRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableDictionary : public IReflectable
	{
	private:
		struct ConstructPrivately {};
		
		struct BS_SCR_BE_EXPORT Hash
		{
			inline size_t operator()(const ManagedSerializableFieldDataPtr& x) const;
		};

		struct BS_SCR_BE_EXPORT Equals
		{
			inline bool operator()(const ManagedSerializableFieldDataPtr& a, const ManagedSerializableFieldDataPtr& b) const;
		};

	public:
		typedef UnorderedMap<ManagedSerializableFieldDataPtr, ManagedSerializableFieldDataPtr> CachedEntriesMap;

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
			CachedEntriesMap::const_iterator mCachedIter;
			bool mIteratorInitialized;

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

		void serialize();
		void deserialize();

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
		CachedEntriesMap mCachedEntries;

		void initMonoObjects(MonoClass* dictionaryClass);

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
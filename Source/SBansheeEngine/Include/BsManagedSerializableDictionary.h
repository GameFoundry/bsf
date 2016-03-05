//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	/**
	 * @brief	Contains key and value data used for serializing a managed dictionary.
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
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Allows access to an underlying managed dictionary, or a cached version of that dictionary that
	 *			can be serialized/deserialized.
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
	 *			deserialize (serialized->linked).
	 *	
	 */
	class BS_SCR_BE_EXPORT ManagedSerializableDictionary : public IReflectable
	{
	private:
		struct ConstructPrivately {};
		
		/**
		 * @brief	Generates a hash value for field data.
		 */
		struct BS_SCR_BE_EXPORT Hash
		{
			inline size_t operator()(const ManagedSerializableFieldDataPtr& x) const;
		};

		/**
		 * @brief	Compares two field data objects.
		 */
		struct BS_SCR_BE_EXPORT Equals
		{
			inline bool operator()(const ManagedSerializableFieldDataPtr& a, const ManagedSerializableFieldDataPtr& b) const;
		};

	public:
		typedef UnorderedMap<ManagedSerializableFieldDataPtr, ManagedSerializableFieldDataPtr> CachedEntriesMap;

		/**
		 * @brief	Helper class that enumerates over all entires in a managed dictionary. Operates on 
		 *			managed object if the parent dictionary is in linked state, or on cached data otherwise.
		 */
		class Enumerator
		{
		public:
			/**
			 * @brief	Constructs a new enumerator for the provided managed object.
			 *
			 * @param	instance	Managed instance of type System.Collections.Generic.Dictionary.
			 * @param	parent		Serializable parent of the managed instance.
			 */
			Enumerator(MonoObject* instance, const ManagedSerializableDictionary* parent);

			/**
			 * @brief	Returns the wrapped key data at the current enumerator position.
			 *			Only valid to call this if enumerator is valid (i.e. last call to 
			 *			::moveNext returned true).
			 */
			ManagedSerializableFieldDataPtr getKey() const;

			/**
			 * @brief	Returns the wrapped value data at the current enumerator position.
			 *			Only valid to call this if enumerator is valid (i.e. last call to 
			 *			::moveNext returned true).
			 */
			ManagedSerializableFieldDataPtr getValue() const;

			/**
			 * @brief	Moves the enumerator to the next position. Initially enumerator is
			 *			at an invalid position and must be called at least once before accesing
			 *			its data.
			 *
			 * @return	Returns if the enumerator is at valid position. When the enumerator returns
			 *			false it means there are no more elements to enumerate.
			 */
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

		/**
		 * @brief	Returns the internal managed instance of the dictionary. This will return null if
		 *			the object is in serialized mode.
		 */
		MonoObject* getManagedInstance() const { return mManagedInstance; }

		/**
		 * @brief	Returns the type information for the internal dictionary.
		 */
		ManagedSerializableTypeInfoDictionaryPtr getTypeInfo() const { return mDictionaryTypeInfo; }

		/**
		 * @brief	Returns the dictionary value at the specified key. If the key doesn't exist
		 *			the default value for the type is returned. Operates on managed object if in 
		 *			linked state, or on cached data otherwise.
		 *
		 * @param	key		Wrapper around the key data at which to retrieve the value.
		 *
		 * @return	A wrapper around the value in the dictionary at the specified key.
		 */
		ManagedSerializableFieldDataPtr getFieldData(const ManagedSerializableFieldDataPtr& key);

		/**
		 * @brief	Sets the dictionary value at the specified key. Operates on managed object if in 
		 *			linked state, or on cached data otherwise.
		 *
		 * @param	key		Wrapper around the key data at which to set the value.
		 * @param	val		Wrapper around the value to set at the specified key.
		 */
		void setFieldData(const ManagedSerializableFieldDataPtr& key, const ManagedSerializableFieldDataPtr& val);

		/**
		 * @brief	Deletes the key/value pair at the specified key. If the key doesn't exist
		 *			this operation does nothing. Operates on managed object if in linked state, 
		 *			or on cached data otherwise.
		 *
		 * @param	key		Wrapper around the key data at which to delete the value.
		 */
		void removeFieldData(const ManagedSerializableFieldDataPtr& key);

		/**
		 * @brief	Checks if the dictionary contains the specified key. Operates on managed object 
		 *			if in linked state, or on cached data otherwise.
		 *
		 * @param	key		Wrapper around the key data which to check.
		 */
		bool contains(const ManagedSerializableFieldDataPtr& key) const;

		/**
		 * @brief	Returns an enumerator object that allows you to iterate over
		 *			all key/value pairs in the dictionary.
		 */
		Enumerator getEnumerator() const;

		/**
		 * @brief	Serializes the internal managed object into a set of cached data that can be saved
		 *			in memory/disk and can be deserialized later. Does nothing if object is already is 
		 *			serialized mode. When in serialized mode the reference to the managed instance will be lost.
		 */
		void serialize();

		/**
		 * @brief	Deserializes a set of cached data into a managed object. This action may fail in case the cached
		 *			data contains a type that no longer exists. You may check if it completely successfully if ::getManagedInstance
		 *			returns non-null after.
		 *
		 *			This action transfers the object into linked mode. All further operations will operate directly on the managed instance
		 *			and the cached data will be cleared. If you call this method on an already linked object the old object will be
		 *			replaced and initialized with empty data (since cached data does not exist).
		 */
		void deserialize();

		/**
		 * @brief	Creates a managed serializable dictionary that references an existing managed dictionary. Created object will be in linked mode.
		 *
		 * @param	managedInstance		Constructed managed instance of the dictionary to link with. Its type must correspond with the provided type info.
		 * @param	typeInfo			Type information for the dictionary and its key/value pair.
		 */
		static ManagedSerializableDictionaryPtr createFromExisting(MonoObject* managedInstance, const ManagedSerializableTypeInfoDictionaryPtr& typeInfo);

		/**
		 * @brief	Creates a managed serializable dictionary that creates and references a brand new managed dictionary instance.
		 *
		 * @param	typeInfo	Type of the dictionary to create.
		 */
		static ManagedSerializableDictionaryPtr createNew(const ManagedSerializableTypeInfoDictionaryPtr& typeInfo);

		/**
		 * @brief	Creates a managed dictionary instance.
		 *
		 * @param	typeInfo	Type of the dictionary to create.
		 */
		static MonoObject* createManagedInstance(const ManagedSerializableTypeInfoDictionaryPtr& typeInfo);

	protected:
		/**
		 * @brief	Retrieves needed Mono types and methods. Should be called
		 *			before performing any operations with the managed object.
		 */
		void initMonoObjects(MonoClass* dictionaryClass);

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

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		/**
		 * @brief	Creates an empty and uninitialized object used for serialization purposes.
		 */
		static ManagedSerializableDictionaryPtr createEmpty();

	public:
		friend class ManagedSerializableDictionaryRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}
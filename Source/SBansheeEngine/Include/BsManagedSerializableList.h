//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	/**
	 * @brief	Allows access to an underlying managed list, or a cached version of that list that
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
	class BS_SCR_BE_EXPORT ManagedSerializableList : public IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
		ManagedSerializableList(const ConstructPrivately& dummy, const ManagedSerializableTypeInfoListPtr& typeInfo, MonoObject* managedInstance);
		ManagedSerializableList(const ConstructPrivately& dummy);

		/**
		 * @brief	Returns the internal managed instance of the list. This will return null if
		 *			the object is in serialized mode.
		 */
		MonoObject* getManagedInstance() const { return mManagedInstance; }

		/**
		 * @brief	Returns the type information for the internal list.
		 */
		ManagedSerializableTypeInfoListPtr getTypeInfo() const { return mListTypeInfo; }

		/**
		 * @brief	Changes the size of the list. Operates on managed object if in linked state, 
		 *			or on cached data otherwise.
		 */
		void resize(UINT32 newSize);

		/**
		 * @brief	Sets a new element value at the specified list index. Operates on 
		 *			managed object if in linked state, or on cached data otherwise.
		 *
		 * @param	arrayIdx	Index at which to set the value.
		 * @param	val			Wrapper around the value to store in the list. Must be of the
		 *						list element type.
		 */
		void setFieldData(UINT32 arrayIdx, const ManagedSerializableFieldDataPtr& val);

		/**
		 * @brief	Returns the element value at the specified list index. Operates on 
		 *			managed object if in linked state, or on cached data otherwise.
		 *
		 * @param	arrayIdx	Index at which to retrieve the value.
		 *
		 * @return	A wrapper around the element value in the list.
		 */
		ManagedSerializableFieldDataPtr getFieldData(UINT32 arrayIdx);

		/**
		 * @brief	Returns the size of the list. Operates on managed object 
		 *			if in linked state, or on cached data otherwise.
		 */
		UINT32 getLength() const { return mNumElements; }

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
		 * @brief	Creates a managed serializable list that references an existing managed list. Created object will be in linked mode.
		 *
		 * @param	managedInstance		Constructed managed instance of the list to link with. Its type must correspond with the provided type info.
		 * @param	typeInfo			Type information for the list and its elements.
		 */
		static ManagedSerializableListPtr createFromExisting(MonoObject* managedInstance, const ManagedSerializableTypeInfoListPtr& typeInfo);

		/**
		 * @brief	Creates a managed serializable list that creates and references a brand new managed list instance.
		 *
		 * @param	typeInfo	Type of the list to create.
		 * @param	size		Initial size of the list.
		 */
		static ManagedSerializableListPtr createNew(const ManagedSerializableTypeInfoListPtr& typeInfo, UINT32 size);

		/**
		 * @brief	Creates a managed list instance.
		 *
		 * @param	typeInfo	Type of the list to create.
		 * @param	size		Initial size of the list.
		 */
		static MonoObject* createManagedInstance(const ManagedSerializableTypeInfoListPtr& typeInfo, UINT32 size);

	protected:
		/**
		 * @brief	Retrieves needed Mono types and methods. Should be called
		 *			before performing any operations with the managed object.
		 */
		void initMonoObjects(MonoClass* listClass);

		/**
		 * @brief	Returns the size of the list. Operates on the 
		 *			internal managed object.
		 */
		UINT32 getLengthInternal() const;

		/**
		 * @brief	Appends data to the end of the list. Operates on
		 *			the internal managed object.
		 */
		void addFieldDataInternal(const ManagedSerializableFieldDataPtr& val);

		MonoObject* mManagedInstance;

		MonoMethod* mAddMethod;
		MonoMethod* mAddRangeMethod;
		MonoMethod* mClearMethod;
		MonoMethod* mCopyToMethod;
		MonoProperty* mItemProp;
		MonoProperty* mCountProp;

		ManagedSerializableTypeInfoListPtr mListTypeInfo;
		Vector<ManagedSerializableFieldDataPtr> mCachedEntries;
		UINT32 mNumElements;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		/**
		 * @brief	Creates an empty and uninitialized object used for serialization purposes.
		 */
		static ManagedSerializableListPtr createEmpty();

	public:
		friend class ManagedSerializableListRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}
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
	class BS_SCR_BE_EXPORT ManagedSerializableList : public IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
		ManagedSerializableList(const ConstructPrivately& dummy, const ManagedSerializableTypeInfoListPtr& typeInfo, MonoObject* managedInstance);
		ManagedSerializableList(const ConstructPrivately& dummy);

		MonoObject* getManagedInstance() const { return mManagedInstance; }
		ManagedSerializableTypeInfoListPtr getTypeInfo() const { return mListTypeInfo; }

		void resize(UINT32 newSize);
		void setFieldData(UINT32 arrayIdx, const ManagedSerializableFieldDataPtr& val);
		ManagedSerializableFieldDataPtr getFieldData(UINT32 arrayIdx);
		UINT32 getLength() const { return mNumElements; }

		void serialize();
		void deserialize();

		static ManagedSerializableListPtr createFromExisting(MonoObject* managedInstance, const ManagedSerializableTypeInfoListPtr& typeInfo);
		static ManagedSerializableListPtr createNew(const ManagedSerializableTypeInfoListPtr& typeInfo, UINT32 size);
		static MonoObject* createManagedInstance(const ManagedSerializableTypeInfoListPtr& typeInfo, UINT32 size);

	protected:
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

		void initMonoObjects(MonoClass* listClass);
		UINT32 getLengthInternal() const;
		void addFieldDataInternal(const ManagedSerializableFieldDataPtr& val);

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
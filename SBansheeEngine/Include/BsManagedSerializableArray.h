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
	class BS_SCR_BE_EXPORT ManagedSerializableArray : public IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
		ManagedSerializableArray(const ConstructPrivately& dummy, const ManagedSerializableTypeInfoArrayPtr& typeInfo, MonoObject* managedInstance);
		ManagedSerializableArray(const ConstructPrivately& dummy);

		MonoObject* getManagedInstance() const { return mManagedInstance; }
		ManagedSerializableTypeInfoArrayPtr getTypeInfo() const { return mArrayTypeInfo; }

		void resize(const Vector<UINT32>& newSizes);
		UINT32 getLength(UINT32 dimension) const { return mNumElements[dimension]; }
		Vector<UINT32> getLengths() const { return mNumElements; }
		UINT32 getTotalLength() const;

		void setFieldData(UINT32 arrayIdx, const ManagedSerializableFieldDataPtr& val);
		ManagedSerializableFieldDataPtr getFieldData(UINT32 arrayIdx);

		void serialize();
		void deserialize();

		static ManagedSerializableArrayPtr createFromExisting(MonoObject* managedInstance, const ManagedSerializableTypeInfoArrayPtr& typeInfo);
		static ManagedSerializableArrayPtr createNew(const ManagedSerializableTypeInfoArrayPtr& typeInfo, const Vector<UINT32>& sizes);
		static MonoObject* createManagedInstance(const ManagedSerializableTypeInfoArrayPtr& typeInfo, const Vector<UINT32>& sizes);

	protected:
		MonoObject* mManagedInstance;
		::MonoClass* mElementMonoClass;
		MonoMethod* mCopyMethod;

		ManagedSerializableTypeInfoArrayPtr mArrayTypeInfo;
		Vector<ManagedSerializableFieldDataPtr> mCachedEntries;
		Vector<UINT32> mNumElements;
		UINT32 mElemSize;

		void initMonoObjects();
		UINT32 getLengthInternal(UINT32 dimension) const;

		void setValueInternal(UINT32 arrayIdx, void* val);
		UINT32 toSequentialIdx(const Vector<UINT32>& idx) const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		static ManagedSerializableArrayPtr createNew();

	public:
		friend class ManagedSerializableArrayRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}
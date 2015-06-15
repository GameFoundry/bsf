#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"
#include "BsManagedSerializableField.h"
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
	 *			deserialize(serialized->linked).
	 */
	class BS_SCR_BE_EXPORT ManagedSerializableObject : public IReflectable
	{
	private:
		struct ConstructPrivately {};

		struct BS_SCR_BE_EXPORT Hash
		{
			inline size_t operator()(const ManagedSerializableFieldKey& x) const;
		};

		struct BS_SCR_BE_EXPORT Equals
		{
			inline bool operator()(const ManagedSerializableFieldKey& a, const ManagedSerializableFieldKey& b) const;
		};

	public:
		ManagedSerializableObject(const ConstructPrivately& dummy, ManagedSerializableObjectInfoPtr objInfo, MonoObject* managedInstance);
		ManagedSerializableObject(const ConstructPrivately& dummy);

		MonoObject* getManagedInstance() const { return mManagedInstance; }
		ManagedSerializableObjectInfoPtr getObjectInfo() const { return mObjInfo; }

		void setFieldData(const ManagedSerializableFieldInfoPtr& fieldInfo, const ManagedSerializableFieldDataPtr& val);
		ManagedSerializableFieldDataPtr getFieldData(const ManagedSerializableFieldInfoPtr& fieldInfo) const;

		void serialize();
		void deserialize();

		static ManagedSerializableObjectPtr createFromExisting(MonoObject* managedInstance);
		static ManagedSerializableObjectPtr createNew(const ManagedSerializableTypeInfoObjectPtr& type);
		static MonoObject* createManagedInstance(const ManagedSerializableTypeInfoObjectPtr& type);
	protected:
		MonoObject* mManagedInstance;

		ManagedSerializableObjectInfoPtr mObjInfo;
		UnorderedMap<ManagedSerializableFieldKey, ManagedSerializableFieldDataPtr, Hash, Equals> mCachedData;

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
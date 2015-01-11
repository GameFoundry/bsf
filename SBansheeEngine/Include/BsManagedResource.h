#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsResource.h"
#include <mono/jit/jit.h>
#include <mono/metadata/object.h>

namespace BansheeEngine
{
	struct ResourceBackupData;

	class BS_SCR_BE_EXPORT ManagedResource : public Resource
	{
	public:
		void construct(MonoObject* object, const HManagedResource& myHandle);

		MonoObject* getManagedInstance() const { return mManagedInstance; }

		ResourceBackupData backup(bool clearExisting = true);
		void restore(MonoObject* instance, const ResourceBackupData& data);

		static HManagedResource create(MonoObject* managedResource);
		static ManagedResourcePtr createEmpty();

	private:
		friend class ScriptManagedResource;

		ManagedResource(MonoObject* managedInstance);
		void destroy();

		MonoObject* mManagedInstance;
		uint32_t mManagedHandle;
		HManagedResource mMyHandle;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedResourceRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		ManagedResource(); // Serialization only
	};

	struct ResourceBackupData
	{
		struct DataBlock
		{
			UINT8* data;
			UINT32 size;
		};

		DataBlock mTypeInfo;
		DataBlock mObjectData;
	};
}
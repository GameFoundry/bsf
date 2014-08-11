#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsResource.h"
#include <mono/jit/jit.h>
#include <mono/metadata/object.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedResource : public Resource
	{
	public:
		void construct(MonoObject* object, const HManagedResource& myHandle);

		MonoObject* getManagedInstance() const { return mManagedInstance; }

		static HManagedResource create(MonoObject* managedResource);
		static ManagedResourcePtr createEmpty();

	private:
		friend class ScriptManagedResource;

		ManagedResource(MonoObject* managedInstance);
		void destroy_internal();

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
}
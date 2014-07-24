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
		ManagedResource(MonoReflectionType* runtimeType);
		void construct(MonoObject* object, MonoReflectionType* runtimeType);

		MonoObject* getManagedInstance() const { return mManagedInstance; }
		MonoReflectionType* getRuntimeType() const { return mRuntimeType; }

		static ManagedResourcePtr createEmpty();

	private:
		void destroy_internal();

		MonoObject* mManagedInstance;
		MonoReflectionType* mRuntimeType;
		uint32_t mManagedHandle;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedResourceRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		ManagedResource() {} // Serialization only
	};
}
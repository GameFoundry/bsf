#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializationDataArray : public CM::IReflectable
	{

	};

	class BS_SCR_BE_EXPORT ManagedSerializationData : public CM::IReflectable
	{
	public:
		static ManagedSerializationDataPtr create(MonoObject* managedInstance);

	protected:
		ManagedSerializationData(const CM::String& ns, const CM::String& typeName);

		CM::String mNamespace;
		CM::String mTypename;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializationDataRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};
}
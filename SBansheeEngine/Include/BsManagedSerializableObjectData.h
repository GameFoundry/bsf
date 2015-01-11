#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableObjectData : public IReflectable
	{
	public:
		Vector<ManagedSerializableFieldDataEntryPtr> mFieldData;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableObjectDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}
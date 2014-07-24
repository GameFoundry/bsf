#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsResourceMetaData.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedResourceMetaData : public ResourceMetaData
	{
	public:
		String typeNamespace;
		String typeName;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class ManagedResourceMetaDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}
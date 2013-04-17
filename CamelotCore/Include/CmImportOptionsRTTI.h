#pragma once

#include "CmPrerequisites.h"
#include "CmImportOptions.h"
#include "CmRTTIType.h"

namespace CamelotFramework
{
	class CM_EXPORT ImportOptionsRTTI : public RTTIType<ImportOptions, IReflectable, ImportOptionsRTTI>
	{
	public:
		ImportOptionsRTTI()
		{
		}

		virtual const String& getRTTIName()
		{
			static String name = "ImportOptions";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ImportOptions;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ImportOptionsPtr(CM_NEW(ImportOptions, PoolAlloc) ImportOptions(),
				&MemAllocDeleter<ImportOptions, PoolAlloc>::deleter);
		}
	};
}
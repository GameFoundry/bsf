#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmResourceRef.h"

namespace CamelotEngine
{
	class CM_EXPORT ResourceRefRTTI : public RTTIType<ResourceRefBase, IReflectable, ResourceRefRTTI>
	{
	private:

	public:
		ResourceRefRTTI()
		{

		}

		virtual const String& getRTTIName()
		{
			static String name = "ResourceRef";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ResourceRef;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return std::shared_ptr<ResourceRefBase>(new ResourceRefBase());
		}
	};
}
#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmComponent.h"

namespace CamelotEngine
{
	class CM_EXPORT ComponentRTTI : public RTTIType<Component, IReflectable, ComponentRTTI>
	{
	private:

	public:
		ComponentRTTI()
		{

		}

		virtual const String& getRTTIName()
		{
			static String name = "Component";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Component;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			CM_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};
}
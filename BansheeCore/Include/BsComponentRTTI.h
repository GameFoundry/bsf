#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsComponent.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT ComponentRTTI : public RTTIType<Component, GameObject, ComponentRTTI>
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
			BS_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "BsManagedComponent.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedComponentRTTI : public CM::RTTIType<ManagedComponent, CM::Component, ManagedComponentRTTI>
	{
	private:

	public:
		ManagedComponentRTTI()
		{

		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ManagedComponent";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_ManagedComponent;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return nullptr; // TODO
		}
	};
}
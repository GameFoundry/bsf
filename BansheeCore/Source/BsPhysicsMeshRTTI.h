//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsPhysicsMesh.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	*  @{
	*/

	class BS_CORE_EXPORT PhysicsMeshRTTI : public RTTIType<PhysicsMesh, Resource, PhysicsMeshRTTI>
	{
	private:
		BS_PLAIN_MEMBER(mType)

	public:
		PhysicsMeshRTTI()
		{
			BS_ADD_PLAIN_FIELD(mType, 0)
		}

		const String& getRTTIName() override
		{
			static String name = "PhysicsMesh";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_PhysicsMesh;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			BS_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
			return nullptr;
		}
	};

	/** @} */
	/** @endcond */
}
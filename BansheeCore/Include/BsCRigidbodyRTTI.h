//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCRigidbody.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CRigidbodyRTTI : public RTTIType<CRigidbody, Component, CRigidbodyRTTI>
	{
	private:

	public:
		CRigidbodyRTTI()
		{

		}

		const String& getRTTIName() override
		{
			static String name = "CRigidbody";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CRigidbody;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CRigidbody>();
		}
	};

	/** @} */
	/** @endcond */
}
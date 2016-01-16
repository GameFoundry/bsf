#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGpuResourceData.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT GpuResourceDataRTTI : public RTTIType<GpuResourceData, IReflectable, GpuResourceDataRTTI>
	{
	public:
		GpuResourceDataRTTI()
		{ }

		const String& getRTTIName() override
		{
			static String name = "GpuResourceData";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_GpuResourceData;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			BS_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};

	/** @} */
	/** @endcond */
}
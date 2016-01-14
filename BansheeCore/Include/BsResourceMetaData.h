#pragma once

#include "BsCorePrerequisites.h"
#include "BsIReflectable.h"
#include "BsCoreObject.h"

namespace BansheeEngine
{
	/** @addtogroup Resources
	 *  @{
	 */

	/**	Class containing meta-information describing a resource. */
	class BS_CORE_EXPORT ResourceMetaData : public IReflectable
	{
	public:
		WString displayName;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class ResourceMetaDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/** @} */
}
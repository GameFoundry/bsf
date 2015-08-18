#pragma once

#include "BsCorePrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class for creating import options from. Import options
	 *			are specific for each importer and control how is data imported.
	 */
	class BS_CORE_EXPORT ImportOptions : public IReflectable
	{
	public:
		virtual ~ImportOptions() {}

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class ImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}
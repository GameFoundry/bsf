#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class for creating import options from. Import options
	 *			are specific for each importer and control how is data imported.
	 */
	class CM_EXPORT ImportOptions : public IReflectable
	{
	public:
		virtual ~ImportOptions() {}

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class ImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}
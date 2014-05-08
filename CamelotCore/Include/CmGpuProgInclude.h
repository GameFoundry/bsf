#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"

namespace BansheeEngine
{
	/**
	 * @brief	Raw text resource that serves as an include file for GPU programs.
	 */
	class CM_EXPORT GpuProgInclude : public Resource
	{
	public:
		/**
		 * @brief	Text of the include file.
		 */
		const String& getString() const { return mString; }

		/**
		 * @brief	Creates a new include file resource with the specified include string.
		 */
		static HGpuProgInclude create(const String& includeString);

		/**
		 * @brief	Creates an include file resource with the specified include string.
		 *
		 * @note	Internal method. Use "create" for normal use.
		 */
		static GpuProgIncludePtr _createPtr(const String& includeString);
	private:
		GpuProgInclude(const String& includeString);

		String mString;
	};
}
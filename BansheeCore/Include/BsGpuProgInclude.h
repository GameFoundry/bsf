//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"

namespace BansheeEngine
{
	/**
	 * @brief	Raw text resource that serves as an include file for GPU programs.
	 */
	class BS_CORE_EXPORT GpuProgInclude : public Resource
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
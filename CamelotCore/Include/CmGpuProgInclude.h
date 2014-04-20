#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"

namespace BansheeEngine
{
	class CM_EXPORT GpuProgInclude : public Resource
	{
	public:
		const String& getString() const { return mString; }

		static HGpuProgInclude create(const String& includeString);
		static GpuProgIncludePtr _createPtr(const String& includeString);
	private:
		GpuProgInclude(const String& includeString);

		String mString;
	};
}
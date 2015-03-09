#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGpuProgInclude.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT GpuProgIncludeRTTI : public RTTIType <GpuProgInclude, Resource, GpuProgIncludeRTTI>
	{
	private:
		String& getString(GpuProgInclude* obj) { return obj->mString; }
		void setString(GpuProgInclude* obj, String& val) { obj->mString = val; }
	public:
		GpuProgIncludeRTTI()
		{
			addPlainField("mString", 0, &GpuProgIncludeRTTI::getString, &GpuProgIncludeRTTI::setString);
		}

		virtual const String& getRTTIName()
		{
			static String name = "GpuProgInclude";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_GpuProgramInclude;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GpuProgInclude::_createPtr(""); // Initial string doesn't matter, it'll get overwritten
		}
	};
}
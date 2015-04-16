#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsShaderInclude.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT ShaderIncludeRTTI : public RTTIType <ShaderInclude, Resource, ShaderIncludeRTTI>
	{
	private:
		String& getString(ShaderInclude* obj) { return obj->mString; }
		void setString(ShaderInclude* obj, String& val) { obj->mString = val; }
	public:
		ShaderIncludeRTTI()
		{
			addPlainField("mString", 0, &ShaderIncludeRTTI::getString, &ShaderIncludeRTTI::setString);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ShaderInclude";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ShaderInclude;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ShaderInclude::_createPtr(""); // Initial string doesn't matter, it'll get overwritten
		}
	};
}
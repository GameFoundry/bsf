#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsShaderInclude.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

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

		const String& getRTTIName() override
		{
			static String name = "ShaderInclude";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ShaderInclude;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return ShaderInclude::_createPtr(""); // Initial string doesn't matter, it'll get overwritten
		}
	};

	/** @} */
	/** @endcond */
}
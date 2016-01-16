#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsMaterial.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT MaterialRTTI : public RTTIType<Material, Resource, MaterialRTTI>
	{
	private:
		HShader& getShader(Material* obj) { return obj->mShader; }
		void setShader(Material* obj, HShader& val) { obj->mShader = val; }

		std::shared_ptr<MaterialParams> getMaterialParams(Material* obj) { return obj->mCachedParams; }
		void setMaterialParams(Material* obj, std::shared_ptr<MaterialParams> value) { obj->mRTTIData = value; }

	public:
		MaterialRTTI()
		{
			addReflectableField("mShader", 0, &MaterialRTTI::getShader, &MaterialRTTI::setShader);
			addReflectablePtrField("mMaterialParams", 2, &MaterialRTTI::getMaterialParams, &MaterialRTTI::setMaterialParams);
		}

		void onDeserializationEnded(IReflectable* obj) override;

		const String& getRTTIName() override
		{
			static String name = "Material";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_Material;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override;
	};

	/** @} */
	/** @endcond */
}
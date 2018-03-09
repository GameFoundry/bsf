//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Private/RTTI/BsMaterialRTTI.h"
#include "Material/BsMaterialManager.h"
#include "Material/BsMaterialParams.h"

namespace bs
{
	void MaterialRTTI::onDeserializationEnded(IReflectable* obj, const UnorderedMap<String, UINT64>& params)
	{
		Material* material = static_cast<Material*>(obj);
		material->initialize();

		if(material->mRTTIData.empty())
			return;

		material->initializeTechniques();

		if (material->getNumTechniques() > 0)
		{
			SPtr<MaterialParams> matParams = any_cast<SPtr<MaterialParams>>(material->mRTTIData);

			if(matParams)
				material->setParams(matParams);
		}

		material->mRTTIData = nullptr; // Delete temporary data
	}

	SPtr<IReflectable> MaterialRTTI::newRTTIObject()
	{
		return MaterialManager::instance().createEmpty();
	}
}
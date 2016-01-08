#include "BsMaterialRTTI.h"
#include "BsMaterialManager.h"
#include "BsMaterialParams.h"

namespace BansheeEngine
{
	void MaterialRTTI::onDeserializationEnded(IReflectable* obj)
	{
		Material* material = static_cast<Material*>(obj);
		material->initialize();

		if(material->mRTTIData.empty())
			return;

		material->initBestTechnique();

		std::shared_ptr<MaterialParams> params = any_cast<std::shared_ptr<MaterialParams>>(material->mRTTIData);
		material->setParams(params);

		material->mRTTIData = nullptr; // Delete temporary data
	}

	std::shared_ptr<IReflectable> MaterialRTTI::newRTTIObject()
	{
		return MaterialManager::instance().createEmpty();
	}
}
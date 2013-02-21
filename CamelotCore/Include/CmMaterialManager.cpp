#include "CmMaterialManager.h"
#include "CmMaterial.h"

namespace CamelotEngine
{
	MaterialPtr MaterialManager::create() const
	{
		MaterialPtr newMat(new Material(), &CoreGpuObject::_deleteDelayed);
		newMat->setThisPtr(newMat);
		newMat->initialize();

		return newMat;
	}
}
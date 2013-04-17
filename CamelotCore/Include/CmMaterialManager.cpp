#include "CmMaterialManager.h"
#include "CmMaterial.h"

namespace CamelotFramework
{
	MaterialPtr MaterialManager::create() const
	{
		MaterialPtr newMat(CM_NEW(Material, PoolAlloc) Material(), &CoreObject::_deleteDelayed<Material, PoolAlloc>);
		newMat->setThisPtr(newMat);
		newMat->initialize();

		return newMat;
	}

	MaterialPtr MaterialManager::create(ShaderPtr shader) const
	{
		MaterialPtr newMat(CM_NEW(Material, PoolAlloc) Material(), &CoreObject::_deleteDelayed<Material, PoolAlloc>);
		newMat->setThisPtr(newMat);
		newMat->initialize();
		newMat->setShader(shader);

		return newMat;
	}
}
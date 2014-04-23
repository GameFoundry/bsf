#include "CmMaterialManager.h"
#include "CmMaterial.h"

namespace BansheeEngine
{
	MaterialPtr MaterialManager::create() const
	{
		MaterialPtr newMat = cm_core_ptr<Material, PoolAlloc>(new (cm_alloc<Material, PoolAlloc>()) Material());
		newMat->_setThisPtr(newMat);
		newMat->initialize();

		return newMat;
	}

	MaterialPtr MaterialManager::create(ShaderPtr shader) const
	{
		MaterialPtr newMat = cm_core_ptr<Material, PoolAlloc>(new (cm_alloc<Material, PoolAlloc>()) Material());
		newMat->_setThisPtr(newMat);
		newMat->initialize();
		newMat->setShader(shader);

		return newMat;
	}
}
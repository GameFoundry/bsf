#include "CmMaterialManager.h"
#include "CmMaterial.h"

namespace CamelotFramework
{
	MaterialPtr MaterialManager::create() const
	{
		MaterialPtr newMat = cm_core_ptr<Material, PoolAlloc>(new (cm_alloc<Material, PoolAlloc>()) Material());
		newMat->setThisPtr(newMat);
		newMat->initialize();

		return newMat;
	}

	MaterialPtr MaterialManager::create(ShaderPtr shader) const
	{
		MaterialPtr newMat = cm_core_ptr<Material, PoolAlloc>(new (cm_alloc<Material, PoolAlloc>()) Material());
		newMat->setThisPtr(newMat);
		newMat->initialize();
		newMat->setShader(shader);

		return newMat;
	}
}
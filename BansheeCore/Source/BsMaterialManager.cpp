#include "BsMaterialManager.h"
#include "BsMaterial.h"

namespace BansheeEngine
{
	MaterialPtr MaterialManager::create() const
	{
		MaterialPtr newMat = bs_core_ptr<Material, PoolAlloc>(new (bs_alloc<Material, PoolAlloc>()) Material());
		newMat->_setThisPtr(newMat);
		newMat->initialize();

		return newMat;
	}

	MaterialPtr MaterialManager::create(const HShader& shader) const
	{
		MaterialPtr newMat = bs_core_ptr<Material, PoolAlloc>(new (bs_alloc<Material, PoolAlloc>()) Material(shader));
		newMat->_setThisPtr(newMat);
		newMat->initialize();

		return newMat;
	}

	MaterialPtr MaterialManager::createEmpty() const
	{
		MaterialPtr newMat = bs_core_ptr<Material, PoolAlloc>(new (bs_alloc<Material, PoolAlloc>()) Material());
		newMat->_setThisPtr(newMat);

		return newMat;
	}
}
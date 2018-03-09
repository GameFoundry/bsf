//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Material/BsMaterialManager.h"
#include "Material/BsMaterial.h"

namespace bs
{
	SPtr<Material> MaterialManager::create() const
	{
		SPtr<Material> newMat = bs_core_ptr<Material>(new (bs_alloc<Material>()) Material());
		newMat->_setThisPtr(newMat);
		newMat->initialize();

		return newMat;
	}

	SPtr<Material> MaterialManager::create(const HShader& shader) const
	{
		SPtr<Material> newMat = bs_core_ptr<Material>(new (bs_alloc<Material>()) Material(shader));
		newMat->_setThisPtr(newMat);
		newMat->initialize();

		return newMat;
	}

	SPtr<Material> MaterialManager::createEmpty() const
	{
		SPtr<Material> newMat = bs_core_ptr<Material>(new (bs_alloc<Material>()) Material());
		newMat->_setThisPtr(newMat);

		return newMat;
	}
}
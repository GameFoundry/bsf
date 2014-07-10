//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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

	MaterialPtr MaterialManager::create(ShaderPtr shader) const
	{
		MaterialPtr newMat = bs_core_ptr<Material, PoolAlloc>(new (bs_alloc<Material, PoolAlloc>()) Material());
		newMat->_setThisPtr(newMat);
		newMat->initialize();
		newMat->setShader(shader);

		return newMat;
	}
}
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsMaterialManager.h"
#include "BsMaterial.h"

namespace BansheeEngine
{
	MaterialPtr MaterialManager::create() const
	{
		MaterialPtr newMat = bs_core_ptr<Material>(new (bs_alloc<Material>()) Material());
		newMat->_setThisPtr(newMat);
		newMat->initialize();

		return newMat;
	}

	MaterialPtr MaterialManager::create(const HShader& shader) const
	{
		MaterialPtr newMat = bs_core_ptr<Material>(new (bs_alloc<Material>()) Material(shader));
		newMat->_setThisPtr(newMat);
		newMat->initialize();

		return newMat;
	}

	MaterialPtr MaterialManager::createEmpty() const
	{
		MaterialPtr newMat = bs_core_ptr<Material>(new (bs_alloc<Material>()) Material());
		newMat->_setThisPtr(newMat);

		return newMat;
	}
}
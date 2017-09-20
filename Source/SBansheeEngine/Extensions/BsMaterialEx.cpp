//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Extensions/BsMaterialEx.h"
#include "CoreThread/BsCoreThread.h"

namespace bs
{
	void MaterialEx::setTexture(const HMaterial& thisPtr, const String& name, const ResourceHandle<Texture>& value, 
		UINT32 mipLevel, UINT32 numMipLevels, UINT32 arraySlice, UINT32 numArraySlices)
	{
		thisPtr->setTexture(name, value, TextureSurface(mipLevel, numMipLevels, arraySlice, numArraySlices));
	}

	HTexture MaterialEx::getTexture(const HMaterial& thisPtr, const String& name)
	{
		return thisPtr->getTexture(name);
	}
}

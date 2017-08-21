//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Material/BsShaderManager.h"
#include "Resources/BsResources.h"
#include "Importer/BsImporter.h"

namespace bs
{
	HShaderInclude DefaultShaderIncludeHandler::findInclude(const String& name) const
	{
		return Importer::instance().import<ShaderInclude>(name);
	}

	HShaderInclude ShaderManager::findInclude(const String& name) const
	{
		return mIncludeHandler->findInclude(name);
	}
}
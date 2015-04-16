#include "BsShaderManager.h"
#include "BsResources.h"
#include "BsImporter.h"

namespace BansheeEngine
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
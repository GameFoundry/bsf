//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsFreeImgPrerequisites.h"
#include "BsFreeImgImporter.h"

namespace BansheeEngine
{
	extern "C" BS_FREEIMG_EXPORT const String& getPluginName()
	{
		static String pluginName = "FreeImageImporter";
		return pluginName;
	}

	extern "C" BS_FREEIMG_EXPORT void* loadPlugin()
	{
		FreeImgImporter* importer = bs_new<FreeImgImporter>();
		Importer::instance()._registerAssetImporter(importer);

		return nullptr;
	}
}
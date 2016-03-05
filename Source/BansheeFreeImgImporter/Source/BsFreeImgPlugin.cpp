//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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
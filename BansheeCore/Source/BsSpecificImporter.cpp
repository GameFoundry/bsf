//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsSpecificImporter.h"
#include "BsImportOptions.h"
#include "BsResources.h"

namespace BansheeEngine
{
	ImportOptionsPtr SpecificImporter::createImportOptions() const
	{
		return bs_shared_ptr<ImportOptions, PoolAlloc>();
	}

	ConstImportOptionsPtr SpecificImporter::getDefaultImportOptions() const
	{
		if(mDefaultImportOptions == nullptr)
			mDefaultImportOptions = createImportOptions();

		return mDefaultImportOptions;
	}
}
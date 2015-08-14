#include "BsSpecificImporter.h"
#include "BsImportOptions.h"
#include "BsResources.h"

namespace BansheeEngine
{
	ImportOptionsPtr SpecificImporter::createImportOptions() const
	{
		return bs_shared_ptr_new<ImportOptions>();
	}

	ConstImportOptionsPtr SpecificImporter::getDefaultImportOptions() const
	{
		if(mDefaultImportOptions == nullptr)
			mDefaultImportOptions = createImportOptions();

		return mDefaultImportOptions;
	}
}
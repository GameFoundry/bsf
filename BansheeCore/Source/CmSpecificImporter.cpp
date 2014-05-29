#include "CmSpecificImporter.h"
#include "CmImportOptions.h"
#include "CmResources.h"

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
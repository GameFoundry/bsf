#include "CmSpecificImporter.h"
#include "CmImportOptions.h"
#include "CmResources.h"

namespace CamelotFramework
{
	ImportOptionsPtr SpecificImporter::createImportOptions() const
	{
		return cm_shared_ptr<ImportOptions, PoolAlloc>();
	}

	ConstImportOptionsPtr SpecificImporter::getDefaultImportOptions() const
	{
		if(mDefaultImportOptions == nullptr)
			mDefaultImportOptions = createImportOptions();

		return mDefaultImportOptions;
	}
}
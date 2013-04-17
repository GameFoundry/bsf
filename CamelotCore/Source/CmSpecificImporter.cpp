#include "CmSpecificImporter.h"
#include "CmImportOptions.h"
#include "CmResources.h"

namespace CamelotFramework
{
	ImportOptionsPtr SpecificImporter::createImportOptions() const
	{
		return ImportOptionsPtr(CM_NEW(ImportOptions, PoolAlloc) ImportOptions(),
			&MemAllocDeleter<ImportOptions, PoolAlloc>::deleter);
	}

	ConstImportOptionsPtr SpecificImporter::getDefaultImportOptions() const
	{
		if(mDefaultImportOptions == nullptr)
			mDefaultImportOptions = createImportOptions();

		return mDefaultImportOptions;
	}
}
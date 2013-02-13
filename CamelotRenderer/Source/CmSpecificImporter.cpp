#include "CmSpecificImporter.h"
#include "CmImportOptions.h"
#include "CmResources.h"

namespace CamelotEngine
{
	ImportOptionsPtr SpecificImporter::createImportOptions() const
	{
		return ImportOptionsPtr(new ImportOptions());
	}

	ConstImportOptionsPtr SpecificImporter::getDefaultImportOptions() const
	{
		if(mDefaultImportOptions == nullptr)
			mDefaultImportOptions = createImportOptions();

		return mDefaultImportOptions;
	}
}
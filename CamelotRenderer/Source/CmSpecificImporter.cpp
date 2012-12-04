#include "CmSpecificImporter.h"
#include "CmResources.h"

namespace CamelotEngine
{
	void SpecificImporter::registerLoadedResource(BaseResourceHandle resource)
	{
		gResources().instance().registerLoadedResource(resource);
	}
}
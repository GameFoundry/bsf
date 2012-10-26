#include "CmResources.h"
#include "CmResource.h"
#include "CmException.h"
#include "CmFileSerializer.h"

namespace CamelotEngine
{
	Resources::Resources()
	{

	}

	Resources::~Resources()
	{

	}

	BaseResourceRef Resources::load(const String& filePath)
	{
		FileSerializer fs;
		std::shared_ptr<IReflectable> loadedData = fs.decode(filePath);

		// TODO - Low priority. Check is file path valid?

		if(loadedData == nullptr)
			CM_EXCEPT(InternalErrorException, "Unable to load resource.");

		if(!loadedData->isDerivedFrom(Resource::getRTTIStatic()))
			CM_EXCEPT(InternalErrorException, "Loaded class doesn't derive from Resource.");

		ResourcePtr resource = std::static_pointer_cast<Resource>(loadedData);
		resource->load();

		return BaseResourceRef(resource);
	}

	BaseResourceRef Resources::load(const UUID& uuid)
	{
		CM_EXCEPT(NotImplementedException, "Not implemented");
	}

	void Resources::save(BaseResourceRef resource, const String& filePath)
	{
		assert(resource.get() != nullptr);

		// TODO - Low priority. Check is file path valid?
		
		FileSerializer fs;
		fs.encode(resource.get(), filePath);
	}

	CM_EXPORT Resources& gResources()
	{
		return Resources::instance();
	}
}
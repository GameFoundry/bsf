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

	ResourcePtr Resources::load(const String& filePath)
	{
		FileSerializer fs;
		std::shared_ptr<IReflectable> resource = fs.decode(filePath);

		// TODO - Low priority. Check is file path valid?

		if(resource == nullptr)
			CM_EXCEPT(InternalErrorException, "Unable to load resource.");

		if(!resource->isDerivedFrom(Resource::getRTTIStatic()))
			CM_EXCEPT(InternalErrorException, "Loaded class doesn't derive from Resource.");

		return std::static_pointer_cast<Resource>(resource);
	}

	ResourcePtr Resources::load(const UUID& uuid)
	{
		CM_EXCEPT(NotImplementedException, "Not implemented");
	}

	void Resources::save(ResourcePtr resource, const String& filePath)
	{
		assert(resource != nullptr);

		// TODO - Low priority. Check is file path valid?
		
		FileSerializer fs;
		fs.encode(resource.get(), filePath);
	}
}
#include "BsProjectResourceMeta.h"
#include "BsProjectResourceMetaRTTI.h"

namespace BansheeEngine
{
	ProjectResourceMeta::ProjectResourceMeta(const ConstructPrivately& dummy)
	{

	}

	ProjectResourceMetaPtr ProjectResourceMeta::create(const String& uuid, const ResourceMetaDataPtr& resourceMetaData, 
		const ImportOptionsPtr& importOptions)
	{
		ProjectResourceMetaPtr meta = bs_shared_ptr<ProjectResourceMeta>(ConstructPrivately());
		meta->mUUID = uuid;
		meta->mResourceMeta = resourceMetaData;
		meta->mImportOptions = importOptions;

		return meta;
	}

	ProjectResourceMetaPtr ProjectResourceMeta::createEmpty()
	{
		return bs_shared_ptr<ProjectResourceMeta>(ConstructPrivately());
	}

	/************************************************************************/
	/* 								RTTI		                     		*/
	/************************************************************************/

	RTTITypeBase* ProjectResourceMeta::getRTTIStatic()
	{
		return ResourceMetaRTTI::instance();
	}

	RTTITypeBase* ProjectResourceMeta::getRTTI() const
	{
		return ProjectResourceMeta::getRTTIStatic();
	}
}
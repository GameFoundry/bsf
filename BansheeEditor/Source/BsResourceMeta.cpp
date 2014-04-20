#include "BsResourceMeta.h"
#include "BsResourceMetaRTTI.h"

namespace BansheeEngine
{
	ResourceMeta::ResourceMeta(const ConstructPrivately& dummy)
	{

	}

	ResourceMetaPtr ResourceMeta::create(const String& uuid, const ImportOptionsPtr& importOptions)
	{
		ResourceMetaPtr meta = cm_shared_ptr<ResourceMeta>(ConstructPrivately());
		meta->mUUID = uuid;
		meta->mImportOptions = importOptions;

		return meta;
	}

	ResourceMetaPtr ResourceMeta::createEmpty()
	{
		return cm_shared_ptr<ResourceMeta>(ConstructPrivately());
	}

	/************************************************************************/
	/* 								RTTI		                     		*/
	/************************************************************************/

	RTTITypeBase* ResourceMeta::getRTTIStatic()
	{
		return ResourceMetaRTTI::instance();
	}

	RTTITypeBase* ResourceMeta::getRTTI() const
	{
		return ResourceMeta::getRTTIStatic();
	}
}
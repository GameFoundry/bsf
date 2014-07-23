#include "BsResourceMeta.h"
#include "BsResourceMetaRTTI.h"

namespace BansheeEngine
{
	ResourceMeta::ResourceMeta(const ConstructPrivately& dummy)
	{

	}

	ResourceMetaPtr ResourceMeta::create(const String& uuid, const WString& displayName, const ImportOptionsPtr& importOptions)
	{
		ResourceMetaPtr meta = bs_shared_ptr<ResourceMeta>(ConstructPrivately());
		meta->mUUID = uuid;
		meta->mDisplayName = displayName;
		meta->mImportOptions = importOptions;

		return meta;
	}

	ResourceMetaPtr ResourceMeta::createEmpty()
	{
		return bs_shared_ptr<ResourceMeta>(ConstructPrivately());
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
#include "BsProjectResourceMeta.h"
#include "BsProjectResourceMetaRTTI.h"

namespace BansheeEngine
{
	ProjectResourceMeta::ProjectResourceMeta(const ConstructPrivately& dummy)
		:mIncludeInBuild(false), mTypeId(0)
	{

	}

	ProjectResourceMetaPtr ProjectResourceMeta::create(const String& uuid, UINT32 typeId, const ResourceMetaDataPtr& resourceMetaData,
		const ImportOptionsPtr& importOptions)
	{
		ProjectResourceMetaPtr meta = bs_shared_ptr_new<ProjectResourceMeta>(ConstructPrivately());
		meta->mUUID = uuid;
		meta->mTypeId = typeId;
		meta->mResourceMeta = resourceMetaData;
		meta->mImportOptions = importOptions;
		meta->mIncludeInBuild = false;

		return meta;
	}

	ProjectResourceMetaPtr ProjectResourceMeta::createEmpty()
	{
		return bs_shared_ptr_new<ProjectResourceMeta>(ConstructPrivately());
	}

	/************************************************************************/
	/* 								RTTI		                     		*/
	/************************************************************************/

	RTTITypeBase* ProjectResourceMeta::getRTTIStatic()
	{
		return ProjectResourceMetaRTTI::instance();
	}

	RTTITypeBase* ProjectResourceMeta::getRTTI() const
	{
		return ProjectResourceMeta::getRTTIStatic();
	}
}
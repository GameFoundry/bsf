//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsProjectResourceMeta.h"
#include "BsProjectResourceMetaRTTI.h"

namespace BansheeEngine
{
	ProjectResourceMeta::ProjectResourceMeta(const ConstructPrivately& dummy)
		:mTypeId(0)
	{

	}

	ProjectResourceMetaPtr ProjectResourceMeta::create(const WString& name, const String& uuid, UINT32 typeId,
		const ResourceMetaDataPtr& resourceMetaData)
	{
		ProjectResourceMetaPtr meta = bs_shared_ptr_new<ProjectResourceMeta>(ConstructPrivately());
		meta->mName = name;
		meta->mUUID = uuid;
		meta->mTypeId = typeId;
		meta->mResourceMeta = resourceMetaData;

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

	ProjectFileMeta::ProjectFileMeta(const ConstructPrivately& dummy)
		:mIncludeInBuild(false)
	{

	}

	ProjectFileMetaPtr ProjectFileMeta::create(const ImportOptionsPtr& importOptions)
	{
		ProjectFileMetaPtr meta = bs_shared_ptr_new<ProjectFileMeta>(ConstructPrivately());
		meta->mImportOptions = importOptions;
		meta->mIncludeInBuild = false;

		return meta;
	}

	void ProjectFileMeta::add(const ProjectResourceMetaPtr& resourceMeta)
	{
		mResourceMetaData.push_back(resourceMeta);
	}

	void ProjectFileMeta::remove(const String& UUID)
	{
		auto iterFind = std::find_if(mResourceMetaData.begin(), mResourceMetaData.end(),
			[&](auto& x) { return x->getUUID() == UUID; });

		if (iterFind != mResourceMetaData.end())
			mResourceMetaData.erase(iterFind);
	}

	bool ProjectFileMeta::hasTypeId(UINT32 typeId) const
	{
		for(auto& entry : mResourceMetaData)
		{
			if (entry->getTypeID() == typeId)
				return true;
		}

		return false;
	}

	bool ProjectFileMeta::hasUUID(const String& uuid) const
	{
		for (auto& entry : mResourceMetaData)
		{
			if (entry->getUUID() == uuid)
				return true;
		}

		return false;
	}

	ProjectFileMetaPtr ProjectFileMeta::createEmpty()
	{
		return bs_shared_ptr_new<ProjectFileMeta>(ConstructPrivately());
	}

	/************************************************************************/
	/* 								RTTI		                     		*/
	/************************************************************************/

	RTTITypeBase* ProjectFileMeta::getRTTIStatic()
	{
		return ProjectFileMetaRTTI::instance();
	}

	RTTITypeBase* ProjectFileMeta::getRTTI() const
	{
		return ProjectFileMeta::getRTTIStatic();
	}
}
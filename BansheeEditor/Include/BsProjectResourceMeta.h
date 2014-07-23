#pragma once

#include "BsEditorPrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	class ProjectResourceMeta : public IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
		explicit ProjectResourceMeta(const ConstructPrivately&);

		static ProjectResourceMetaPtr create(const String& uuid, const ResourceMetaDataPtr& resourceMetaData,
			const ImportOptionsPtr& importOptions);

		const String& getUUID() const { return mUUID; }
		ResourceMetaDataPtr getResourceMetaData() const { return mResourceMeta; }
		const ImportOptionsPtr& getImportOptions() const { return mImportOptions; }

	private:
		String mUUID;
		ResourceMetaDataPtr mResourceMeta;
		ImportOptionsPtr mImportOptions;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		static ProjectResourceMetaPtr createEmpty();

	public:
		friend class ResourceMetaRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}
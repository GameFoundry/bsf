#pragma once

#include "BsEditorPrerequisites.h"
#include "CmIReflectable.h"

namespace BansheeEditor
{
	class ResourceMeta : public CM::IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
		explicit ResourceMeta(const ConstructPrivately&);

		static ResourceMetaPtr create(const CM::String& uuid, const CM::ImportOptionsPtr& importOptions);

		const CM::String& getUUID() const { return mUUID; }
		const CM::ImportOptionsPtr& getImportOptions() const { return mImportOptions; }

	private:
		CM::String mUUID;
		CM::ImportOptionsPtr mImportOptions;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		static ResourceMetaPtr createEmpty();

	public:
		friend class ResourceMetaRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;	
	};
}
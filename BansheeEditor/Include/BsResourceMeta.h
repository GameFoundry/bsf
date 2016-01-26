#pragma once

#include "BsEditorPrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	class ResourceMeta : public IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
		explicit ResourceMeta(const ConstructPrivately&);

		static ResourceMetaPtr create(const String& uuid, const ImportOptionsPtr& importOptions);

		const String& getUUID() const { return mUUID; }
		const ImportOptionsPtr& getImportOptions() const { return mImportOptions; }

	private:
		String mUUID;
		ImportOptionsPtr mImportOptions;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		static ResourceMetaPtr createEmpty();

	public:
		friend class ResourceMetaRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}
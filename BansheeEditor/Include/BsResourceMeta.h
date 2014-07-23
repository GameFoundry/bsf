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

		static ResourceMetaPtr create(const String& uuid, const WString& displayName, 
			const ImportOptionsPtr& importOptions);

		const String& getUUID() const { return mUUID; }
		const WString& getDisplayName() const { return mDisplayName; }
		const ImportOptionsPtr& getImportOptions() const { return mImportOptions; }

	private:
		String mUUID;
		WString mDisplayName;
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
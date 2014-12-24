#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsSavedResourceData.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT SavedResourceDataRTTI : public RTTIType <SavedResourceData, IReflectable, SavedResourceDataRTTI>
	{
	private:
		String& getDependency(SavedResourceData* obj, UINT32 arrayIdx)
		{
			return obj->mDependencies[arrayIdx];
		}

		void setDependency(SavedResourceData* obj, UINT32 arrayIdx, String& val)
		{
			obj->mDependencies[arrayIdx] = val;
		}

		UINT32 getNumDependencies(SavedResourceData* obj)
		{
			return (UINT32)obj->mDependencies.size();
		}

		void setNumDependencies(SavedResourceData* obj, UINT32 numEntries)
		{
			obj->mDependencies = Vector<String>(numEntries);
		}

		bool& getAllowAsyncLoading(SavedResourceData* obj)
		{
			return obj->mAllowAsync;
		}

		void setAllowAsyncLoading(SavedResourceData* obj, bool& value)
		{
			obj->mAllowAsync = value;
		}

	public:
		SavedResourceDataRTTI()
		{
			addPlainArrayField("mDependencies", 0, &SavedResourceDataRTTI::getDependency, &SavedResourceDataRTTI::getNumDependencies,
				&SavedResourceDataRTTI::setDependency, &SavedResourceDataRTTI::setNumDependencies);
			addPlainField("mAllowAsync", 1, &SavedResourceDataRTTI::getAllowAsyncLoading, &SavedResourceDataRTTI::setAllowAsyncLoading);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ResourceDependencies";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ResourceDependencies;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<SavedResourceData>();
		}
	};
}
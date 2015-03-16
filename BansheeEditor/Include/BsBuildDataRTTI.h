#pragma once

#include "BsEditorPrerequisites.h"
#include "BsRTTIType.h"
#include "BsBuildManager.h"

namespace BansheeEngine
{
	class BuildDataRTTI : public RTTIType <BuildData, IReflectable, BuildDataRTTI>
	{
	private:
		SPtr<PlatformInfo> getPlatformInfo(BuildData* obj, UINT32 idx) { return obj->platformData[idx]; }
		void setPlatformInfo(BuildData* obj, UINT32 idx, SPtr<PlatformInfo> val) { obj->platformData[idx] = val; }
		UINT32 getPlatformInfoArraySize(BuildData* obj) { return (UINT32)obj->platformData.size(); }
		void setPlatformInfoArraySize(BuildData* obj, UINT32 size) { obj->platformData.resize(size); }

		PlatformType& getActivePlatform(BuildData* obj) { return obj->activePlatform; }
		void setActivePlatform(BuildData* obj, PlatformType& val) { obj->activePlatform = val; }

	public:
		BuildDataRTTI()
		{
			addReflectablePtrArrayField("platformData", 0, &BuildDataRTTI::getPlatformInfo, &BuildDataRTTI::getPlatformInfoArraySize,
				&BuildDataRTTI::setPlatformInfo, &BuildDataRTTI::setPlatformInfoArraySize);

			addPlainField("activePlatform", 1, &BuildDataRTTI::getActivePlatform, &BuildDataRTTI::setActivePlatform);
		}

		virtual const String& getRTTIName()
		{
			static String name = "BuildData";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_BuildData;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<BuildData>();
		}
	};
}
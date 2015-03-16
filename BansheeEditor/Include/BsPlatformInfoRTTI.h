#pragma once

#include "BsEditorPrerequisites.h"
#include "BsRTTIType.h"
#include "BsPlatformInfo.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT PlatformInfoRTTI : public RTTIType <PlatformInfo, IReflectable, PlatformInfoRTTI>
	{
	private:
		WString& getDefines(PlatformInfo* obj) { return obj->defines; }
		void setDefines(PlatformInfo* obj, WString& val) { obj->defines = val; }

		PlatformType& getType(PlatformInfo* obj) { return obj->type; }
		void setType(PlatformInfo* obj, PlatformType& val) { obj->type = val; }
	public:
		PlatformInfoRTTI()
		{
			addPlainField("defines", 0, &PlatformInfoRTTI::getDefines, &PlatformInfoRTTI::setDefines);
			addPlainField("type", 1, &PlatformInfoRTTI::getType, &PlatformInfoRTTI::setType);
		}

		virtual const String& getRTTIName()
		{
			static String name = "PlatformInfo";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_PlatformInfo;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<PlatformInfo>();
		}
	};

	class BS_ED_EXPORT WinPlatformInfoRTTI : public RTTIType <WinPlatformInfo, PlatformInfo, WinPlatformInfoRTTI>
	{
	private:
		bool& getIs32Bit(WinPlatformInfo* obj) { return obj->is32bit; }
		void setIs32Bit(WinPlatformInfo* obj, bool& val) { obj->is32bit = val; }
	public:
		WinPlatformInfoRTTI()
		{
			addPlainField("is32bit", 0, &WinPlatformInfoRTTI::getIs32Bit, &WinPlatformInfoRTTI::setIs32Bit);
		}

		virtual const String& getRTTIName()
		{
			static String name = "WinPlatformInfo";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_WinPlatformInfo;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<WinPlatformInfo>();
		}
	};
}
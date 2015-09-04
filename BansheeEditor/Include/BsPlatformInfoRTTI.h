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

		virtual const String& getRTTIName() override
		{
			static String name = "PlatformInfo";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_PlatformInfo;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<PlatformInfo>();
		}
	};

	class BS_ED_EXPORT WinPlatformInfoRTTI : public RTTIType <WinPlatformInfo, PlatformInfo, WinPlatformInfoRTTI>
	{
	private:
		bool& getIs32Bit(WinPlatformInfo* obj) { return obj->is32bit; }
		void setIs32Bit(WinPlatformInfo* obj, bool& val) { obj->is32bit = val; }

		HTexture& getIcon16(WinPlatformInfo* obj) { return obj->icon16; }
		void setIcon16(WinPlatformInfo* obj, HTexture& val) { obj->icon16 = val; }

		HTexture& getIcon32(WinPlatformInfo* obj) { return obj->icon32; }
		void setIcon32(WinPlatformInfo* obj, HTexture& val) { obj->icon32 = val; }

		HTexture& getIcon48(WinPlatformInfo* obj) { return obj->icon48; }
		void setIcon48(WinPlatformInfo* obj, HTexture& val) { obj->icon48 = val; }

		HTexture& getIcon64(WinPlatformInfo* obj) { return obj->icon64; }
		void setIcon64(WinPlatformInfo* obj, HTexture& val) { obj->icon64 = val; }

		HTexture& getIcon96(WinPlatformInfo* obj) { return obj->icon96; }
		void setIcon96(WinPlatformInfo* obj, HTexture& val) { obj->icon96 = val; }

		HTexture& getIcon128(WinPlatformInfo* obj) { return obj->icon128; }
		void setIcon128(WinPlatformInfo* obj, HTexture& val) { obj->icon128 = val; }

		HTexture& getIcon192(WinPlatformInfo* obj) { return obj->icon192; }
		void setIcon192(WinPlatformInfo* obj, HTexture& val) { obj->icon192 = val; }

		HTexture& getIcon256(WinPlatformInfo* obj) { return obj->icon256; }
		void setIcon256(WinPlatformInfo* obj, HTexture& val) { obj->icon256 = val; }

	public:
		WinPlatformInfoRTTI()
		{
			addPlainField("is32bit", 0, &WinPlatformInfoRTTI::getIs32Bit, &WinPlatformInfoRTTI::setIs32Bit);
			addReflectableField("icon16", 1, &WinPlatformInfoRTTI::getIcon16, &WinPlatformInfoRTTI::setIcon16);
			addReflectableField("icon32", 2, &WinPlatformInfoRTTI::getIcon32, &WinPlatformInfoRTTI::setIcon32);
			addReflectableField("icon48", 3, &WinPlatformInfoRTTI::getIcon48, &WinPlatformInfoRTTI::setIcon48);
			addReflectableField("icon64", 4, &WinPlatformInfoRTTI::getIcon64, &WinPlatformInfoRTTI::setIcon64);
			addReflectableField("icon96", 5, &WinPlatformInfoRTTI::getIcon96, &WinPlatformInfoRTTI::setIcon96);
			addReflectableField("icon128", 6, &WinPlatformInfoRTTI::getIcon128, &WinPlatformInfoRTTI::setIcon128);
			addReflectableField("icon192", 7, &WinPlatformInfoRTTI::getIcon192, &WinPlatformInfoRTTI::setIcon192);
			addReflectableField("icon256", 8, &WinPlatformInfoRTTI::getIcon256, &WinPlatformInfoRTTI::setIcon256);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "WinPlatformInfo";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_WinPlatformInfo;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<WinPlatformInfo>();
		}
	};
}
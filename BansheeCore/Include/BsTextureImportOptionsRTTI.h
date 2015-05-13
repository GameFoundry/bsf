#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsTextureImportOptions.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT TextureImportOptionsRTTI : public RTTIType<TextureImportOptions, ImportOptions, TextureImportOptionsRTTI>
	{
	private:
		PixelFormat& getPixelFormat(TextureImportOptions* obj) { return obj->mFormat; }
		void setPixelFormat(TextureImportOptions* obj, PixelFormat& value) { obj->mFormat = value; }

		bool& getGenerateMips(TextureImportOptions* obj) { return obj->mGenerateMips; }
		void setGenerateMips(TextureImportOptions* obj, bool& value) { obj->mGenerateMips = value; }

		UINT32& getMaxMip(TextureImportOptions* obj) { return obj->mMaxMip; }
		void setMaxMip(TextureImportOptions* obj, UINT32& value) { obj->mMaxMip = value; }

		bool& getCPUReadable(TextureImportOptions* obj) { return obj->mCPUReadable; }
		void setCPUReadable(TextureImportOptions* obj, bool& value) { obj->mCPUReadable = value; }

		bool& getSRGB(TextureImportOptions* obj) { return obj->mSRGB; }
		void setSRGB(TextureImportOptions* obj, bool& value) { obj->mSRGB = value; }

	public:
		TextureImportOptionsRTTI()
		{
			addPlainField("mPixelFormat", 0, &TextureImportOptionsRTTI::getPixelFormat, &TextureImportOptionsRTTI::setPixelFormat);
			addPlainField("mGenerateMips", 1, &TextureImportOptionsRTTI::getGenerateMips, &TextureImportOptionsRTTI::setGenerateMips);
			addPlainField("mMaxMip", 2, &TextureImportOptionsRTTI::getMaxMip, &TextureImportOptionsRTTI::setMaxMip);
			addPlainField("mCPUReadable", 3, &TextureImportOptionsRTTI::getCPUReadable, &TextureImportOptionsRTTI::setCPUReadable);
			addPlainField("mSRGB", 4, &TextureImportOptionsRTTI::getSRGB, &TextureImportOptionsRTTI::setSRGB);
		}

		virtual const String& getRTTIName()
		{
			static String name = "TextureImportOptions";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_TextureImportOptions;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<TextureImportOptions, PoolAlloc>();
		}
	};
}
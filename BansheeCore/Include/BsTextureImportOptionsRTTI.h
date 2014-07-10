//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsTextureImportOptions.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT TextureImportOptionsRTTI : public RTTIType<TextureImportOptions, IReflectable, TextureImportOptionsRTTI>
	{
	private:
		PixelFormat& getPixelFormat(TextureImportOptions* obj) { return obj->mFormat; }
		void setPixelFormat(TextureImportOptions* obj, PixelFormat& value) { obj->mFormat = value; }

		bool& getGenerateMips(TextureImportOptions* obj) { return obj->mGenerateMips; }
		void setGenerateMips(TextureImportOptions* obj, bool& value) { obj->mGenerateMips = value; }

		UINT32& getMaxMip(TextureImportOptions* obj) { return obj->mMaxMip; }
		void setMaxMip(TextureImportOptions* obj, UINT32& value) { obj->mMaxMip = value; }

	public:
		TextureImportOptionsRTTI()
		{
			addPlainField("mPixelFormat", 0, &TextureImportOptionsRTTI::getPixelFormat, &TextureImportOptionsRTTI::setPixelFormat);
			addPlainField("mGenerateMips", 1, &TextureImportOptionsRTTI::getGenerateMips, &TextureImportOptionsRTTI::setGenerateMips);
			addPlainField("mMaxMip", 2, &TextureImportOptionsRTTI::getMaxMip, &TextureImportOptionsRTTI::setMaxMip);
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
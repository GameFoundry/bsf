#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmFont.h"
#include "CmFontManager.h"
#include "CmTexture.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT FontDataRTTI : public RTTIType<FontData, Resource, FontDataRTTI>
	{
	private:
		UINT32& getSize(FontData* obj)
		{
			return obj->size;
		}

		void setSize(FontData* obj, UINT32& size)
		{
			obj->size = size;
		}

		FONT_DESC& getFontDesc(FontData* obj)
		{
			return obj->fontDesc;
		}

		void setFontDesc(FontData* obj, FONT_DESC& val)
		{
			obj->fontDesc = val;
		}

		HTexture& getTexture(FontData* obj, UINT32 idx)
		{
			return obj->texturePages.at(idx);
		}

		void setTexture(FontData* obj, UINT32 idx, HTexture& value)
		{
			obj->texturePages[idx] = value;
		}

		UINT32 getTextureArraySize(FontData* obj)
		{
			return (UINT32)obj->texturePages.size();
		}

		void setTextureArraySize(FontData* obj, UINT32 size)
		{
			obj->texturePages.resize(size);
		}

	public:
		FontDataRTTI()
		{
			addPlainField("size", 0, &FontDataRTTI::getSize, &FontDataRTTI::setSize);
			addPlainField("fontDesc", 1, &FontDataRTTI::getFontDesc, &FontDataRTTI::setFontDesc);
			addReflectableArrayField("texturePages", 2, &FontDataRTTI::getTexture, &FontDataRTTI::getTextureArraySize, &FontDataRTTI::setTexture, &FontDataRTTI::setTextureArraySize);
		}

		virtual const String& getRTTIName()
		{
			static String name = "FontData";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_FontData;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<FontData, PoolAlloc>();
		}
	};

	class BS_CORE_EXPORT FontRTTI : public RTTIType<Font, Resource, FontRTTI>
	{
		struct FontInitData
		{
			Vector<FontData> fontDataPerSize;
		};

	private:
		FontData& getFontData(Font* obj, UINT32 idx)
		{
			if(idx >= obj->mFontDataPerSize.size())
				BS_EXCEPT(InternalErrorException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((int)obj->mFontDataPerSize.size()));

			auto iter = obj->mFontDataPerSize.begin();
			for(UINT32 i = 0; i < idx; i++, ++iter)
			{ }

			return iter->second;
		}

		void setFontData(Font* obj, UINT32 idx, FontData& value)
		{
			FontInitData* initData = any_cast<FontInitData*>(obj->mRTTIData);

			initData->fontDataPerSize[idx] = value;
		}

		UINT32 getNumFontData(Font* obj)
		{
			return (UINT32)obj->mFontDataPerSize.size();
		}

		void setNumFontData(Font* obj, UINT32 size)
		{
			FontInitData* initData = any_cast<FontInitData*>(obj->mRTTIData);

			initData->fontDataPerSize.resize(size);
		}

	public:
		FontRTTI()
		{
			addReflectableArrayField("mFontDataPerSize", 0, &FontRTTI::getFontData, &FontRTTI::getNumFontData, &FontRTTI::setFontData, &FontRTTI::setNumFontData);
		}

		virtual const String& getRTTIName()
		{
			static String name = "Font";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Font;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return FontManager::instance()._createEmpty();
		}

	protected:
		virtual void onDeserializationStarted(IReflectable* obj)
		{
			FontInitData* initData = bs_new<FontInitData, PoolAlloc>();

			Font* font = static_cast<Font*>(obj);
			font->mRTTIData = initData;
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			Font* font = static_cast<Font*>(obj);
			FontInitData* initData = any_cast<FontInitData*>(font->mRTTIData);

			font->initialize(initData->fontDataPerSize);

			bs_delete<PoolAlloc>(initData);
		}
	};
}
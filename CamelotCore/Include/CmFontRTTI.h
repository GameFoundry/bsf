#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmFont.h"
#include "CmFontManager.h"
#include "CmTexture.h"

namespace CamelotEngine
{
	class CM_EXPORT FontRTTI : public RTTIType<Font, Resource, FontRTTI>
	{
	private:
		FONT_DESC& getFontDesc(Font* obj)
		{
			return obj->mFontDesc;
		}

		void setFontDesc(Font* obj, FONT_DESC& val)
		{
			obj->mFontDesc = val;
		}

		TexturePtr getTexture(Font* obj, UINT32 idx)
		{
			return obj->mTexturePages.at(idx);
		}

		void setTexture(Font* obj, UINT32 idx, TexturePtr value)
		{
			obj->mTexturePages[idx] = value;
		}

		UINT32 getTextureArraySize(Font* obj)
		{
			return (UINT32)obj->mTexturePages.size();
		}

		void setTextureArraySize(Font* obj, UINT32 size)
		{
			obj->mTexturePages.resize(size);
		}

	public:
		FontRTTI()
		{
			addPlainField("mFontDesc", 0, &FontRTTI::getFontDesc, &FontRTTI::setFontDesc);
			addReflectablePtrArrayField("mTexturePages", 1, &FontRTTI::getTexture, &FontRTTI::getTextureArraySize, &FontRTTI::setTexture, &FontRTTI::setTextureArraySize);
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
			return FontManager::instance().createEmpty();
		}

	protected:
		virtual void onDeserializationEnded(IReflectable* obj)
		{
			Font* font = static_cast<Font*>(obj);
			
			//font->initialize();
		}
	};
}
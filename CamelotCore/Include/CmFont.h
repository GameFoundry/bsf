#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmFontDesc.h"

namespace BansheeEngine
{
	struct CM_EXPORT FontData : public IReflectable
	{
		UINT32 size;
		FONT_DESC fontDesc;
		Vector<HTexture> texturePages;

		const CHAR_DESC& getCharDesc(UINT32 charId) const;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class FontDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	// TODO - When saved on disk font currently stores a copy of the texture pages. This should be acceptable
	// if you import a new TrueType or OpenType font since the texture will be generated on the spot
	// but if you use a bitmap texture to initialize the font manually, then you will potentially have duplicate textures.
	// Also, changing the source texture will not automatically update the font because there is no direct link between them.
	// -- This is probably not a large problem, but it is something to keep an eye out.
	class CM_EXPORT Font : public Resource
	{
	public:
		virtual ~Font();

		void initialize(const Vector<FontData>& fontData);

		const FontData* getFontDataForSize(UINT32 size) const;
		INT32 getClosestAvailableSize(UINT32 size) const;

	protected:
		friend class FontManager;

		Font();

	private:
		Map<UINT32, FontData> mFontDataPerSize;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class FontRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

		/************************************************************************/
		/* 								STATICS		                     		*/
		/************************************************************************/
		
	public:
		static HFont create(const Vector<FontData>& fontInitData);
		static FontPtr _createPtr(const Vector<FontData>& fontInitData);
	};
}
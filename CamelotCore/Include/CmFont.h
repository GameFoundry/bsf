#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmFontDesc.h"

namespace CamelotEngine
{
	// TODO - When saved on disk font currently stores a copy of the texture pages. This should be acceptable
	// if you import a new TrueType or OpenType font since the texture will be generated on the spot
	// but if you use a bitmap texture to initialize the font manually, then you will potentially have duplicate textures.
	// Also, changing the source texture will not automatically update the font because there is no direct link between them.
	// -- This is probably not a large problem, but it is something to keep an eye out.
	class CM_EXPORT Font : public Resource
	{
	public:
		virtual ~Font();

		void initialize(const FONT_DESC& fontDesc, vector<TexturePtr>::type texturePages);

	protected:
		friend class FontManager;

		Font();

	private:
		vector<TexturePtr>::type mTexturePages;
		FONT_DESC mFontDesc;

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
		static FontHandle create(const FONT_DESC& fontDesc, vector<TexturePtr>::type texturePages);
	};
}
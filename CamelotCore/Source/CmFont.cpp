#include "CmFont.h"
#include "CmFontRTTI.h"
#include "CmFontManager.h"

namespace CamelotEngine
{
	Font::Font()
	{

	}

	Font::~Font()
	{

	}

	void Font::initialize(const FONT_DESC& fontDesc, vector<TexturePtr>::type texturePages)
	{
		mFontDesc = fontDesc;
		mTexturePages = texturePages;

		Resource::initialize();
	}

	FontHandle Font::create(const FONT_DESC& fontDesc, vector<TexturePtr>::type texturePages)
	{
		FontPtr newFont = FontManager::instance().create(fontDesc, texturePages);

		return Resource::_createResourceHandle(newFont);
	}

	RTTITypeBase* Font::getRTTIStatic()
	{
		return FontRTTI::instance();
	}

	RTTITypeBase* Font::getRTTI() const
	{
		return Font::getRTTIStatic();
	}
}
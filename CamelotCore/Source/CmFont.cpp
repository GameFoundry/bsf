#include "CmFont.h"
#include "CmFontRTTI.h"
#include "CmFontManager.h"

namespace CamelotEngine
{
	RTTITypeBase* FontData::getRTTIStatic()
	{
		return FontDataRTTI::instance();
	}

	RTTITypeBase* FontData::getRTTI() const
	{
		return FontData::getRTTIStatic();
	}

	Font::Font()
		:Resource(false)
	{ }

	Font::~Font()
	{ }

	void Font::initialize(vector<FontData>::type& fontData)
	{
		for(auto iter = fontData.begin(); iter != fontData.end(); ++iter)
			mFontDataPerSize[iter->size] = *iter;

		Resource::initialize();
	}

	FontHandle Font::create(vector<FontData>::type& fontData)
	{
		FontPtr newFont = FontManager::instance().create(fontData);

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
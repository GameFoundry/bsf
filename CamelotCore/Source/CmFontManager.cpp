#include "CmFontManager.h"
#include "CmFont.h"

namespace CamelotEngine
{
	FontPtr FontManager::create(vector<FontData>::type& fontData) const
	{
		FontPtr newFont(new Font(), &CoreObject::_deleteDelayed);
		newFont->setThisPtr(newFont);
		newFont->initialize(fontData);

		return newFont;
	}

	FontPtr FontManager::createEmpty() const
	{
		FontPtr newFont(new Font(), &CoreObject::_deleteDelayed);
		newFont->setThisPtr(newFont);

		return newFont;
	}
}
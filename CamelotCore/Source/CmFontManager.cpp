#include "CmFontManager.h"
#include "CmFont.h"

namespace CamelotEngine
{
	FontPtr FontManager::create(const FONT_DESC& fontDesc, vector<TexturePtr>::type texturePages) const
	{
		FontPtr newFont(new Font(), &CoreObject::_deleteDelayed);
		newFont->setThisPtr(newFont);
		newFont->initialize(fontDesc, texturePages);

		return newFont;
	}

	FontPtr FontManager::createEmpty() const
	{
		FontPtr newFont(new Font(), &CoreObject::_deleteDelayed);
		newFont->setThisPtr(newFont);

		return newFont;
	}
}
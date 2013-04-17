#include "CmFontManager.h"
#include "CmFont.h"

namespace CamelotFramework
{
	FontPtr FontManager::create(vector<FontData>::type& fontData) const
	{
		FontPtr newFont(CM_NEW(Font, PoolAlloc) Font(), &CoreObject::_deleteDelayed<Font, PoolAlloc>);
		newFont->setThisPtr(newFont);
		newFont->initialize(fontData);

		return newFont;
	}

	FontPtr FontManager::createEmpty() const
	{
		FontPtr newFont(CM_NEW(Font, PoolAlloc) Font(), &CoreObject::_deleteDelayed<Font, PoolAlloc>);
		newFont->setThisPtr(newFont);

		return newFont;
	}
}
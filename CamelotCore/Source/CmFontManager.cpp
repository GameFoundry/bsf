#include "CmFontManager.h"
#include "CmFont.h"

namespace CamelotFramework
{
	FontPtr FontManager::create(Vector<FontData>::type& fontData) const
	{
		FontPtr newFont = cm_core_ptr<Font, PoolAlloc>(new (cm_alloc<Font, PoolAlloc>()) Font());
		newFont->setThisPtr(newFont);
		newFont->initialize(fontData);

		return newFont;
	}

	FontPtr FontManager::createEmpty() const
	{
		FontPtr newFont = cm_core_ptr<Font, PoolAlloc>(new (cm_alloc<Font, PoolAlloc>()) Font());
		newFont->setThisPtr(newFont);

		return newFont;
	}
}
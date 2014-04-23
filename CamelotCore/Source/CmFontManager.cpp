#include "CmFontManager.h"
#include "CmFont.h"

namespace BansheeEngine
{
	FontPtr FontManager::create(const Vector<FontData>::type& fontData) const
	{
		FontPtr newFont = cm_core_ptr<Font, PoolAlloc>(new (cm_alloc<Font, PoolAlloc>()) Font());
		newFont->_setThisPtr(newFont);
		newFont->initialize(fontData);

		return newFont;
	}

	FontPtr FontManager::createEmpty() const
	{
		FontPtr newFont = cm_core_ptr<Font, PoolAlloc>(new (cm_alloc<Font, PoolAlloc>()) Font());
		newFont->_setThisPtr(newFont);

		return newFont;
	}
}
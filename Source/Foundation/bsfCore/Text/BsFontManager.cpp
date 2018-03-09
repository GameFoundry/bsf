//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Text/BsFontManager.h"
#include "Text/BsFont.h"

namespace bs
{
	SPtr<Font> FontManager::create(const Vector<SPtr<FontBitmap>>& fontData) const
	{
		SPtr<Font> newFont = bs_core_ptr<Font>(new (bs_alloc<Font>()) Font());
		newFont->_setThisPtr(newFont);
		newFont->initialize(fontData);

		return newFont;
	}

	SPtr<Font> FontManager::_createEmpty() const
	{
		SPtr<Font> newFont = bs_core_ptr<Font>(new (bs_alloc<Font>()) Font());
		newFont->_setThisPtr(newFont);

		return newFont;
	}
}
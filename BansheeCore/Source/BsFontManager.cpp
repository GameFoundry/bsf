//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsFontManager.h"
#include "BsFont.h"

namespace BansheeEngine
{
	FontPtr FontManager::create(const Vector<SPtr<FontBitmap>>& fontData) const
	{
		FontPtr newFont = bs_core_ptr<Font>(new (bs_alloc<Font>()) Font());
		newFont->_setThisPtr(newFont);
		newFont->initialize(fontData);

		return newFont;
	}

	FontPtr FontManager::_createEmpty() const
	{
		FontPtr newFont = bs_core_ptr<Font>(new (bs_alloc<Font>()) Font());
		newFont->_setThisPtr(newFont);

		return newFont;
	}
}
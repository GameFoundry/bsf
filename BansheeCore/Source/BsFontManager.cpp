//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsFontManager.h"
#include "BsFont.h"

namespace BansheeEngine
{
	FontPtr FontManager::create(const Vector<FontData>& fontData) const
	{
		FontPtr newFont = bs_core_ptr<Font, PoolAlloc>(new (bs_alloc<Font, PoolAlloc>()) Font());
		newFont->_setThisPtr(newFont);
		newFont->initialize(fontData);

		return newFont;
	}

	FontPtr FontManager::_createEmpty() const
	{
		FontPtr newFont = bs_core_ptr<Font, PoolAlloc>(new (bs_alloc<Font, PoolAlloc>()) Font());
		newFont->_setThisPtr(newFont);

		return newFont;
	}
}
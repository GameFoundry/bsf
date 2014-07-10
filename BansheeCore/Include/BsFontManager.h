//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of fonts.
	 */
	class BS_CORE_EXPORT FontManager : public Module<FontManager>
	{
	public:
		/**
		 * @brief	Creates a new font from the provided populated font data structure.
		 */
		FontPtr create(const Vector<FontData>& fontData) const;

		/**
		 * @brief	Creates an empty font.
		 *
		 * @note	Internal method. Used by factory methods.
		 */
		FontPtr _createEmpty() const;
	};
}
#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of fonts.
	 */
	class CM_EXPORT FontManager : public Module<FontManager>
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
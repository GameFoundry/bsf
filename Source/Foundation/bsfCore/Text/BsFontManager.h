//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Utility/BsModule.h"

namespace bs
{
	/** @addtogroup Text-Internal
	 *  @{
	 */

	/**	Handles creation of fonts. */
	class BS_CORE_EXPORT FontManager : public Module<FontManager>
	{
	public:
		/**	Creates a new font from the provided populated font data structure. */
		SPtr<Font> create(const Vector<SPtr<FontBitmap>>& fontData) const;

		/**
		 * Creates an empty font.
		 *
		 * @note	Internal method. Used by factory methods.
		 */
		SPtr<Font> _createEmpty() const;
	};

	/** @} */
}
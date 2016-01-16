#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup Text
	 *  @{
	 */

	/**	Handles creation of fonts. */
	class BS_CORE_EXPORT FontManager : public Module<FontManager>
	{
	public:
		/**	Creates a new font from the provided populated font data structure. */
		FontPtr create(const Vector<SPtr<FontBitmap>>& fontData) const;

		/**
		 * Creates an empty font.
		 *
		 * @note	Internal method. Used by factory methods.
		 */
		FontPtr _createEmpty() const;
	};

	/** @} */
	/** @endcond */
}
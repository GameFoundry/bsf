#pragma once

#include "BsCorePrerequisites.h"
#include "BsImportOptions.h"
#include "BsFont.h"

namespace BansheeEngine
{
	/**
	 * @brief	Import options that allow you to control how is a font
	 *			imported.
	 */
	class BS_CORE_EXPORT FontImportOptions : public ImportOptions
	{
	public:
		FontImportOptions();

		/**
		 * @brief	Sets font sizes that are to be imported. Sizes are in points.
		 */
		void setFontSizes(const Vector<UINT32>& fontSizes) { mFontSizes = fontSizes; }

		/**
		 * @brief	Adds an index range of characters to import. 
		 */
		void addCharIndexRange(UINT32 from, UINT32 to);

		/**
		 * @brief	Clears all character indexes, so no character are imported.
		 */
		void clearCharIndexRanges();

		/**
		 * @brief	Sets dots per inch resolution to use when rendering the characters into the texture.
		 */
		void setDPI(UINT32 dpi) { mDPI = dpi; }

		/**
		 * @brief	Set to true if you want your characters to be antialiased.
		 */
		void setAntialiasing(bool enabled) { mAntialiasing = enabled; }

		/**
		 * @brief	Gets the sizes that are to be imported. Ranges are defined as unicode numbers.
		 */
		Vector<UINT32> getFontSizes() const { return mFontSizes; }

		/**
		 * @brief	Gets character index ranges to import. Ranges are defined as unicode numbers.
		 */
		Vector<std::pair<UINT32, UINT32>> getCharIndexRanges() const { return mCharIndexRanges; }

		/**
		 * @brief	Returns dots per inch scale that will be used when rendering the characters.
		 */
		UINT32 getDPI() const { return mDPI; }

		/**
		 * @brief	Query if antialiasing will be used when rendering the characters.
		 */
		bool getAntialiasing() const { return mAntialiasing; }

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class FontImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;

	private:
		Vector<UINT32> mFontSizes;
		Vector<std::pair<UINT32, UINT32>> mCharIndexRanges;
		UINT32 mDPI;
		bool mAntialiasing;
	};
}
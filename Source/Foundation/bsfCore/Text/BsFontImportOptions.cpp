//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Text/BsFontImportOptions.h"
#include "Private/RTTI/BsFontImportOptionsRTTI.h"

namespace bs
{
	FontImportOptions::FontImportOptions()
		:mDPI(96), mRenderMode(FontRenderMode::HintedSmooth), mBold(false), mItalic(false)
	{
		mFontSizes.push_back(10);
		mCharIndexRanges.push_back(std::make_pair(33, 166)); // Most used ASCII characters
	}

	void FontImportOptions::addCharIndexRange(UINT32 from, UINT32 to)
	{
		mCharIndexRanges.push_back(std::make_pair(from, to));
	}

	void FontImportOptions::clearCharIndexRanges()
	{
		mCharIndexRanges.clear();
	}

	SPtr<FontImportOptions> FontImportOptions::create()
	{
		return bs_shared_ptr_new<FontImportOptions>();
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* FontImportOptions::getRTTIStatic()
	{
		return FontImportOptionsRTTI::instance();
	}

	RTTITypeBase* FontImportOptions::getRTTI() const
	{
		return FontImportOptions::getRTTIStatic();
	}
}
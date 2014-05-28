#include "CmFontImportOptions.h"
#include "CmFontImportOptionsRTTI.h"

namespace BansheeEngine
{
	FontImportOptions::FontImportOptions()
		:mDPI(72), mAntialiasing(true)
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
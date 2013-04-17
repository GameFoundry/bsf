#pragma once

#include "CmPrerequisites.h"
#include "CmImportOptions.h"
#include "CmFont.h"

namespace CamelotFramework
{
	class CM_EXPORT FontImportOptions : public ImportOptions
	{
	public:
		FontImportOptions();

		void setFontSizes(const vector<UINT32>::type& fontSizes) { mFontSizes = fontSizes; }
		void addCharIndexRange(UINT32 from, UINT32 to);
		void clearCharIndexRanges();
		void setDPI(UINT32 dpi) { mDPI = dpi; }

		vector<UINT32>::type getFontSizes() const { return mFontSizes; }
		vector<std::pair<UINT32, UINT32>>::type getCharIndexRanges() const { return mCharIndexRanges; }
		UINT32 getDPI() const { return mDPI; }

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class FontImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	private:
		vector<UINT32>::type mFontSizes;
		vector<std::pair<UINT32, UINT32>>::type mCharIndexRanges;
		UINT32 mDPI;
	};
}
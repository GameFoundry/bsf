#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsFontImportOptions.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT FontImportOptionsRTTI : public RTTIType<FontImportOptions, ImportOptions, FontImportOptionsRTTI>
	{
	private:
		Vector<UINT32>& getFontSizes(FontImportOptions* obj) { return obj->mFontSizes; }
		void setFontSizes(FontImportOptions* obj, Vector<UINT32>& value) { obj->mFontSizes = value; }

		Vector<std::pair<UINT32, UINT32>>& getCharIndexRanges(FontImportOptions* obj) { return obj->mCharIndexRanges; }
		void setCharIndexRanges(FontImportOptions* obj, Vector<std::pair<UINT32, UINT32>>& value) { obj->mCharIndexRanges = value; }

		UINT32& getDPI(FontImportOptions* obj) { return obj->mDPI; }
		void setDPI(FontImportOptions* obj, UINT32& value) { obj->mDPI = value; }

		bool& getAntialiasing(FontImportOptions* obj) { return obj->mAntialiasing; }
		void setAntialiasing(FontImportOptions* obj, bool& value) { obj->mAntialiasing = value; }

	public:
		FontImportOptionsRTTI()
		{
			addPlainField("mFontSizes", 0, &FontImportOptionsRTTI::getFontSizes, &FontImportOptionsRTTI::setFontSizes);
			addPlainField("mCharIndexRanges", 1, &FontImportOptionsRTTI::getCharIndexRanges, &FontImportOptionsRTTI::setCharIndexRanges);
			addPlainField("mDPI", 2, &FontImportOptionsRTTI::getDPI, &FontImportOptionsRTTI::setDPI);
			addPlainField("mAntialiasing", 3, &FontImportOptionsRTTI::getAntialiasing, &FontImportOptionsRTTI::setAntialiasing);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "FontImportOptions";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_FontImportOptions;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<FontImportOptions>();
		}
	};
}
#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmFontImportOptions.h"

namespace BansheeEngine
{
	class CM_EXPORT FontImportOptionsRTTI : public RTTIType<FontImportOptions, IReflectable, FontImportOptionsRTTI>
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

		virtual const String& getRTTIName()
		{
			static String name = "FontImportOptions";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_FontImportOptions;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<FontImportOptions, PoolAlloc>();
		}
	};
}
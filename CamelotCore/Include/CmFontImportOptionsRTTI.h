#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmFontImportOptions.h"

namespace CamelotEngine
{
	class CM_EXPORT FontImportOptionsRTTI : public RTTIType<FontImportOptions, IReflectable, FontImportOptionsRTTI>
	{
	private:


	public:
		FontImportOptionsRTTI()
		{

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
			return ImportOptionsPtr(new FontImportOptions());
		}
	};
}
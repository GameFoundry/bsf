#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsPlainText.h"

namespace BansheeEngine
{
	class BS_EXPORT PlainTextRTTI : public RTTIType <PlainText, Resource, PlainTextRTTI>
	{
	private:
		WString& getString(PlainText* obj) { return obj->mString; }
		void setString(PlainText* obj, WString& val) { obj->mString = val; }
	public:
		PlainTextRTTI()
		{
			addPlainField("mString", 0, &PlainTextRTTI::getString, &PlainTextRTTI::setString);
		}

		virtual const String& getRTTIName()
		{
			static String name = "PlainText";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_PlainText;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return PlainText::_createPtr(L""); // Initial string doesn't matter, it'll get overwritten
		}
	};
}
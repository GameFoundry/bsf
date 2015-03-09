#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsScriptCode.h"

namespace BansheeEngine
{
	class BS_EXPORT ScriptCodeRTTI : public RTTIType <ScriptCode, Resource, ScriptCodeRTTI>
	{
	private:
		WString& getString(ScriptCode* obj) { return obj->mString; }
		void setString(ScriptCode* obj, WString& val) { obj->mString = val; }
	public:
		ScriptCodeRTTI()
		{
			addPlainField("mString", 0, &ScriptCodeRTTI::getString, &ScriptCodeRTTI::setString);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptCode";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ScriptCode;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ScriptCode::_createPtr(L""); // Initial string doesn't matter, it'll get overwritten
		}
	};
}